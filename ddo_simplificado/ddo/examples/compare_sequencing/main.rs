//! Example demonstrating how to compile an exact/relaxed/restricted decision
//! diagram for the single-machine sequencing problem using DDO.
//!
//! Objective (minimisation):
//!   minimise  Σ_j  w_j * C_j        (weighted completion time)
//!
//! We use the separable reformulation of the weighted completion time obtained
//! by exchanging the order of summation:
//!   Σ_j w_j C_j = Σ_i d_i * (Σ_{k>=i} w_{pi_k})
//! where d_i = setup + processing is the duration added at step i. Hence the
//! cost of sequencing `job` from a partial sequence is
//!   d(job) * (W_total - W_scheduled)
//! with d(job) = setup_times[last_job+1][job] + processing_times[job] and
//! W_scheduled the total weight of the jobs scheduled so far. This contribution
//! depends only on the (scheduled set, last_job) state — NOT on the accumulated
//! completion time — which is why the state no longer tracks `current_time`.
//!
//! DDO maximises by default, so — exactly like the Set Cover example — every
//! transition cost is negated and the final objective is multiplied by -1.
//!
//! Mirrors the Python (`SequencingProblem`) and C++ (`SequencingProblem`)
//! implementations. Based on: Cirè & van Hoeve (2013) — Multivalued Decision
//! Diagrams for Sequencing Problems, Operations Research 61(6):1411-1428.

use std::{
    env,
    fs::File,
    io::{BufRead, BufReader},
    path::Path,
    sync::Arc,
    time::Instant,
};

use ddo::*;

const DEPOT: i32 = -1;

// --------------------------------------------------------------------------
// State
// --------------------------------------------------------------------------

/// DP state for the single-machine sequencing problem.
///   - `scheduled`: bitmask of jobs already assigned to a slot (bit j = job j)
///   - `last_job`: index of the previous job (-1 = depot / start of sequence)
///
/// The state key is `(scheduled, last_job)`: with the separable objective the
/// cost of every future transition depends only on which jobs remain and on the
/// last scheduled job, so the accumulated completion time is no longer part of
/// the state. This matches the C++ (`SequencingState`) and Python
/// (`get_state_as_string`) state keys, which were both reduced to
/// `(scheduled, last_job)`.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct SequencingState {
    scheduled: u64,
    last_job: i32,
}

// --------------------------------------------------------------------------
// Problem
// --------------------------------------------------------------------------

pub struct Sequencing {
    n_jobs: usize,
    processing_times: Vec<i64>,
    weights: Vec<i64>,
    setup_times: Vec<Vec<i64>>, // (n_jobs + 1) x n_jobs ; row 0 = from depot
    total_weight: i64,          // Σ_j w_j, used by the separable cost
}

impl Sequencing {
    pub fn new(
        n_jobs: usize,
        processing_times: Vec<i64>,
        weights: Vec<i64>,
        setup_times: Vec<Vec<i64>>,
    ) -> Self {
        let total_weight = weights.iter().sum();
        Sequencing { n_jobs, processing_times, weights, setup_times, total_weight }
    }

    /// Total weight of the jobs in `scheduled` (W_scheduled).
    fn scheduled_weight(&self, scheduled: u64) -> i64 {
        (0..self.n_jobs)
            .filter(|&j| scheduled & (1u64 << j) != 0)
            .map(|j| self.weights[j])
            .sum()
    }

    /// Total processing time of the jobs in `scheduled`. Used as a proxy for
    /// "how far along" a partial sequence is, replacing the accumulated
    /// `current_time` that used to live in the state (mirrors the C++/Python
    /// `scheduled_processing` / `_scheduled_processing` heuristic).
    fn scheduled_processing(&self, scheduled: u64) -> i64 {
        (0..self.n_jobs)
            .filter(|&j| scheduled & (1u64 << j) != 0)
            .map(|j| self.processing_times[j])
            .sum()
    }
}

impl Problem for Sequencing {
    type State = SequencingState;

    fn nb_variables(&self) -> usize {
        self.n_jobs
    }

    fn initial_state(&self) -> Self::State {
        SequencingState { scheduled: 0, last_job: DEPOT }
    }

    fn initial_value(&self) -> isize {
        0
    }

    /// Feasible decisions: every job not yet scheduled. The decision value is
    /// the job index that occupies the current slot.
    fn for_each_in_domain(
        &self,
        variable: Variable,
        state: &Self::State,
        f: &mut dyn DecisionCallback,
    ) {
        for job in 0..self.n_jobs {
            if state.scheduled & (1u64 << job) == 0 {
                f.apply(Decision { variable, value: job as isize });
            }
        }
    }

    /// Append the chosen job to the sequence: update the scheduled set and
    /// remember the last job. The accumulated time is no longer tracked.
    fn transition(&self, state: &Self::State, dec: Decision) -> Self::State {
        let job = dec.value as usize;
        SequencingState {
            scheduled: state.scheduled | (1u64 << job),
            last_job: job as i32,
        }
    }

    /// Separable contribution of sequencing `job` from the source state, negated
    /// so that the maximising solver minimises Σ w_j * C_j:
    ///   d(job) * (W_total - W_scheduled(source))
    /// with d(job) = setup_times[source.last_job + 1][job] + processing_times[job].
    /// Summed along a path this equals Σ w_j * C_j (see module header).
    fn transition_cost(&self, source: &Self::State, _dest: &Self::State, dec: Decision) -> isize {
        let job = dec.value as usize;
        let setup = self.setup_times[(source.last_job + 1) as usize][job];
        let duration = setup + self.processing_times[job];
        let remaining_weight = self.total_weight - self.scheduled_weight(source.scheduled);
        -((duration * remaining_weight) as isize)
    }

    fn next_variable(
        &self,
        depth: usize,
        _: &mut dyn Iterator<Item = &Self::State>,
    ) -> Option<Variable> {
        if depth < self.n_jobs {
            Some(Variable(depth))
        } else {
            None
        }
    }
}

// --------------------------------------------------------------------------
// Relaxation
// --------------------------------------------------------------------------

pub struct SequencingRelax<'a> {
    pb: &'a Sequencing,
}

impl Relaxation for SequencingRelax<'_> {
    type State = SequencingState;

    /// Merge (Cirè & van Hoeve §4):
    ///   - intersect scheduled sets  → fewer committed jobs → more feasible
    ///     completions survive (over-approximation),
    ///   - keep `last_job` from the state with the least committed processing
    ///     (optimistic), mirroring the C++/Python `merge_operator`.
    fn merge(&self, states: &mut dyn Iterator<Item = &Self::State>) -> Self::State {
        let mut merged_sched = u64::MAX;
        let mut best_proc = i64::MAX;
        let mut best_last = DEPOT;

        for s in states {
            merged_sched &= s.scheduled;
            let proc = self.pb.scheduled_processing(s.scheduled);
            if proc < best_proc {
                best_proc = proc;
                best_last = s.last_job;
            }
        }

        SequencingState { scheduled: merged_sched, last_job: best_last }
    }

    fn relax(
        &self,
        _: &Self::State,
        _: &Self::State,
        _: &Self::State,
        _: Decision,
        cost: isize,
    ) -> isize {
        cost
    }
}

// --------------------------------------------------------------------------
// State ranking
// --------------------------------------------------------------------------

pub struct SequencingRanking<'a> {
    pb: &'a Sequencing,
}

impl StateRanking for SequencingRanking<'_> {
    type State = SequencingState;

    /// Higher score = more promising = kept during restriction / merged last.
    /// A smaller committed processing time is more promising (mirrors the
    /// C++/Python discard/merge priority), so the state with the lower
    /// `scheduled_processing` compares as greater.
    fn compare(&self, a: &Self::State, b: &Self::State) -> std::cmp::Ordering {
        let pa = self.pb.scheduled_processing(a.scheduled);
        let pb = self.pb.scheduled_processing(b.scheduled);
        pb.cmp(&pa)
    }
}

// --------------------------------------------------------------------------
// Instance file reader
// --------------------------------------------------------------------------

/// File format (matches Python/C++ implementations):
/// ```text
/// <n_jobs>
/// <p_0> ... <p_{n-1}>            (processing times)
/// <w_0> ... <w_{n-1}>           (weights)
/// <s_00> ... <s_0{n-1}>          (setup times from depot, row 0)
/// <s_10> ...                     (setup times from job 0, row 1)
///   ... (n_jobs + 1 rows total)
/// ```
pub fn read_instance<P: AsRef<Path>>(fname: P) -> Sequencing {
    let file = File::open(fname).expect("Could not open the file");
    let mut reader = BufReader::new(file);
    let mut line = String::new();

    let next_line = |reader: &mut BufReader<File>, line: &mut String| {
        line.clear();
        reader.read_line(line).unwrap();
    };

    // Line 1: n_jobs
    next_line(&mut reader, &mut line);
    let n_jobs: usize = line.trim().parse().unwrap();

    // Line 2: processing times
    next_line(&mut reader, &mut line);
    let processing_times: Vec<i64> =
        line.trim().split_whitespace().map(|x| x.parse().unwrap()).collect();

    // Line 3: weights
    next_line(&mut reader, &mut line);
    let weights: Vec<i64> =
        line.trim().split_whitespace().map(|x| x.parse().unwrap()).collect();

    // n_jobs + 1 rows of setup times
    let mut setup_times = Vec::with_capacity(n_jobs + 1);
    for _ in 0..n_jobs + 1 {
        next_line(&mut reader, &mut line);
        let row: Vec<i64> =
            line.trim().split_whitespace().map(|x| x.parse().unwrap()).collect();
        setup_times.push(row);
    }

    Sequencing::new(n_jobs, processing_times, weights, setup_times)
}

// --------------------------------------------------------------------------
// Main
// --------------------------------------------------------------------------

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 5 {
        eprintln!(
            "Uso: cargo run --example compare_sequencing \
             <input_file> <output_file> <Exact|Relaxed|Restricted> <max_width>"
        );
        std::process::exit(1);
    }

    let input_file = &args[1];
    let output_file = &args[2];
    let comp_type_str = &args[3];
    let max_width: usize = args[4].parse().expect("max_width must be a number");

    let comp_type = match comp_type_str.to_lowercase().as_str() {
        "exact" => CompilationType::Exact,
        "relaxed" => CompilationType::Relaxed,
        "restricted" => CompilationType::Restricted,
        _ => {
            eprintln!("Invalid compilation type. Use: Exact | Relaxed | Restricted");
            std::process::exit(1);
        }
    };

    let file_path = format!("../resources/sequencing/{}", input_file);
    let problem = read_instance(&file_path);
    let relaxation = SequencingRelax { pb: &problem };
    let ranking = SequencingRanking { pb: &problem };

    let mut cache = EmptyCache::default();
    cache.initialize(&problem);
    let dominance = EmptyDominanceChecker::default();

    let residual = SubProblem {
        state: Arc::new(problem.initial_state()),
        value: problem.initial_value(),
        path: vec![],
        ub: isize::MAX,
        depth: 0,
    };

    let input = CompilationInput {
        comp_type,
        problem: &problem,
        relaxation: &relaxation,
        ranking: &ranking,
        cutoff: &NoCutoff,
        max_width,
        residual: &residual,
        best_lb: isize::MIN,
        cache: &cache,
        dominance: &dominance,
    };

    let dd_type_str = match input.comp_type {
        CompilationType::Exact => "Exact",
        CompilationType::Relaxed => "Relaxed",
        CompilationType::Restricted => "Restricted",
    };

    let mut dd = Mdd::<SequencingState, { FRONTIER }>::new();

    let start = Instant::now();
    let result = dd.compile(&input);
    let duration = start.elapsed();

    match result {
        Ok(completion) => {
            use std::fs::OpenOptions;
            use std::io::Write;

            let need_headers = !Path::new(output_file).exists();
            let mut csv_file =
                OpenOptions::new().create(true).append(true).open(output_file).unwrap();

            if need_headers {
                csv_file
                    .write_all(b"File_name,Problem type,Variables number,DD Type,Max Width,Is Reduced,Objective Type,Optimization Value,Construction Time,Reduce Time,Solution Time,Number Node,Number Arcs,Actual Max Width\n")
                    .unwrap();
            }

            // We maximize the negated cost, so the real optimum is -best_value.
            let best_value_str =
                dd.best_value().map_or("N/A".to_string(), |v| (-v).to_string());
            let num_nodes =
                completion.num_nodes.map_or("na".to_string(), |v| v.to_string());
            let num_edges =
                completion.num_edges.map_or("na".to_string(), |v| v.to_string());
            let max_width_real =
                completion.actual_max_width.map_or("na".to_string(), |v| v.to_string());

            let line = format!(
                "{},{},{},{},{},{},{},{},{:.5},{},{},{},{},{}\n",
                input_file,
                "Sequencing",
                problem.nb_variables(),
                dd_type_str,
                input.max_width,
                "false",
                "min",
                best_value_str,
                duration.as_secs_f32(),
                "na",
                "na",
                num_nodes,
                num_edges,
                max_width_real,
            );
            csv_file.write_all(line.as_bytes()).unwrap();
        }
        Err(reason) => {
            println!("Compilation failed: {:?}", reason);
        }
    }
}

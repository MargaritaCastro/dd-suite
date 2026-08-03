
//! Example demonstrating how to compile an exact/relaxed/restricted decision
//! diagram for the Second-Order Cone (SOC) Knapsack Problem using DDO.
//!
//! The feasibility constraint is:
//!   linear_sum + omega * sqrt(sqrt_sum) <= capacity
//!
//! where linear_sum = Σ linear_weight[i]*x[i]  and
//!       sqrt_sum   = Σ (sqrt_weight[i])^2 * x[i]

use std::{
    env,
    fs::File,
    io::{BufRead, BufReader},
    path::Path,
    sync::Arc,
    time::Instant,
};

use ddo::*;

// --------------------------------------------------------------------------
// Decision values
// --------------------------------------------------------------------------
const TAKE_IT: isize = 1;
const LEAVE_IT_OUT: isize = 0;

// --------------------------------------------------------------------------
// Sentinel encoding inside SOCKnapsackState.linear_sum
//   -1  →  only 0-arcs are feasible from this node onward
//   -2  →  all arcs are always feasible from this node onward
// --------------------------------------------------------------------------
const SENTINEL_ONLY_ZEROS: i64 = -1;
const SENTINEL_ALWAYS_FEASIBLE: i64 = -2;

// --------------------------------------------------------------------------
// State
// --------------------------------------------------------------------------

/// DP state for the SOC Knapsack.
/// `linear_sum` and `sqrt_sum` hold the accumulated sums **or** a sentinel.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct SOCKnapsackState {
    depth: usize,
    linear_sum: i64,
    sqrt_sum: i64,
}

// --------------------------------------------------------------------------
// Problem
// --------------------------------------------------------------------------

/// Instance data (already sorted by the variable-ordering heuristic).
pub struct SOCKnapsack {
    nb_variables: usize,
    capacity: i64,
    omega: i64,
    objective_weights: Vec<i64>,
    linear_weights: Vec<i64>,
    sqrt_weights_2: Vec<i64>, // (sqrt_weight[i])^2, sorted
    // Precomputed suffix arrays for pruning (index i → applies to positions i+1..n)
    min_linear: Vec<i64>,
    min_sqrt: Vec<i64>,
    sum_linear: Vec<i64>,
    sum_sqrt: Vec<i64>,
}

impl SOCKnapsack {
    /// Build the problem for one SOC constraint, sorting variables by
    /// (linear_weight + omega * sqrt_weight) in decreasing order.
    pub fn new(
        capacity: i64,
        omega: i64,
        objective_weights: Vec<i64>,
        linear_weights: Vec<i64>,
        sqrt_weights: Vec<i64>,
    ) -> Self {
        let n = objective_weights.len();

        // Prune variables whose linear and sqrt weights are both zero — matches Python's _prune()
        let pruned: Vec<(i64, i64, i64)> = (0..n)
            .filter(|&i| linear_weights[i] != 0 || sqrt_weights[i] != 0)
            .map(|i| (objective_weights[i], linear_weights[i], sqrt_weights[i]))
            .collect();
        let pruned_obj: Vec<i64> = pruned.iter().map(|&(o, _, _)| o).collect();
        let pruned_lin: Vec<i64> = pruned.iter().map(|&(_, l, _)| l).collect();
        let pruned_sqr: Vec<i64> = pruned.iter().map(|&(_, _, s)| s).collect();

        let n = pruned_obj.len();

        // Sort by (linear + omega * sqrt) descending — same heuristic as Python/C++
        let mut indices: Vec<usize> = (0..n).collect();
        indices.sort_unstable_by(|&a, &b| {
            let va = pruned_lin[a] as f64 + omega as f64 * pruned_sqr[a] as f64;
            let vb = pruned_lin[b] as f64 + omega as f64 * pruned_sqr[b] as f64;
            vb.partial_cmp(&va).unwrap_or(std::cmp::Ordering::Equal)
        });

        let obj: Vec<i64> = indices.iter().map(|&i| pruned_obj[i]).collect();
        let lin: Vec<i64> = indices.iter().map(|&i| pruned_lin[i]).collect();
        let sqr2: Vec<i64> = indices.iter().map(|&i| pruned_sqr[i] * pruned_sqr[i]).collect();

        // Suffix min/sum for pruning — position i holds the aggregate over [i+1, n)
        let mut min_linear = vec![0i64; n];
        let mut min_sqrt = vec![0i64; n];
        let mut sum_linear = vec![0i64; n];
        let mut sum_sqrt = vec![0i64; n];

        for i in 0..n.saturating_sub(1) {
            min_linear[i] = *lin[i + 1..].iter().min().unwrap_or(&0);
            min_sqrt[i] = *sqr2[i + 1..].iter().min().unwrap_or(&0);
            sum_linear[i] = lin[i + 1..].iter().sum();
            sum_sqrt[i] = sqr2[i + 1..].iter().sum();
        }

        SOCKnapsack {
            nb_variables: n,
            capacity,
            omega,
            objective_weights: obj,
            linear_weights: lin,
            sqrt_weights_2: sqr2,
            min_linear,
            min_sqrt,
            sum_linear,
            sum_sqrt,
        }
    }

    /// Returns true when (lin, sqr) satisfies the SOC constraint.
    #[inline]
    fn soc_feasible(&self, lin: i64, sqr: i64) -> bool {
        lin as f64 + self.omega as f64 * (sqr as f64).sqrt() <= self.capacity as f64
    }

    /// True when even the cheapest future 1-arc violates the constraint
    /// (so only 0-arcs can be feasible in all remaining layers).
    fn only_zeros_from_here(&self, lin: i64, sqr: i64, vid: usize) -> bool {
        if vid + 1 >= self.nb_variables {
            return false;
        }
        !self.soc_feasible(lin + self.min_linear[vid], sqr + self.min_sqrt[vid])
    }

    /// True when even the most expensive aggregate of future 1-arcs is feasible
    /// (so all future 1-arcs can always be taken).
    fn all_ones_from_here(&self, lin: i64, sqr: i64, vid: usize) -> bool {
        if vid + 1 >= self.nb_variables {
            return false;
        }
        self.soc_feasible(lin + self.sum_linear[vid], sqr + self.sum_sqrt[vid])
    }
}

impl Problem for SOCKnapsack {
    type State = SOCKnapsackState;

    fn nb_variables(&self) -> usize {
        self.nb_variables
    }

    fn initial_state(&self) -> Self::State {
        SOCKnapsackState { depth: 0, linear_sum: 0, sqrt_sum: 0 }
    }

    fn initial_value(&self) -> isize {
        0
    }

    /// Enumerate feasible decisions at the current state.
    /// Sentinel states skip the SOC check; normal states check the 1-arc.
    fn for_each_in_domain(
        &self,
        variable: Variable,
        state: &Self::State,
        f: &mut dyn DecisionCallback,
    ) {
        match state.linear_sum {
            SENTINEL_ONLY_ZEROS => {
                f.apply(Decision { variable, value: LEAVE_IT_OUT });
            }
            SENTINEL_ALWAYS_FEASIBLE => {
                f.apply(Decision { variable, value: TAKE_IT });
                f.apply(Decision { variable, value: LEAVE_IT_OUT });
            }
            _ => {
                let vid = variable.id();
                let new_lin = state.linear_sum + self.linear_weights[vid];
                let new_sqr = state.sqrt_sum + self.sqrt_weights_2[vid];
                if self.soc_feasible(new_lin, new_sqr) {
                    f.apply(Decision { variable, value: TAKE_IT });
                }
                f.apply(Decision { variable, value: LEAVE_IT_OUT });
            }
        }
    }

    /// Compute the successor state.
    /// Sentinel states are absorbing (depth increments, values stay).
    /// Normal states update the sums and then apply pruning.
    fn transition(&self, state: &Self::State, dec: Decision) -> Self::State {
        // Sentinel: absorbing states — only depth changes
        if state.linear_sum == SENTINEL_ONLY_ZEROS
            || state.linear_sum == SENTINEL_ALWAYS_FEASIBLE
        {
            return SOCKnapsackState {
                depth: state.depth + 1,
                linear_sum: state.linear_sum,
                sqrt_sum: state.sqrt_sum,
            };
        }

        let vid = dec.variable.id();

        let (new_lin, new_sqr) = if dec.value == TAKE_IT {
            (
                state.linear_sum + self.linear_weights[vid],
                state.sqrt_sum + self.sqrt_weights_2[vid],
            )
        } else {
            (state.linear_sum, state.sqrt_sum)
        };

        // Pruning: collapse to sentinel when future outcome is determined
        let (final_lin, final_sqr) = if self.only_zeros_from_here(new_lin, new_sqr, vid) {
            (SENTINEL_ONLY_ZEROS, SENTINEL_ONLY_ZEROS)
        } else if new_lin >= 0 && self.all_ones_from_here(new_lin, new_sqr, vid) {
            (SENTINEL_ALWAYS_FEASIBLE, SENTINEL_ALWAYS_FEASIBLE)
        } else {
            (new_lin, new_sqr)
        };

        SOCKnapsackState {
            depth: state.depth + 1,
            linear_sum: final_lin,
            sqrt_sum: final_sqr,
        }
    }

    fn transition_cost(&self, _: &Self::State, _: &Self::State, dec: Decision) -> isize {
        self.objective_weights[dec.variable.id()] as isize * dec.value as isize
    }

    fn next_variable(
        &self,
        depth: usize,
        _: &mut dyn Iterator<Item = &Self::State>,
    ) -> Option<Variable> {
        if depth < self.nb_variables {
            Some(Variable(depth))
        } else {
            None
        }
    }
}

// --------------------------------------------------------------------------
// Relaxation
// --------------------------------------------------------------------------

pub struct SOCKRelax<'a> {
    #[allow(dead_code)]
    pb: &'a SOCKnapsack,
}

impl Relaxation for SOCKRelax<'_> {
    type State = SOCKnapsackState;

    /// Merge operator: element-wise minimum of (linear_sum, sqrt_sum) across
    /// normal states (lower sums = more room = over-approximation). Sentinels:
    ///   - ALWAYS_FEASIBLE dominates (most relaxed).
    ///   - ONLY_ZEROS is the most constrained sentinel.
    fn merge(&self, states: &mut dyn Iterator<Item = &Self::State>) -> Self::State {
        let mut depth = 0;
        let mut best_lin = i64::MAX;
        let mut best_sqr = i64::MAX;
        let mut has_always_feasible = false;
        let mut all_only_zeros = true;

        for s in states {
            depth = s.depth;
            match s.linear_sum {
                SENTINEL_ALWAYS_FEASIBLE => {
                    has_always_feasible = true;
                    all_only_zeros = false;
                }
                SENTINEL_ONLY_ZEROS => { /* all_only_zeros stays true if no normal state */ }
                lin => {
                    all_only_zeros = false;
                    best_lin = best_lin.min(lin);
                    best_sqr = best_sqr.min(s.sqrt_sum);
                }
            }
        }

        if has_always_feasible {
            SOCKnapsackState {
                depth,
                linear_sum: SENTINEL_ALWAYS_FEASIBLE,
                sqrt_sum: SENTINEL_ALWAYS_FEASIBLE,
            }
        } else if all_only_zeros || best_lin == i64::MAX {
            SOCKnapsackState {
                depth,
                linear_sum: SENTINEL_ONLY_ZEROS,
                sqrt_sum: SENTINEL_ONLY_ZEROS,
            }
        } else {
            SOCKnapsackState { depth, linear_sum: best_lin, sqrt_sum: best_sqr }
        }
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

pub struct SOCKRanking;

impl StateRanking for SOCKRanking {
    type State = SOCKnapsackState;

    /// Higher score = more promising = kept during restriction / merged last.
    /// ALWAYS_FEASIBLE is best; ONLY_ZEROS is worst; normal states: lower sums = better.
    fn compare(&self, a: &Self::State, b: &Self::State) -> std::cmp::Ordering {
        fn score(s: &SOCKnapsackState) -> i64 {
            match s.linear_sum {
                SENTINEL_ALWAYS_FEASIBLE => i64::MAX,
                SENTINEL_ONLY_ZEROS => i64::MIN,
                v => -(v + s.sqrt_sum),
            }
        }
        score(a).cmp(&score(b))
    }
}

// --------------------------------------------------------------------------
// Instance file reader
// --------------------------------------------------------------------------

/// Raw data from the SOC Knapsack instance file.
pub struct SOCKnapsackFile {
    pub omega: i64,
    pub objective_weights: Vec<i64>,
    pub right_side: Vec<i64>,
    pub linear_weights: Vec<Vec<i64>>,
    pub sqrt_weights: Vec<Vec<i64>>,
}

/// File format (matches Python/C++ implementations):
/// ```
/// <n_vars>  <n_constraints>
/// <omega>
/// <obj_1> ... <obj_n>
/// <rhs_1> ... <rhs_m>
/// <lin_weights_constraint_0>   (n values)
///   ... (m rows total)
/// <sqrt_weights_constraint_0>  (n values)
///   ... (m rows total)
/// ```
pub fn read_instance<P: AsRef<Path>>(fname: P) -> SOCKnapsackFile {
    let file = File::open(fname).expect("Could not open the file");
    let mut reader = BufReader::new(file);
    let mut line = String::new();

    let next_line = |reader: &mut BufReader<File>, line: &mut String| {
        line.clear();
        reader.read_line(line).unwrap();
    };

    // Line 1: n_vars n_constraints
    next_line(&mut reader, &mut line);
    let mut parts = line.trim().split_whitespace();
    let _nb_vars: usize = parts.next().unwrap().parse().unwrap();
    let nb_constraints: usize = parts.next().unwrap().parse().unwrap();

    // Line 2: omega
    next_line(&mut reader, &mut line);
    let omega: i64 = line.trim().parse().unwrap();

    // Line 3: objective weights
    next_line(&mut reader, &mut line);
    let objective_weights: Vec<i64> =
        line.trim().split_whitespace().map(|x| x.parse().unwrap()).collect();

    // Line 4: right-hand side (one per constraint)
    next_line(&mut reader, &mut line);
    let right_side: Vec<i64> =
        line.trim().split_whitespace().map(|x| x.parse().unwrap()).collect();

    // m rows of linear weights
    let mut linear_weights = Vec::with_capacity(nb_constraints);
    for _ in 0..nb_constraints {
        next_line(&mut reader, &mut line);
        let row: Vec<i64> =
            line.trim().split_whitespace().map(|x| x.parse().unwrap()).collect();
        linear_weights.push(row);
    }

    // m rows of sqrt weights
    let mut sqrt_weights = Vec::with_capacity(nb_constraints);
    for _ in 0..nb_constraints {
        next_line(&mut reader, &mut line);
        let row: Vec<i64> =
            line.trim().split_whitespace().map(|x| x.parse().unwrap()).collect();
        sqrt_weights.push(row);
    }

    SOCKnapsackFile { omega, objective_weights, right_side, linear_weights, sqrt_weights }
}

// --------------------------------------------------------------------------
// Main
// --------------------------------------------------------------------------

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 5 || args.len() > 6 {
        eprintln!(
            "Uso: cargo run --example compare_socknapsack \
             <input_file> <output_file> <Exact|Relaxed|Restricted> <max_width> [constraint_index]"
        );
        std::process::exit(1);
    }

    let input_file = &args[1];
    let output_file = &args[2];
    let comp_type_str = &args[3];
    let max_width: usize = args[4].parse().expect("max_width must be a number");
    let constraint_index: usize = args
        .get(5)
        .map(|s| s.parse().expect("constraint_index must be a number"))
        .unwrap_or(0);

    let comp_type = match comp_type_str.to_lowercase().as_str() {
        "exact" => CompilationType::Exact,
        "relaxed" => CompilationType::Relaxed,
        "restricted" => CompilationType::Restricted,
        _ => {
            eprintln!("Invalid compilation type. Use: Exact | Relaxed | Restricted");
            std::process::exit(1);
        }
    };

    let file_path = format!("../resources/socknapsack/{}", input_file);
    let instance = read_instance(&file_path);

    let problem = SOCKnapsack::new(
        instance.right_side[constraint_index],
        instance.omega,
        instance.objective_weights.clone(),
        instance.linear_weights[constraint_index].clone(),
        instance.sqrt_weights[constraint_index].clone(),
    );
    let relaxation = SOCKRelax { pb: &problem };
    let ranking = SOCKRanking;

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

    let mut dd = Mdd::<SOCKnapsackState, { FRONTIER }>::new();

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

            let best_value_str =
                dd.best_value().map_or("N/A".to_string(), |v| v.to_string());
            let num_nodes =
                completion.num_nodes.map_or("na".to_string(), |v| v.to_string());
            let num_edges =
                completion.num_edges.map_or("na".to_string(), |v| v.to_string());
            let max_width_real =
                completion.actual_max_width.map_or("na".to_string(), |v| v.to_string());

            let line = format!(
                "{},{},{},{},{},{},{},{},{:.5},{},{},{},{},{}\n",
                input_file,
                "SOCKnapsack",
                problem.nb_variables(),
                dd_type_str,
                input.max_width,
                "false",
                "max",
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

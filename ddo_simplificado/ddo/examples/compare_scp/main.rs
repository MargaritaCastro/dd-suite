use std::{sync::Arc, path::Path, fs::File, io::{BufRead, BufReader}, time::Instant};
use ddo::*;
use bit_set::BitSet;
use std::env;

pub struct SCP {
    nb_vars: usize,
    nb_constraints: usize,
    matrix_of_weight: Vec<BitSet>,
    matrix_of_weight_transpose: Vec<BitSet>,
    objective_weights: Vec<usize>,
    max_indices: Vec<usize>,
}

impl SCP {
    pub fn new(
        nb_vars: usize,
        nb_constraints: usize,
        matrix_of_weight: Vec<BitSet>,
        matrix_of_weight_transpose: Vec<BitSet>,
        objective_weights: Vec<usize>,
        max_indices: Vec<usize>,
    ) -> Self {
        SCP {
            nb_vars,
            nb_constraints,
            matrix_of_weight,
            matrix_of_weight_transpose,
            objective_weights,
            max_indices,
        }
    }

    fn is_feasible(
    &self,
    previous_state: &BitSet,
    variable_index: usize,
    ) -> bool {
        for (row, &max_j) in self.max_indices.iter().enumerate() {
            if variable_index >= max_j
                && previous_state.contains(row)
            {
                return false;
            }
        }
        true
    }
}

impl Problem for SCP {
    type State = BitSet;

    fn nb_variables(&self) -> usize {
        self.nb_vars
    }

    fn initial_state(&self) -> Self::State {
        let mut state = BitSet::with_capacity(self.nb_constraints);
        for i in 0..self.nb_constraints {
            state.insert(i);
        }
        state
    }

    fn initial_value(&self) -> isize {
        0
    }

    fn transition(&self, state: &Self::State, decision: Decision) -> Self::State {
        let mut new_state = state.clone();
        if decision.value == 1 {
            new_state.difference_with(&self.matrix_of_weight_transpose[decision.variable.id()]);
        }
        new_state
    }

    fn transition_cost(&self, _: &Self::State, _: &Self::State, decision: Decision) -> isize {
        if decision.value == 1 {
            -(self.objective_weights[decision.variable.id()] as isize)
        } else {
            0
        }
    }

    fn for_each_in_domain(&self, variable: Variable, _state: &Self::State, f: &mut dyn DecisionCallback) {
        let is_feasible = self.is_feasible(_state, variable.id());
        if !is_feasible {
            f.apply(Decision { variable, value: 1 });
        } else {
            f.apply(Decision { variable, value: 1 });
            f.apply(Decision { variable, value: 0 });
        }
    }

    fn next_variable(&self, depth: usize, _: &mut dyn Iterator<Item = &Self::State>) -> Option<Variable> {

        if depth < self.nb_vars {
            Some(Variable(depth))
        } else {
            None
        }
    }
}

// Relaxation: merge estados con intersection
pub struct SCPRelax<'a> {
    pb: &'a SCP,
}

impl Relaxation for SCPRelax<'_> {
    type State = BitSet;

    fn merge(&self, states: &mut dyn Iterator<Item = &Self::State>) -> Self::State {
        let mut iter = states.peekable();
        if let Some(first) = iter.next() {
            let mut merged = first.clone();
            for s in iter {
                merged.intersect_with(s);
            }
            merged
        } else {
            BitSet::with_capacity(self.pb.nb_constraints)
        }
    }

    fn relax(&self, _source: &Self::State, _dest: &Self::State, _merged: &Self::State, _decision: Decision, cost: isize) -> isize {
        cost
    }
}

pub struct SCPRanking;

impl StateRanking for SCPRanking {
    type State = BitSet;

    fn compare(&self, a: &Self::State, b: &Self::State) -> std::cmp::Ordering {
        b.len().cmp(&a.len())
    }
}

/// Reads a Set Covering Problem instance with the same format as the C++ code.
/// 
/// Formato esperado:
/// ```txt
/// <n_variables>
/// <n_restricciones>
/// <peso_1> <peso_2> ... <peso_n>
/// <fila_1>
/// <fila_2>
/// ...
/// <fila_m>
/// ```
/// Each row of the matrix indicates whether variable j covers constraint i or not.
/// Reads an SCP instance from a file, including the coverage-based ordering heuristic.
pub fn read_instance<P: AsRef<Path>>(fname: P) -> SCP {
    let file = File::open(fname).expect("Could not open the file");
    let mut reader = BufReader::new(file);
    let mut line = String::new();

    reader.read_line(&mut line).unwrap();
    let variable_length: usize = line.trim().parse().unwrap();
    line.clear();
    reader.read_line(&mut line).unwrap();
    let restriction_length: usize = line.trim().parse().unwrap();
    line.clear();

    reader.read_line(&mut line).unwrap();
    let objective_weights: Vec<usize> = line
        .trim()
        .split_whitespace()
        .map(|x| x.parse::<usize>().unwrap())
        .collect();
    line.clear();

    let mut raw_matrix = vec![vec![0; variable_length]; restriction_length];
    for i in 0..restriction_length {
        reader.read_line(&mut line).unwrap();
        for (j, val) in line.trim().split_whitespace().enumerate() {
            raw_matrix[i][j] = val.parse::<usize>().unwrap();
        }
        line.clear();
    }

    let mut coverage: Vec<(usize, usize)> = (0..variable_length)
        .map(|j| {
            let count = raw_matrix.iter().filter(|row| row[j] == 1).count();
            (j, count)
        })
        .collect();
    coverage.sort_by(|a, b| b.1.cmp(&a.1));

    let mut matrix_of_weight_transpose = vec![BitSet::with_capacity(restriction_length); variable_length];
    let mut reordered_weights = vec![0; variable_length];

    for (new_idx, (old_idx, _)) in coverage.iter().enumerate() {
        reordered_weights[new_idx] = objective_weights[*old_idx];
        for i in 0..restriction_length {
            if raw_matrix[i][*old_idx] == 1 {
                matrix_of_weight_transpose[new_idx].insert(i);
            }
        }
    }

    let mut matrix_of_weight = vec![BitSet::with_capacity(variable_length); restriction_length];
    for (j, bitset) in matrix_of_weight_transpose.iter().enumerate() {
        for i in bitset.iter() {
            matrix_of_weight[i].insert(j);
        }
    }

    let mut max_indices = vec![0; restriction_length];
    for row in 0..restriction_length {
        for col in (0..variable_length).rev() {
            if matrix_of_weight_transpose[col].contains(row) {
                max_indices[row] = col;
                break;
            }
        }
    }

    SCP::new(variable_length, restriction_length, matrix_of_weight, matrix_of_weight_transpose, reordered_weights, max_indices)
}

fn main() {

    let args: Vec<String> = env::args().collect();
    if args.len() != 5 {
        eprintln!("Uso: cargo run --example compare_scp <input_file> <output_file> <comp_type: Exact|Relaxed|Restricted> <max_width>");
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

    let file_name = input_file; // Use the provided input file name
    let file_path = format!("../resources/scp/{}", file_name);

    let problem = read_instance(&file_path);
    let relaxation = SCPRelax {pb: &problem};
    let ranking = SCPRanking;
    
    // If you think you don't need a cache at all, just replace 'SimpleCache' by 'EmptyCache' in this next line.
    //let mut cache = SimpleCache::default();
    let mut cache = EmptyCache::default();
    cache.initialize(&problem);
    
    let dominance = EmptyDominanceChecker::default();

    let residual = SubProblem {
        state: Arc::new(problem.initial_state()),
        value: problem.initial_value(),
        path: vec![],
        ub: isize::MAX, // Upper bound for the subproblem
        depth: 0,
    };

    let input = CompilationInput {
        comp_type: comp_type,          // The type of compilation you want to perform
        problem: &problem,                  // This is the root problem definition. It provides the 'rules' of your optimization (transition + transition cost)
        relaxation: &relaxation,            // This is the problem relaxation. When compiling an Exact DD, it is not going to be used.
        ranking: &ranking,                  // This is the partial order imposed on problem states. It is only used for merging or restricting layers
        cutoff: &NoCutoff,                  // Unless you want to impose a timeout, you can leave NoCutoff
        max_width: max_width,              // The maximum width of any given layer. It is not going to be used when compiling an exact DD
        residual: &residual,                // The subproblem that will be the root of the compiled DD
        best_lb: isize::MIN,                // The value of the incumbent best known solution
        cache: &cache,                      // The cache (see above)
        dominance: &dominance,              // The dominance checker (see above00)
    };

    let dd_type_str = match input.comp_type {
        CompilationType::Exact => "Exact",
        CompilationType::Relaxed => "Relaxed",
        CompilationType::Restricted => "Restricted",
    };

    let mut dd = Mdd::<BitSet, {FRONTIER}>::new();
    
    let start = Instant::now();
    let compilation_result = dd.compile(&input);
    let duration = start.elapsed();

    match compilation_result {
        Ok(completion) => {
            // Write the result to a CSV file
            use std::fs::OpenOptions;
            use std::io::Write;

            let file_path = output_file; // Use the provided output file name
            let need_headers = !Path::new(file_path).exists();
            let mut csv_file = OpenOptions::new().create(true).append(true).open(file_path).unwrap();

            if need_headers {
                csv_file.write_all(b"File_name,Problem type,Variables number,DD Type,Max Width,Is Reduced,Objective Type,Optimization Value,Construction Time,Reduce Time,Solution Time,Number Node,Number Arcs,Actual Max Width\n").unwrap();
            }

            let mut csv_file = OpenOptions::new()
                .create(true)
                .append(true)
                .open(file_path)
                .expect("Cannot open results.csv");

            // Since we are maximizing, the value has to be multiplied by -1 to turn it into a minimization
            let best_value_str = dd.best_value()
                .map_or("N/A".to_string(), |v| (-v).to_string());

            let num_nodes = completion.num_nodes.map_or("na".to_string(), |v| v.to_string());
            let num_edges = completion.num_edges.map_or("na".to_string(), |v| v.to_string());
            let max_width_real = completion.actual_max_width.map_or("na".to_string(), |v| v.to_string());

            let line = format!(
                "{},{},{},{},{},{},{},{},{:.5},{},{},{},{},{}\n",
                file_name,               // File_name
                "Set Covering",          // Problem type
                problem.nb_variables(),  // Variables number
                dd_type_str,             // DD Type
                input.max_width,         // Max Width
                "false",                 // Is Reduced
                "min",                   // Objective Type
                best_value_str,          // Optimization Value
                duration.as_secs_f32(),  // Construction Time
                "na",                    // Reduce Time
                "na",                    // Solution Time
                num_nodes,               // Number Node
                num_edges,               // Number Arcs
                max_width_real           // Actual Max Width
            );

            csv_file.write_all(line.as_bytes()).unwrap();

        },
        Err(reason) => {
            println!("Compilation failed: {:?}", reason);
        }
    }

}
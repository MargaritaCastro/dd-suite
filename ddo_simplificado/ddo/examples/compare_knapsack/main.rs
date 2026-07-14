
//! Example for Antonia Blanco demonstrating how to compile an exact decision diagram using DDO.
//! 
//! Note: 
//! If that is of any interest to you, most of this example has been copy-pasted from the 'visualization'
//! example provided in the repository of the ddo project. 
//! 

use std::env;

use std::{path::Path, fs::File, io::{BufReader, BufRead}, time::{Duration, Instant}, num::ParseIntError, sync::Arc};

use ddo::*;

/// In our DP model, we consider a state that simply consists of the remaining 
/// capacity in the knapsack. Additionally, we also consider the *depth* (number
/// of assigned variables) as part of the state since it useful when it comes to
/// determine the next variable to branch on.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct KnapsackState {
    /// the number of variables that have already been decided upon in the complete
    /// problem.
    depth: usize,
    /// the remaining capacity in the knapsack. That is the maximum load the sack
    /// can bear without cracking **given what is already in the sack**.
    capacity: usize
}

/// This structure represents a particular instance of the knapsack problem.
/// This is the structure that will implement the knapsack model.
/// 
/// The problem definition is quite easy to understand: there is a knapsack having 
/// a maximum (weight) capacity, and a set of items to chose from. Each of these
/// items having a weight and a profit, the goal is to select the best subset of
/// the items to place them in the sack so as to maximize the profit.
pub struct Knapsack {
    /// The maximum capacity of the sack (when empty)
    capacity: usize,
    /// the profit of each item
    profit: Vec<isize>,
    /// the weight of each item.
    weight: Vec<usize>,
}

/// For each variable in the decision problem, there are two possible choices:
/// either we take the item in the sack, or we decide to leave it out. This
/// constant is used to indicate that the item is to be taken in the sack.
const TAKE_IT: isize = 1;
/// For each variable in the decision problem, there are two possible choices:
/// either we take the item in the sack, or we decide to leave it out. This
/// constant is used to indicate that the item is to be left out of the sack.
const LEAVE_IT_OUT: isize = 0;

/// This is how you implement the labeled transition system (LTS) semantics of
/// a simple dynamic program solving the knapsack problem. The definition of
/// each of the methods should be pretty clear and easy to grasp. Should you
/// want more details on the role of each of these methods, then you are 
/// encouraged to go checking the documentation of the `Problem` trait.
impl Problem for Knapsack {
    type State = KnapsackState;

    fn nb_variables(&self) -> usize {
        self.profit.len()
    }
    fn for_each_in_domain(&self, variable: Variable, state: &Self::State, f: &mut dyn DecisionCallback)
    {
        if state.capacity >= self.weight[variable.id()] {
            f.apply(Decision { variable, value: TAKE_IT });
            f.apply(Decision { variable, value: LEAVE_IT_OUT });
        } else {
            f.apply(Decision { variable, value: LEAVE_IT_OUT });
        }
    }
    fn initial_state(&self) -> Self::State {
        KnapsackState{ depth: 0, capacity: self.capacity }
    }
    fn initial_value(&self) -> isize {
        0
    }
    fn transition(&self, state: &Self::State, dec: Decision) -> Self::State {
        let mut ret = *state;
        ret.depth  += 1;
        if dec.value == TAKE_IT { 
            ret.capacity -= self.weight[dec.variable.id()] 
        }
        ret
    }
    fn transition_cost(&self, _state: &Self::State, _: &Self::State, dec: Decision) -> isize {
        self.profit[dec.variable.id()] as isize * dec.value
    }

    fn next_variable(&self, depth: usize, _: &mut dyn Iterator<Item = &Self::State>) -> Option<Variable> {
        let n = self.nb_variables();
        if depth < n {
            Some(Variable(depth))
        } else {
            None
        }
    }
}

/// This default implementation only serves the purpose of providing an hardcoded instance
impl Knapsack {
    pub fn new(capacity: usize, profit: Vec<isize>, weight: Vec<usize>) -> Self {
        Knapsack { capacity, profit, weight }
    }
}

/// In addition to a dynamic programming (DP) model of the problem you want to solve, 
/// the branch and bound with MDD algorithm (and thus ddo) requires that you provide
/// an additional relaxation allowing to control the maximum amount of space used by
/// the decision diagrams that are compiled. 
/// 
/// That relaxation requires two operations: one to merge several nodes into one 
/// merged node that acts as an over approximation of the other nodes. The second
/// operation is used to possibly offset some weight that would otherwise be lost 
/// to the arcs entering the newly created merged node.
/// 
/// The role of this very simple structure is simply to provide an implementation
/// of that relaxation.
/// 
/// # Note:
/// In addition to the aforementioned two operations, the KPRelax structure implements
/// an optional `fast_upper_bound` method. Which one provides a useful bound to 
/// prune some portions of the state-space as the decision diagrams are compiled.
/// (aka rough upper bound pruning).
pub struct KPRelax<'a>{pub pb: &'a Knapsack}
impl Relaxation for KPRelax<'_> {
    type State = KnapsackState;

    fn merge(&self, states: &mut dyn Iterator<Item = &Self::State>) -> Self::State {
        states.max_by_key(|node| node.capacity).copied().unwrap()
    }

    fn relax(&self, _source: &Self::State, _dest: &Self::State, _merged: &Self::State, _decision: Decision, cost: isize) -> isize {
        cost
    }

    /* 
    fn fast_upper_bound(&self, state: &Self::State) -> isize {
        let mut tot = 0;
        for var in state.depth..self.pb.nb_variables() {
            if self.pb.weight[var] <= state.capacity {
                tot += self.pb.profit[var];
            }
        }
        tot as isize
    }
    */
}

/// This enumeration simply groups the kind of errors that might occur when parsing a
/// knapsack instance from file. There can be io errors (file unavailable ?), format error
/// (e.g. the file is not a knapsack instance but contains the text of your next paper), 
/// or parse int errors (which are actually a variant of the format error since it tells 
/// you that the parser expected an integer number but got ... something else).
#[derive(Debug, thiserror::Error)]
pub enum Error {
    /// There was an io related error
    #[error("io error {0}")]
    Io(#[from] std::io::Error),
    /// The parser expected to read something that was an integer but got some garbage
    #[error("parse int {0}")]
    ParseInt(#[from] ParseIntError),
    /// The file was not properly formatted.
    #[error("ill formed instance")]
    Format,
}

pub fn read_instance<P: AsRef<Path>>(fname: P) -> Result<Knapsack, Error> {
    let f = File::open(fname)?;
    let f = BufReader::new(f);
    
    let mut is_first = true;
    let mut n = 0;
    let mut count = 0;
    let mut capa = 0;
    let mut profit = vec![];
    let mut weight = vec![];

    for line in f.lines() {
        let line = line?;
        if line.starts_with('c') {
            continue;
        }
        if is_first {
            is_first = false;
            let mut ab = line.split(' ');
            n = ab.next().ok_or(Error::Format)?.parse()?;
            capa = ab.next().ok_or(Error::Format)?.parse()?;
        } else {
            if count >= n {
                break;
            }
            let mut ab = line.split(' ');
            profit.push(ab.next().ok_or(Error::Format)?.parse()?);
            weight.push(ab.next().ok_or(Error::Format)?.parse()?);
            count += 1;
        }
    }
    Ok(Knapsack::new(capa, profit, weight))
}

/// The last bit of information which we need to provide when implementing a ddo-based
/// solver is a `StateRanking`. This is an heuristic which is used to select the most
/// and least promising nodes as a means to only delete/merge the *least* promising nodes
/// when compiling restricted and relaxed DDs.
pub struct KPranking;
impl StateRanking for KPranking {
    type State = KnapsackState;

    fn compare(&self, a: &Self::State, b: &Self::State) -> std::cmp::Ordering {
        a.capacity.cmp(&b.capacity)
    }
}

fn write_dot_file(dot_representation: String, file_name: &str) {
    use std::io::Write;
    let mut file = File::create(file_name).unwrap();
    file.write_all(dot_representation.as_bytes()).unwrap();
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 5 {
        eprintln!("Uso: cargo run --example compare <input_file> <output_file> <comp_type: Exact|Relaxed|Restricted> <max_width>");
        std::process::exit(1);
    }

    let input_file = &args[1];
    let output_file = &args[2];
    let comp_type_str = &args[3];
    let max_width: usize = args[4].parse().expect("max_width debe ser un número");

    let comp_type = match comp_type_str.to_lowercase().as_str() {
        "exact" => CompilationType::Exact,
        "relaxed" => CompilationType::Relaxed,
        "restricted" => CompilationType::Restricted,
        _ => {
            eprintln!("Tipo de compilación inválido. Usa: Exact | Relaxed | Restricted");
            std::process::exit(1);
        }
    };

    //let file_name = "f6_l-d_kp_10_60";
    let file_name = input_file; // Use the provided input file name
    let file_path = format!("../resources/knapsack/{}", file_name);
    let problem = read_instance(&file_path).unwrap();
    let relaxation = KPRelax{pb: &problem};
    let ranking = KPranking;
    
    // If you think you don't need a cache at all, just replace 'SimpleCache' by 'EmptyCache' in this next line.
    //let mut cache = SimpleCache::default();
    let mut cache = EmptyCache::default();
    cache.initialize(&problem);
    
    let dominance = EmptyDominanceChecker::default();
    
    let residual = SubProblem { 
        state: Arc::new(problem.initial_state()), 
        value: 0, 
        path: vec![], 
        ub: isize::MAX, 
        depth: 0
     };

    let input = CompilationInput {
        //comp_type: CompilationType::Relaxed,  // << !!! THIS IS WHAT YOU ARE INTERESTED IN !!! You can choose betweeb Exact, Relaxed and Restricted
        comp_type: comp_type,          // The type of compilation you want to perform
        problem: &problem,                  // This is the root problem definition. It provides the 'rules' of your optimization (transition + transition cost)
        relaxation: &relaxation,            // This is the problem relaxation. When compiling an Exact DD, it is not going to be used.
        ranking: &ranking,                  // This is the partial order imposed on problem states. It is only used for merging or restricting layers
        cutoff: &NoCutoff,                  // Unless you want to impose a timeout, you can leave NoCutoff
        //max_width: 3852,                    // << !!! THIS IS WHAT YOU ARE INTERESTED IN !!! This is the maximum width of any given layer. It is not going to be used when compiling an exact DD
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

    let mut dd = Mdd::<KnapsackState, {FRONTIER}>::new();
    
    let start = Instant::now();
    let compilation_result = dd.compile(&input);
    let duration = start.elapsed();

    match compilation_result {
        Ok(completion) => {
            /* 
            println!("Compilation successful!");
            println!("Duration:   {:.5} seconds", duration.as_secs_f32());
            println!("Is exact: {}", dd.is_exact());

            // Imprime el valor óptimo si existe
            if let Some(value) = dd.best_value() {
                println!("Best value: {}", value);
            } else {
                println!("No feasible solution found.");
            }

            // Imprime la mejor solución si existe
            if let Some(solution) = dd.best_solution() {
                println!("Best solution: {:?}", solution);
            } else {
                println!("No feasible solution found.");
            }

            // Imprimir la representación DOT del diagrama de decisión
            let config = VizConfigBuilder::default()
                .show_deleted(true)
                .group_merged(true)
                .build()
                .unwrap();
            let dot_representation = dd.as_graphviz(&config);
            write_dot_file(dot_representation, "knapsack.dot");
            */
            
            // Imprimir el resultado en un archivo CSV
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

            let best_value_str = dd.best_value()
                .map_or("N/A".to_string(), |v| v.to_string());

            let num_nodes = completion.num_nodes.map_or("na".to_string(), |v| v.to_string());
            let num_edges = completion.num_edges.map_or("na".to_string(), |v| v.to_string());
            let max_width_real = completion.actual_max_width.map_or("na".to_string(), |v| v.to_string());

            let line = format!(
                "{},{},{},{},{},{},{},{},{:.5},{},{},{},{},{}\n",
                file_name,               // File_name
                "Knapsack",              // Problem type
                problem.nb_variables(),  // Variables number
                dd_type_str,         // DD Type (puedes cambiarlo por Exact si corresponde)
                input.max_width,         // Max Width
                "false",                 // Is Reduced
                "max",                   // Objective Type
                best_value_str,          // Optimization Value
                duration.as_secs_f32(),  // Construction Time
                "na",                    // Reduce Time
                "na",                    // Solution Time
                num_nodes,                    // Number Node
                num_edges,                    // Number Arcs
                max_width_real                     // Actual Max Width
            );

            csv_file.write_all(line.as_bytes()).unwrap();

        },
        Err(reason) => {
            println!("Compilation failed: {:?}", reason);
        }
    }

}


// Copyright 2020 Xavier Gillard

//! This example show how to implement a solver for the maximum independent set problem 
//! using ddo. It is a fairly simple example but it features most of the aspects you will
//! want to copy when implementing your own solver.
use std::{path::Path, fs::File, io::{BufReader, BufRead}, num::ParseIntError, time::{Duration, Instant}, sync::Arc};

use bit_set::BitSet;
use clap::Parser;
use ddo::*;
use regex::Regex;

use std::env;


/// This structure represents an instance of the Maximum Independent Set Problem. 
/// It is this structure that implements a simple dynamic programming model for the
/// MISP. In that model, the state is simply a bitset where each bit represents 
/// a node that may be kept or left out of the MIS. 
pub struct Misp {
    /// The number of variables in the problem instance
    nb_vars: usize,
    /// For each vertex 'i' of the original graph, the field 'neighbors[i]' contains
    /// a bitmask representing the COMPLEMENT of the adjacency list of i in the 
    /// original graph. While this may seem a complicated take on the representation
    /// of this problem instance, using the complement is helpful as it allows to
    /// easily remove all the neighbors of a vertex from a state very efficiently.
    neighbors: Vec<BitSet>,
    /// For each vertex 'i', the value of 'weight[i]' denotes the weight associated
    /// to vertex i in the problem instance. The goal of MISP is to select the nodes
    /// from the underlying graph such that the resulting set is an independent set
    /// where the sum of the weights of selected vertices is maximum.
    weight: Vec<isize>,
}

/// A constant to mean take the node in the independent set.
const YES: isize = 1;
/// A constant to mean leave the node out of the independent set.
const NO: isize = 0;

/// The Misp class implements the 'Problem' trait. This means Misp is the definition
/// of the DP model. That DP model is pretty straightforward, still you might want
/// to check the implementation of the branching heuristic (next_variable method)
/// since it does interesting stuffs. 
impl Problem for Misp {
    type State = BitSet;

    fn nb_variables(&self) -> usize {
        self.nb_vars
    }

    fn initial_state(&self) -> Self::State {
        (0..self.nb_variables()).collect()
    }

    fn initial_value(&self) -> isize {
        0
    }

    fn transition(&self, state: &Self::State, decision: Decision) -> Self::State {
        let mut res = state.clone();
        res.remove(decision.variable.id());
        if decision.value == YES {
            // intersect with complement of the neighbors for fast set difference
            res.intersect_with(&self.neighbors[decision.variable.id()]); 
        }
        res
    }

    fn transition_cost(&self, _: &Self::State, _: &Self::State, decision: Decision) -> isize {
        if decision.value == NO {
            0
        } else {
            self.weight[decision.variable.id()]
        }
    }

    fn for_each_in_domain(&self, variable: Variable, state: &Self::State, f: &mut dyn DecisionCallback) {
        if state.contains(variable.id()) {
            f.apply(Decision{variable, value: YES});
            f.apply(Decision{variable, value: NO });
        } else {
            f.apply(Decision{variable, value: NO });
        }
    }

    /// This method is (apparently) a bit more hairy. What it does is it simply decides to branch on
    /// the variable that occurs in the least number of states present in the next layer. The intuition
    /// here is to limit the max width as much as possible when developing the layers since all 
    /// nodes that are not impacted by the change on the selectd vertex are simply copied over to the
    /// next layer.
    fn next_variable(&self, depth: usize, _: &mut dyn Iterator<Item = &Self::State>) -> Option<Variable> {
        if depth < self.nb_vars {
            Some(Variable(depth))
        } else {
            None
        }
    }

    fn is_impacted_by(&self, var: Variable, state: &Self::State) -> bool {
        state.contains(var.id())
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
/// In addition to the aforementioned two operations, the MispRelax structure implements
/// an optional `fast_upper_bound` method. Which one provides a useful bound to 
/// prune some portions of the state-space as the decision diagrams are compiled.
/// (aka rough upper bound pruning).
pub struct MispRelax<'a>{pb: &'a Misp}
impl Relaxation for MispRelax<'_> {
    type State = BitSet;

    fn merge(&self, states: &mut dyn Iterator<Item = &Self::State>) -> Self::State {
        let mut state = BitSet::with_capacity(self.pb.nb_variables());
        for s in states {
            state.union_with(s);
        }
        state
    }

    fn relax(
        &self,
        _source: &Self::State,
        _dest: &Self::State,
        _new: &Self::State,
        _decision: Decision,
        cost: isize,
    ) -> isize {
        cost
    }
}


/// The last bit of information which we need to provide when implementing a ddo-based
/// solver is a `StateRanking`. This is an heuristic which is used to select the most
/// and least promising nodes as a means to only delete/merge the *least* promising nodes
/// when compiling restricted and relaxed DDs.
pub struct MispRanking;
impl StateRanking for MispRanking {
    type State = BitSet;

    fn compare(&self, a: &Self::State, b: &Self::State) -> std::cmp::Ordering {
        a.len().cmp(&b.len())
            .then_with(|| a.cmp(b))
    }
}


// #########################################################################################
// # THE INFORMATION BEYOND THIS LINE IS NOT DIRECTLY RELATED TO THE IMPLEMENTATION OF     #
// # A SOLVER BASED ON DDO. INSTEAD, THAT PORTION OF THE CODE CONTAINS GENERIC FUNCTION    #
// # THAT ARE USED TO READ AN INSTANCE FROM FILE, PROCESS COMMAND LINE ARGUMENTS, AND      #
// # THE MAIN FUNCTION. THESE ARE THUS NOT REQUIRED 'PER-SE', BUT I BELIEVE IT IS USEFUL   #
// # TO SHOW HOW IT CAN BE DONE IN AN EXAMPLE.                                             #
// #########################################################################################

/// This enumeration simply groups the kind of errors that might occur when parsing a
/// misp instance from file. There can be io errors (file unavailable ?), format error
/// (e.g. the file is not an instance but contains the text of your next paper), 
/// or parse int errors (which are actually a variant of the format error since it tells 
/// you that the parser expected an integer number but got ... something else).
#[derive(Debug, thiserror::Error)]
enum Error {
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

/// This function is used to read a misp instance from file. It returns either a
/// misp instance if everything went on well or an error describing the problem.
fn read_instance<P: AsRef<Path>>(fname: P) -> Result<Misp, Error> {
    let f = File::open(fname)?;
    let f = BufReader::new(f);
    
    let comment   = Regex::new(r"^c\s.*$").unwrap();
    let pb_decl   = Regex::new(r"^p\s+edge\s+(?P<vars>\d+)\s+(?P<edges>\d+)$").unwrap();
    let node_decl = Regex::new(r"^n\s+(?P<node>\d+)\s+(?P<weight>-?\d+)").unwrap();
    let edge_decl = Regex::new(r"^e\s+(?P<src>\d+)\s+(?P<dst>\d+)").unwrap();

    let mut g = Misp{nb_vars: 0, neighbors: vec![], weight: vec![]};
    for line in f.lines() {
        let line = line?;
        let line = line.trim();

        if line.is_empty() {
            continue;
        }

        if comment.is_match(line) {
            continue;
        }

        if let Some(caps) = pb_decl.captures(line) {
            let n = caps["vars"].to_string().parse::<usize>()?;
            let full = (0..n).collect(); 

            g.nb_vars    = n;
            g.neighbors  = vec![full; n];
            g.weight     = vec![1; n];
            continue;
        }

        if let Some(caps) = node_decl.captures(line) {
            let n = caps["node"].to_string().parse::<usize>()?;
            let w = caps["weight"].to_string().parse::<isize>()?;

            let n = n - 1;
            g.weight[n] = w;
            continue;
        }

        if let Some(caps) = edge_decl.captures(line) {
            let src = caps["src"].to_string().parse::<usize>()?;
            let dst = caps["dst"].to_string().parse::<usize>()?;

            let src = src-1;
            let dst = dst-1;

            g.neighbors[src].remove(dst);
            g.neighbors[dst].remove(src);

            continue;
        }

        // skip
        return Err(Error::Format)
    }

    // Sort variables by degree descending (stable: ties keep original file order),
    // matching Python/C++ behavior.
    let n = g.nb_vars;
    let degree: Vec<usize> = (0..n).map(|i| n - g.neighbors[i].len()).collect();

    let mut order: Vec<usize> = (0..n).collect();
    order.sort_by(|&a, &b| degree[b].cmp(&degree[a])); // stable in Rust

    let mut old_to_new = vec![0usize; n];
    for (new_idx, &old_idx) in order.iter().enumerate() {
        old_to_new[old_idx] = new_idx;
    }

    let new_weight: Vec<isize> = order.iter().map(|&old| g.weight[old]).collect();
    let new_neighbors: Vec<BitSet> = order.iter().map(|&old_idx| {
        let mut bs = BitSet::with_capacity(n);
        for old_nb in g.neighbors[old_idx].iter() {
            bs.insert(old_to_new[old_nb]);
        }
        bs
    }).collect();

    g.weight    = new_weight;
    g.neighbors = new_neighbors;

    // Imprime índices originales (0-based) en orden de procesamiento del DD
    let order_1based: Vec<usize> = order.iter().map(|&i| i + 1).collect();
    println!("[RUST][MISP] Variable order ({} vars): {:?}", n, order_1based);

    Ok(g)
}

/// This is your executable's entry point. It is the place where all the pieces are put together
/// to create a fast an effective solver for the misp problem.
fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 5 {
        eprintln!("Uso: cargo run --example compare_misp <input_file> <output_file> <comp_type: Exact|Relaxed|Restricted> <max_width>");
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

    let file_name = input_file; // Use the provided input file name
    let file_path = format!("../resources/misp/{}", file_name);

    let problem = read_instance(&file_path).unwrap();
    let relaxation = MispRelax {pb: &problem};
    let ranking = MispRanking;
    
    // If you think you don't need a cache at all, just replace 'SimpleCache' by 'EmptyCache' in this next line.
    //let mut cache = SimpleCache::default();
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

    let mut dd = Mdd::<BitSet, {FRONTIER}>::new();
    
    let start = Instant::now();
    let compilation_result = dd.compile(&input);
    let duration = start.elapsed();

    match compilation_result {
        Ok(completion) => {
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
                "IndependentSet",              // Problem type
                problem.nb_variables(),  // Variables number
                dd_type_str,             // DD Type
                input.max_width,         // Max Width
                "false",                 // Is Reduced
                "max",                   // Objective Type
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
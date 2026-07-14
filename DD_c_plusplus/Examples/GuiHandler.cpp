#include "GuiHandler.h"

string to_string(ProblemClass problem_class) {
    switch (problem_class) {
        case ProblemClass::KnapsackState:  return "KnapsackState";
        case ProblemClass::SetCover:       return "Set Cover";
        case ProblemClass::Knapsack:       return "Knapsack";
        case ProblemClass::IndependentSet: return "Independent Set";
        case ProblemClass::SOCKnapsack:    return "SOC Knapsack";
        case ProblemClass::Scheduler:      return "Scheduler";
        default:                           return "None";
    }
}

string to_string(CutType cut_type) {
    switch (cut_type) {
        case CutType::Flow:      return "FlowCuts";
        case CutType::JointFlow: return "JointFlowCuts";
        case CutType::Target:    return "TargetCuts";
        default:                 return "Unknown";
    }
}

string to_string(DDType dd_type) {
    switch (dd_type) {
        case DDType::Exact:
            return "Exact";
        case DDType::Restricted:
            return "Restricted";
        case DDType::RelaxPriority:
            return "RelaxPriority";
        case DDType::RelaxGrouping:
            return "RelaxGrouping";
        default: return "None";
    }
}

bool is_integer(double val) {
    return abs(floor(val) - val) < tolerance || abs(ceil(val) - val) < tolerance;
}

bool are_all_variables_integer(const GRBModel* model) {
    for (int i = 0; i < model->get(GRB_IntAttr_NumVars); ++i) {
        double value = model->getVar(i).get(GRB_DoubleAttr_X);
        if (!is_integer(value)) {
            return false;
        }
    }
    return true;
}

void write_statistics(const string& file_path, const string& statistics, const string& headers) {
    try {
        if (filesystem::exists(file_path)) {
            ifstream file(file_path);
            string first_line;

            getline(file, first_line);
            if (first_line != headers) {
                cerr << "Error: this file is already in use by another program, please erase the file "
                          << file_path << " or provide another output file." << endl;
                exit(1);
            }
        }

        bool write_header = !filesystem::exists(file_path) || filesystem::file_size(file_path) == 0;

        ofstream file(file_path, ios::app);
        if (file.is_open()) {
            if (write_header) {
                file << headers;
                file << "\n";
            }

            file << statistics;
        } else {
            cerr << "Error: could not open file " << file_path << endl;
            exit(1);
        }

    } catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        exit(1);
    }
}

template <typename T, typename Solver>
void create_and_solve_dd(AbstractProblem<T>* problem_instance, const vector<double>& objective_weights, GuiStructure gui_structure) {
    vector<double> sorted_obj(problem_instance->dd_to_original_index.size());
    for (int i = 0; i < (int)problem_instance->dd_to_original_index.size(); ++i)
        sorted_obj[i] = objective_weights[problem_instance->dd_to_original_index[i]];

    DD<T> dd_instance(*problem_instance);
    string headers = "File_name,Problem type,Variables number,DD Type,Max Width,Is Reduced,Objective Type,Optimization Value,Construction Time,Reduce Time,Solution Time,Number Node,Number Arcs,Actual Max Width,Sort";

    switch (gui_structure.dd_type) {
        case DDType::Exact:
            dd_instance.create_decision_diagram(gui_structure.verbose); break;
        case DDType::Restricted:
            dd_instance.create_restricted_decision_diagram(gui_structure.maximum_width, gui_structure.verbose); break;
        case DDType::RelaxPriority:
            dd_instance.create_relax_priority_decision_diagram(gui_structure.maximum_width, gui_structure.verbose); break;
        case DDType::RelaxGrouping:
            dd_instance.create_relax_grouping_decision_diagram(gui_structure.maximum_width, gui_structure.verbose); break;
        default:
            cerr << "Error: No DD type specified" << endl; exit(1);
    }

    string reduction_time = "na";
    if (gui_structure.reduce_flag) {
        dd_instance.reduce_decision_diagram(gui_structure.verbose);
        reduction_time = dd_instance.get_reduction_time();
    }

    if (gui_structure.export_flag)
        dd_instance.export_graph_file("graph");

    Solver solver(dd_instance);
    solver.set_parameters(sorted_obj, gui_structure.minmax);
    solver.solve();

    string statistics =
            fs::path(gui_structure.input_file_path).filename().string() + "," +
            to_string(gui_structure.problem_class) + "," +
            to_string(problem_instance->ordered_variables.size()) + "," +
            to_string(gui_structure.dd_type) + "," +
            to_string(gui_structure.maximum_width) + "," +
            (gui_structure.reduce_flag ? "true" : "false") + "," +
            gui_structure.minmax + "," +
            to_string(solver.get_solution().value) + "," +
            dd_instance.get_building_time() + "," +
            reduction_time + "," +
            solver.get_time() + "," +
            to_string(dd_instance.get_decision_diagram()->get_node_count()) + "," +
            to_string(dd_instance.get_decision_diagram()->get_arc_count()) + "," +
            to_string(dd_instance.get_decision_diagram()->get_actual_max_width()) + "," +
            (gui_structure.sort_flag ? "true" : "false") + "\n";

    if (!gui_structure.output_file_path.empty())
        write_statistics(gui_structure.output_file_path, statistics, headers);
}

void run_dd_code(GuiStructure gui_structure) {

    switch (gui_structure.problem_class) {
        case ProblemClass::SetCover: {
            SetCoverInstance* set_cover_parameters = new SetCoverInstance(gui_structure.input_file_path);
            AbstractProblem<dynamic_bitset<>>* sc_problem = new SetCoverProblem(*set_cover_parameters, gui_structure.sort_flag);
            create_and_solve_dd<dynamic_bitset<>>(sc_problem, set_cover_parameters->objective_weights, gui_structure);
            delete sc_problem;
            delete set_cover_parameters;
            break;
        }
        case ProblemClass::IndependentSet: {
            IndependentSetInstance* independent_set_parameters = new IndependentSetInstance(gui_structure.input_file_path);
            AbstractProblem<dynamic_bitset<>>* is_problem = new IndependentSetProblem(*independent_set_parameters, gui_structure.sort_flag);
            create_and_solve_dd<dynamic_bitset<>>(is_problem, independent_set_parameters->objective_weights, gui_structure);
            delete is_problem;
            delete independent_set_parameters;
            break;
        }
        case ProblemClass::Knapsack: {
            KnapsackInstance* knapsack_parameters = new KnapsackInstance(gui_structure.input_file_path);
            AbstractProblem<int>* problem_instance = new KnapsackProblem(*knapsack_parameters, gui_structure.sort_flag);
            create_and_solve_dd<int>(problem_instance, knapsack_parameters->objective_weights, gui_structure);
            delete problem_instance;
            delete knapsack_parameters;
            break;
        }
        case ProblemClass::KnapsackState: {
            KnapsackInstance* knapsack_parameters = new KnapsackInstance(gui_structure.input_file_path);
            State* ks_state = new State(vector<int>(2, 0));
            AbstractProblem<State>* problem_instance = new KnapsackProblemState(ks_state, *knapsack_parameters, gui_structure.sort_flag);
            create_and_solve_dd<State>(problem_instance, knapsack_parameters->objective_weights, gui_structure);
            delete problem_instance;
            delete ks_state;
            delete knapsack_parameters;
            break;
        }
        case ProblemClass::SOCKnapsack: {
            SOCKnapsackInstance* sock_params = new SOCKnapsackInstance(gui_structure.input_file_path);
            vector<double>* init_state = new vector<double>(*sock_params->initial_state);
            AbstractProblem<vector<double>>* sock_problem = new SOCKnapsackProblem(init_state, *sock_params, 0, gui_structure.sort_flag);
            create_and_solve_dd<vector<double>>(sock_problem, sock_params->objective_weights, gui_structure);
            delete sock_problem;
            delete init_state;
            delete sock_params;
            break;
        }
        case ProblemClass::Scheduler: {
            SchedulerInstance* params = new SchedulerInstance(gui_structure.input_file_path);
            SchedulerProblem* problem = new SchedulerProblem(*params);
            create_and_solve_dd<SchedulerState, SchedulerPathSolver>(problem, params->objective_weights, gui_structure);
            delete problem;
            delete params;
            break;
        }
        case ProblemClass::None: {
            cerr << "Error: No problem class specified" << endl;
            exit(1);
        }
    }

}

// Build the decision diagram for cut generation according to the requested DD type.
// Restricted/relaxed are width-limited; exact ignores the width. The DD is reduced afterwards.
template<typename T>
void build_dd_for_cuts(DD<T>& dd_instance, const GuiCutsStructure& gui_cuts_structure) {
    switch (gui_cuts_structure.dd_type) {
        case DDType::Restricted:
            dd_instance.create_restricted_decision_diagram(gui_cuts_structure.maximum_width, false); break;
        case DDType::RelaxPriority:
            dd_instance.create_relax_priority_decision_diagram(gui_cuts_structure.maximum_width, false); break;
        case DDType::RelaxGrouping:
            dd_instance.create_relax_grouping_decision_diagram(gui_cuts_structure.maximum_width, false); break;
        default:  // DDType::Exact
            dd_instance.create_decision_diagram(false); break;
    }
    dd_instance.reduce_decision_diagram(false);
}

template<typename GurobiClass, typename T>
void gurobi_dd_cutting_planes(GuiCutsStructure gui_cuts_structure, GurobiClass& gurobi_instance, AbstractProblem<T>* problem_instance) {
    string headers = "File_name,Problem type,Variables number,DD Type,Max Width,Cut Type,CutStrengthening,Best solution,Root Gap,Gap,Solution Time,DD Time,Number Nodes,Number Arcs,NodeBB,Num Cuts";

    DD<T> dd_instance(*problem_instance);
    build_dd_for_cuts(dd_instance, gui_cuts_structure);

    CutType cut_type    = gui_cuts_structure.cut_type;
    bool cut_strengthening  = gui_cuts_structure.cut_strengthening_flag;

    gurobi_instance.create_model();

    vector<DD<T>*> dds = {&dd_instance};
    gurobi_instance.optimize_with_cuts(dds, cut_type, cut_strengthening);

    string root_gap  = to_string(gurobi_instance.get_root_gap());
    string final_gap = to_string(gurobi_instance.get_final_gap());

    string statistics =
            fs::path(gui_cuts_structure.input_file_path).filename().string() + "," +
            to_string(gui_cuts_structure.problem_class) + "," +
            to_string(problem_instance->ordered_variables.size()) + "," +
            to_string(gui_cuts_structure.dd_type) + "," +
            to_string(gui_cuts_structure.maximum_width) + "," +
            to_string(cut_type) + "," +
            (cut_strengthening ? "true" : "false") + "," +
            to_string(gurobi_instance.model->get(GRB_DoubleAttr_ObjVal)) + "," +
            root_gap + "," +
            final_gap + "," +
            gurobi_instance.get_optimization_time() + "," +
            to_string(stof(dd_instance.get_building_time()) + stof(dd_instance.get_reduction_time())) + "," +
            to_string(dd_instance.get_decision_diagram()->get_node_count()) + "," +
            to_string(dd_instance.get_decision_diagram()->get_arc_count()) + "," +
            to_string((long long)gurobi_instance.model->get(GRB_DoubleAttr_NodeCount)) + "," +
            to_string(gurobi_instance.num_cuts_added) + "\n";

    if (!gui_cuts_structure.output_file_path.empty()) {
        write_statistics(gui_cuts_structure.output_file_path, statistics, headers);
    }
}

void gurobi_dd_cutting_planes_soc(const GuiCutsStructure& gui_cuts_structure, SOCKnapsackGurobiClass& gurobi_instance, SOCKnapsackInstance* sock_params) {
    string headers = "File_name,Problem type,Variables number,DD Type,Max Width,Num SOC constraints,Cut Type,CutStrengthening,Best solution,Root Gap,Gap,Solution Time,DD Time,Total Number Nodes,Total Number Arcs,NodeBB,Num Cuts";

    int num_constraints = sock_params->right_side_of_restrictions.size();
    vector<SOCKnapsackProblem*> problems(num_constraints);
    vector<vector<double>*> init_states(num_constraints);
    vector<DD<vector<double>>*> dds(num_constraints);

    for (int i = 0; i < num_constraints; i++) {
        init_states[i] = new vector<double>(*sock_params->initial_state);
        problems[i] = new SOCKnapsackProblem(init_states[i], *sock_params, i, gui_cuts_structure.sort_flag);
        dds[i] = new DD<vector<double>>(*problems[i]);
        build_dd_for_cuts(*dds[i], gui_cuts_structure);
    }

    gurobi_instance.create_model();
    gurobi_instance.optimize_with_cuts(dds, gui_cuts_structure.cut_type, gui_cuts_structure.cut_strengthening_flag);

    double total_dd_time = 0.0;
    int total_nodes = 0, total_arcs = 0;
    for (int i = 0; i < num_constraints; i++) {
        total_dd_time += stof(dds[i]->get_building_time()) + stof(dds[i]->get_reduction_time());
        total_nodes   += dds[i]->get_decision_diagram()->get_node_count();
        total_arcs    += dds[i]->get_decision_diagram()->get_arc_count();
    }

    string root_gap  = to_string(gurobi_instance.get_root_gap());
    string final_gap = to_string(gurobi_instance.get_final_gap());

    string statistics =
            fs::path(gui_cuts_structure.input_file_path).filename().string() + "," +
            to_string(gui_cuts_structure.problem_class) + "," +
            to_string(sock_params->variables.size()) + "," +
            to_string(gui_cuts_structure.dd_type) + "," +
            to_string(gui_cuts_structure.maximum_width) + "," +
            to_string(num_constraints) + "," +
            to_string(gui_cuts_structure.cut_type) + "," +
            (gui_cuts_structure.cut_strengthening_flag ? "true" : "false") + "," +
            to_string(gurobi_instance.model->get(GRB_DoubleAttr_ObjVal)) + "," +
            root_gap + "," +
            final_gap + "," +
            gurobi_instance.get_optimization_time() + "," +
            to_string(total_dd_time) + "," +
            to_string(total_nodes) + "," +
            to_string(total_arcs) + "," +
            to_string((long long)gurobi_instance.model->get(GRB_DoubleAttr_NodeCount)) + "," +
            to_string(gurobi_instance.num_cuts_added) + "\n";

    for (int i = 0; i < num_constraints; i++) {
        delete dds[i];
        delete problems[i];
        delete init_states[i];
    }

    if (!gui_cuts_structure.output_file_path.empty()) {
        write_statistics(gui_cuts_structure.output_file_path, statistics, headers);
    }
}

void run_gurobi_dd(const GuiCutsStructure& gui_cuts_structure) {

    switch (gui_cuts_structure.problem_class) {
        case ProblemClass::SetCover: {

            SetCoverInstance* set_cover_parameters = new SetCoverInstance(gui_cuts_structure.input_file_path);
            SetCoverGurobiClass gurobi_instance(*set_cover_parameters, gui_cuts_structure.continuous_flag);
            SetCoverProblem problem_instance(*set_cover_parameters, gui_cuts_structure.sort_flag);

            gurobi_dd_cutting_planes<SetCoverGurobiClass, dynamic_bitset<>>(gui_cuts_structure, gurobi_instance, &problem_instance);
            delete set_cover_parameters;
            break;
        }

        case ProblemClass::IndependentSet: {
            IndependentSetInstance* independent_set_parameters = new IndependentSetInstance(gui_cuts_structure.input_file_path);
            IndependentSetGurobiClass gurobi_instance(*independent_set_parameters, gui_cuts_structure.continuous_flag);
            IndependentSetProblem problem_instance(*independent_set_parameters, gui_cuts_structure.sort_flag);

            gurobi_dd_cutting_planes<IndependentSetGurobiClass, dynamic_bitset<>>(gui_cuts_structure, gurobi_instance, &problem_instance);
            delete independent_set_parameters;
            break;
        }
        case ProblemClass::Knapsack: {

            KnapsackInstance* knapsack_parameters = new KnapsackInstance(gui_cuts_structure.input_file_path);
            KnapsackGurobiClass gurobi_instance(*knapsack_parameters, gui_cuts_structure.continuous_flag);
            KnapsackProblem problem_instance(*knapsack_parameters, gui_cuts_structure.sort_flag);

            gurobi_dd_cutting_planes<KnapsackGurobiClass, int>(gui_cuts_structure, gurobi_instance, &problem_instance);

            delete knapsack_parameters;
            break;
        }
        case ProblemClass::KnapsackState: {
            cerr << "Error: No implementation yet for this problem class" << endl;
            exit(1);
        }
        case ProblemClass::SOCKnapsack: {
            SOCKnapsackInstance* sock_params = new SOCKnapsackInstance(gui_cuts_structure.input_file_path);
            SOCKnapsackGurobiClass gurobi_instance(*sock_params, gui_cuts_structure.continuous_flag);
            gurobi_dd_cutting_planes_soc(gui_cuts_structure, gurobi_instance, sock_params);
            delete sock_params;
            break;
        }
        case ProblemClass::Scheduler: {
            SchedulerInstance* params = new SchedulerInstance(gui_cuts_structure.input_file_path);
            SchedulerGurobiClass gurobi_instance(*params, gui_cuts_structure.continuous_flag);
            SchedulerProblem problem_instance(*params);
            gurobi_dd_cutting_planes<SchedulerGurobiClass, SchedulerState>(
                gui_cuts_structure, gurobi_instance, &problem_instance);
            delete params;
            break;
        }
        case ProblemClass::None: {
            cerr << "Error: No problem class specified" << endl;
            exit(1);
        }
    }

}

void gurobi_ilp(GuiGurobiStructure gui_gurobi_structure) {
    AbstractProblemGurobi* gurobi_instance = nullptr;
    string headers = "File_name,Problem type,Variables number,Best solution,Dual bound,Root Gap,Gap,Solution Time,NodeBB";

    switch (gui_gurobi_structure.problem_class) {
        case ProblemClass::SetCover: {
            SetCoverInstance* set_cover_parameters = new SetCoverInstance(gui_gurobi_structure.input_file_path);
            gurobi_instance = new SetCoverGurobiClass(*set_cover_parameters, gui_gurobi_structure.continuous);
            delete set_cover_parameters;
            break;
        }

        case ProblemClass::IndependentSet: {
            IndependentSetInstance* independent_set_parameters = new IndependentSetInstance(gui_gurobi_structure.input_file_path);
            gurobi_instance = new IndependentSetGurobiClass(*independent_set_parameters, gui_gurobi_structure.continuous);
            delete independent_set_parameters;
            break;
        }
        case ProblemClass::Knapsack: {
            KnapsackInstance* knapsack_parameters = new KnapsackInstance(gui_gurobi_structure.input_file_path);
            gurobi_instance = new KnapsackGurobiClass(*knapsack_parameters, gui_gurobi_structure.continuous);
            delete knapsack_parameters;
            break;
        }
        case ProblemClass::KnapsackState: {
            cerr << "Error: No implementation yet for this problem class" << endl;
            exit(1);
        }
        case ProblemClass::SOCKnapsack: {
            SOCKnapsackInstance* sock_params = new SOCKnapsackInstance(gui_gurobi_structure.input_file_path);
            gurobi_instance = new SOCKnapsackGurobiClass(*sock_params, gui_gurobi_structure.continuous);
            delete sock_params;
            break;
        }
        case ProblemClass::Scheduler: {
            SchedulerInstance* params = new SchedulerInstance(gui_gurobi_structure.input_file_path);
            gurobi_instance = new SchedulerGurobiClass(*params, gui_gurobi_structure.continuous);
            delete params;
            break;
        }
        case ProblemClass::None: {
            cerr << "Error: No problem class specified" << endl;
            exit(1);
        }
    }

    gurobi_instance->create_model();
    gurobi_instance->optimize_model();

    string root_gap  = to_string(gurobi_instance->get_root_gap());
    string final_gap = to_string(gurobi_instance->get_final_gap());

    // At the time limit Gurobi may stop with no feasible incumbent; in that case
    // ObjVal throws. Write the row anyway (best solution = "na") so unfinished
    // instances still produce a record, just like the cut-based runs.
    string best_solution = (gurobi_instance->model->get(GRB_IntAttr_SolCount) > 0)
            ? to_string(gurobi_instance->model->get(GRB_DoubleAttr_ObjVal))
            : "na";

    string statistics =
            fs::path(gui_gurobi_structure.input_file_path).filename().string() + "," +
            to_string(gui_gurobi_structure.problem_class) + "," +
            to_string(gurobi_instance->variable_length) + "," +
            best_solution + "," +
            to_string(gurobi_instance->model->get(GRB_DoubleAttr_ObjBound)) + "," +
            root_gap + "," +
            final_gap + "," +
            gurobi_instance->get_optimization_time() + "," +
            to_string((long long)gurobi_instance->model->get(GRB_DoubleAttr_NodeCount)) + "\n";

    if (!gui_gurobi_structure.output_file_path.empty()) {
        write_statistics(gui_gurobi_structure.output_file_path, statistics, headers);
    }
    delete gurobi_instance;
}
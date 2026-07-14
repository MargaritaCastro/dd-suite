#include "GuiStructure.h"

GuiStructure::GuiStructure(const string& input_file_path, const string& output_file_path,
             const DDType& dd_type, const bool& verbose, bool export_flag,
             bool reduce_flag, int maximum_width, const ProblemClass& problem_class,
             const string& minmax, bool sort_flag)
             : input_file_path(input_file_path), output_file_path(output_file_path), dd_type(dd_type),
             verbose(verbose), export_flag(export_flag), reduce_flag(reduce_flag), maximum_width(maximum_width),
             problem_class(problem_class), minmax(minmax), sort_flag(sort_flag)
             {}

GuiCutsStructure::GuiCutsStructure(const string& input_file_path, const string& output_file_path,
                                   const ProblemClass& problem_class, CutType cut_type,
                                   const bool& cut_strengthening_flag,
                                   const bool& verbose, const bool& continuous_flag, bool sort_flag,
                                   DDType dd_type, int maximum_width)
             : input_file_path(input_file_path), output_file_path(output_file_path), problem_class(problem_class),
             cut_type(cut_type), continuous_flag(continuous_flag),
             verbose(verbose), cut_strengthening_flag(cut_strengthening_flag), sort_flag(sort_flag),
             dd_type(dd_type), maximum_width(maximum_width)
             {}

GuiGurobiStructure::GuiGurobiStructure(const string& input_file_path, const string& output_file_path,
                                       const ProblemClass& problem_class, const bool& verbose, const bool& continuous)
        : input_file_path(input_file_path), output_file_path(output_file_path), problem_class(problem_class),
        verbose(verbose), continuous(continuous)
        {}
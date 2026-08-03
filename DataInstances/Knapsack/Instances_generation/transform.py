import os
from tqdm import tqdm

files_path = [
    f"low-dimensional/{file}" for file in os.listdir('low-dimensional/')
] + [
    f"large_scale/{file}" for file in os.listdir('large_scale/')
]


for file_path in tqdm(files_path):
    print(file_path)
    matrix_of_weights = []
    objective_weights = []
    with open(file_path, 'r') as f:
        lines = [line for line in f.readlines() if line.strip()]
        variables_length = int(lines[0].split()[0])
        right_side_of_restrictions = int(lines[0].split()[1])
        # Read EXACTLY variables_length items after the header. Using lines[1:-1] does
        # not work: the large_scale files end with an extra line (the 0/1 vector of the
        # optimal solution) that must be discarded, but the low-dimensional files do not
        # have it and there the last line is an item. Always dropping the last line lost
        # the last item of the 9 low-dimensional instances (e.g. f3_l-d_kp_4_20 ended up
        # with 3 items instead of 4).
        item_lines = lines[1:variables_length + 1]
        assert len(item_lines) == variables_length, (
            f"{file_path}: expected {variables_length} items but found {len(item_lines)}")
        for line in item_lines:
            objective, weigth = line.split()
            matrix_of_weights.append(weigth)
            objective_weights.append(objective)

    # f5_l-d_kp_15_375 is the only instance in the benchmark with fractional
    # coefficients (its reference optimum is 481.0694). The DD-suite parser
    # (KnapsackInstance.read_file) reads integers with int(), so it is skipped instead of
    # writing a file that crashes on load. This is why the standard set has 9
    # low-dimensional instances rather than 10.
    if any(not tok.lstrip('-').isdigit()
           for tok in matrix_of_weights + objective_weights):
        print(f"  skipped (non-integer coefficients): {file_path}")
        continue

    custom_folder = 'Standard/'
    # Source file names may or may not carry a '.txt' extension, so strip it before
    # appending one to avoid producing a duplicated '...txt.txt'.
    file_name = os.path.splitext(os.path.basename(file_path))[0] + '.txt'

    with open(custom_folder + file_name, 'w') as custom_file:
        custom_file.write(str(variables_length) + '\n')
        custom_file.write(str(right_side_of_restrictions) + '\n') 
        custom_file.write(' '.join(map(str, matrix_of_weights)) + '\n')
        custom_file.write(' '.join(map(str, objective_weights)) + '\n')
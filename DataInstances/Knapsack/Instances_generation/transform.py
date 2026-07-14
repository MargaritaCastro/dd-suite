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
        lines = f.readlines()
        variables_length = int(lines[0].split()[0])
        right_side_of_restrictions = int(lines[0].split()[1])
        for line in lines[1:len(lines)-1]:
            objective, weigth = line.split()
            matrix_of_weights.append(weigth)
            objective_weights.append(objective)

    custom_folder = 'Standard/'
    file_name = file_path.split('/')[1] + '.txt'

    with open(custom_folder + file_name, 'w') as custom_file:
        custom_file.write(str(variables_length) + '\n')
        custom_file.write(str(right_side_of_restrictions) + '\n') 
        custom_file.write(' '.join(map(str, matrix_of_weights)) + '\n')
        custom_file.write(' '.join(map(str, objective_weights)) + '\n')
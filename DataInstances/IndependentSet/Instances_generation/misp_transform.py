import os
from tqdm import tqdm

files_path = [f"misp/{file}" for file in os.listdir('misp/')] 

for file_path in tqdm(files_path):
    dict_node_neighbors = {}
    with open(file_path, 'r') as f:
        p, edge, variables_length, arcs_length = f.readline().strip().split(' ')

        for i in range(int(variables_length)):
            dict_node_neighbors[f'x_{i+1}'] = []

        for i in range(int(arcs_length)):
            line = f.readline().strip()
            arc, node_1, node_2 = line.split(' ')
            if int(node_2) not in dict_node_neighbors[f'x_{node_1}']:
                dict_node_neighbors[f'x_{node_1}'].append(int(node_2))
            if int(node_1) not in dict_node_neighbors[f'x_{node_2}']:
                dict_node_neighbors[f'x_{node_2}'].append(int(node_1))

    custom_folder = 'Standard/'
    file_name = file_path.split('/')[1][:-4] + '.txt'

    objective_weights = [1 for _ in range(int(variables_length))]

    with open(custom_folder + file_name, 'w') as custom_file:
        custom_file.write(str(variables_length) + '\n')
        custom_file.write(' '.join(map(str, objective_weights)) + '\n')
        for key, value in dict_node_neighbors.items():
            custom_file.write(f'{key}: {value}')
            if key != list(dict_node_neighbors.keys())[-1]:
                custom_file.write('\n')
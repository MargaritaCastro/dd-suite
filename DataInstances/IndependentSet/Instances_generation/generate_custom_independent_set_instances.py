import random

### Independent Set
### NECESSARY: Change THE seed_value, variables_length, and max_variable_value
seed_value = 1
random.seed(seed_value)
variables_length = 250
max_variable_value = round(variables_length*3/4)

dict_node_neighbors = {}
for i in range(variables_length):
    dict_node_neighbors[f'x_{i+1}'] = []

for i in range(1, variables_length + 1):
    i_variable_name = f'x_{i}'
    for _ in range(random.randint(1, variables_length)):
        number = random.randint(1, variables_length)
        number_variable_name = f'x_{number}'
        if i != number:
            if number not in dict_node_neighbors[i_variable_name]:
                dict_node_neighbors[i_variable_name].append(number)
            if i not in dict_node_neighbors[number_variable_name]:
                dict_node_neighbors[number_variable_name].append(i)

objective_weights = [random.randint(1, max_variable_value) for _ in range(variables_length)]



folder = 'Custom/'
file_name = 'independent_set_instance_v' + str(variables_length) + '_d' + str(max_variable_value)+ '_seed'+str(seed_value)+'.txt'

with open(folder + file_name, 'w') as file:
    file.write(str(variables_length) + '\n')
    file.write(' '.join(map(str, objective_weights)) + '\n')
    for key, value in dict_node_neighbors.items():
        file.write(f'{key}: {value}')
        if key != list(dict_node_neighbors.keys())[-1]:
            file.write('\n')
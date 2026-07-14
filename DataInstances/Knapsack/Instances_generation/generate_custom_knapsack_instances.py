import random

### Knapsack
### NECESSARY: Change THE seed_value, variables_length, and max_variable_value
seed_value = 1
random.seed(seed_value)
variables_length = 1000
max_variable_value = 100

right_side = 0
matrix_of_weights = []
for j in range(variables_length):
    value = random.randint(1, max_variable_value)
    right_side += value
    matrix_of_weights.append(value)

right_side_of_restrictions = variables_length // 2

objective_weights = []
for j in range(variables_length):
    value = random.randint(1, max_variable_value)
    objective_weights.append(value)

folder = 'Custom/'
file_name = 'knapsack_instance_v' + str(variables_length) + '_d' + str(max_variable_value)+ '_seed'+str(seed_value)+'.txt'

with open(folder + file_name, 'w') as file:
    file.write(str(variables_length) + '\n')
    file.write(str(right_side_of_restrictions) + '\n') 
    file.write(' '.join(map(str, matrix_of_weights)) + '\n')
    file.write(' '.join(map(str, objective_weights)) + '\n')
import random

### Set Cover
### NECESSARY: Change THE seed_value, variables_length, and restrictions_length
seed_value = 1
random.seed(seed_value)
variables_length = 70
max_variable_value = 10
restrictions_length = 50

matrix_of_weight = [[random.randint(0, 1) for _ in range(variables_length)] for _ in range(restrictions_length)]


objective_weights = [random.randint(1, max_variable_value) for _ in range(variables_length)]

folder = 'Custom/'
file_name = 'set_cover_v' + str(variables_length) + '_r' + str(restrictions_length)+ '_seed'+str(seed_value)+'.txt'

with open(folder + file_name, 'w') as file:
    file.write(str(variables_length) + '\n')
    file.write(str(restrictions_length) + '\n')
    file.write(' '.join(map(str, objective_weights)) + '\n')
    for i, row in enumerate(matrix_of_weight):
        file.write(' '.join(map(str, row)))
        if i != len(matrix_of_weight) - 1:  
            file.write('\n')
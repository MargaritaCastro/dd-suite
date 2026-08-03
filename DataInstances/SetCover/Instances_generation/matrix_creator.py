######################################################################################################################################################
### Based on the paper: Manipulating MDD Relaxations for Combinatorial Optimization by David Bergman, Willem-Jan van Hoeve, and John N. Hooker     ###
######################################################################################################################################################

import numpy as np

def set_cover_matrix_generator(n: int, m: int, d: float, bw: int, seed: int=1) -> np.ndarray:
    """
    Generate an m x n matrix A for a structured Set Cover instance.

    Parameters:
    - n: number of columns (variables).
    - m: number of rows (constraints).
    - d: density of ones per row (a proportion between 0 and 1).
    - bw: bandwidth (how many columns ahead can be chosen).

    Returns:
    - A: binary matrix of size m x n.
    """
    A = np.zeros((m, n), dtype=int)
    k = int(d * n)  # number of ones per row
    np.random.seed(seed)

    for i in range(m):
        start = i
        end = min(i + bw, n)  # avoid exceeding n
        candidates = list(range(start, end))

        if len(candidates) == 0:
            continue  # there are no valid columns for this row

        selection = candidates if len(candidates) <= k else np.random.choice(candidates, size=k, replace=False)
        A[i, selection] = 1

    return A

n_options = [250, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000, 3250, 3500, 3750, 4000]
d_options = [0.1]
b_w_options = [165]

for seed in range(1, 5):
    for n in n_options:
        for d in d_options:
            d = 75 / n  # fixes the number of ones per row at k = int(d * n) = 75
            for b_w in b_w_options:
                m = n - b_w + 1
                custom_folder = './Standard/'
                file_name = f'set_cover_n{n}_m{m}_d{round(d,2)}_b_w{b_w}_seed{seed}.txt'

                objective_weights = [1 for _ in range(int(n))]
                matrix_of_weight = set_cover_matrix_generator(n, m, d, b_w, seed)

                print(f"n: {n}, m: {m}, d: {d}, b_w: {b_w}")
                print(matrix_of_weight)

                with open(custom_folder + file_name, 'w') as file:
                    file.write(str(n) + '\n')
                    file.write(str(m) + '\n')
                    file.write(' '.join(map(str, objective_weights)) + '\n')
                    for i, row in enumerate(matrix_of_weight):
                        file.write(' '.join(map(str, row)))
                        if i != len(matrix_of_weight) - 1:
                            file.write('\n')

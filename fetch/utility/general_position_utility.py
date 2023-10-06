import numpy as np


def is_valid_transform(matrix):
    if matrix.shape != (4, 4):
        return False
    if not np.all(matrix[3, :] == [0, 0, 0, 1]):
        return False

    return True

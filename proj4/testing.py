import numc as nc
import numpy as np
import hashlib, struct
from typing import Union, List
import operator
import time
from unittest import TestCase

num_samples = 1000
decimal_places = 6
func_mapping = {
    "add": operator.add,
    "sub": operator.sub,
    "mul": operator.mul,
    "neg": operator.neg,
    "abs": operator.abs,
    "pow": operator.pow
}

def dp_nc_matrix(*args, **kwargs):
    if len(kwargs) > 0:
        return nc.Matrix(*args, **kwargs)
    else:
        return nc.Matrix(*args)

def compute(nc_mat_lst: List[Union[nc.Matrix, int]], op: str):
    f = func_mapping[op]
    nc_start, nc_end, dp_start, dp_end = None, None, None, None
    nc_result, dp_result = None, None
    assert(op in list(func_mapping.keys()))
    if op == "neg" or op == "abs":
        assert(len(nc_mat_lst) == 1)
        nc_start = time.time()
        nc_result = f(nc_mat_lst[0])
        nc_end = time.time()

    else:
        assert(len(nc_mat_lst) > 1)
        nc_start = time.time()
        nc_result = nc_mat_lst[0]
        for mat in nc_mat_lst[1:]:
            nc_result = f(nc_result, mat)
        nc_end = time.time()

    return True, nc_end - nc_start

def rand_dp_nc_matrix(rows, cols, seed=0):
    return nc.Matrix(rows, cols, rand=True, seed=seed)

def print_speedup(speed_up):
    print("Speed up is:", speed_up)

class TestAdd(TestCase):
    def test_small_add(self):
        # TODO: YOUR CODE HERE
        nc_mat1 = rand_dp_nc_matrix(2, 2, seed=0)
        nc_mat2 = rand_dp_nc_matrix(2, 2, seed=1)
        is_correct, speed_up = compute([nc_mat1, nc_mat2], "add")
        print_speedup(speed_up)

    def test_medium_add(self):
        # TODO: YOUR CODE HERE
        nc_mat1 = rand_dp_nc_matrix(500, 500, seed=0)
        nc_mat2 = rand_dp_nc_matrix(500, 500, seed=1)
        is_correct, speed_up = compute([nc_mat1, nc_mat2], "add")
        print_speedup(speed_up)
    def test_large_add(self):
        # TODO: YOUR CODE
        nc_mat1 = rand_dp_nc_matrix(5000, 5000, seed=0)
        nc_mat2 = rand_dp_nc_matrix(5000, 5000, seed=1)
        is_correct, speed_up = compute([nc_mat1, nc_mat2], "add")
        print_speedup(speed_up)

"""
CS61C Fall 2020 Project 4: numc imported!
Speed up is: 1.9073486328125e-06
Speed up is: 0.0018420219421386719
Speed up is: 0.10489416122436523
"""

test1 = TestAdd()
test1.test_small_add()
test1.test_medium_add()
test1.test_large_add()
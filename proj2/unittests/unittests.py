from unittest import TestCase
from framework import AssemblyTest, print_coverage


class TestAbs(TestCase):
    def test_zero(self):
        t = AssemblyTest(self, "abs.s")
        # load 0 into register a0
        t.input_scalar("a0", 0)
        # call the abs function
        t.call("abs")
        # check that after calling abs, a0 is equal to 0 (abs(0) = 0)
        t.check_scalar("a0", 0)
        # generate the `assembly/TestAbs_test_zero.s` file and run it through venus
        t.execute()

    def test_one(self):
        # same as test_zero, but with input 1
        t = AssemblyTest(self, "abs.s")
        t.input_scalar("a0", 1)
        t.call("abs")
        t.check_scalar("a0", 1)
        t.execute()

    def test_minus_one(self):
        t = AssemblyTest(self, "abs.s")
        t.input_scalar("a0", -1)
        t.call("abs")
        t.check_scalar("a0", 1)
        t.execute()

    @classmethod
    def tearDownClass(cls):
        print_coverage("abs.s", verbose=False)


class TestRelu(TestCase):
    def test_simple(self):
        t = AssemblyTest(self, "relu.s")
        # create an array in the data section
        array0 = t.array([1, -2, 3, -4, 5, -6, 7, -8, 9])
        # load address of `array0` into register a0
        t.input_array("a0", array0)
        # set a1 to the length of our array
        t.input_scalar("a1", len(array0))
        # call the relu function
        t.call("relu")
        # check that the array0 was changed appropriately
        t.check_array(array0, [1, 0, 3, 0, 5, 0, 7, 0, 9])
        # generate the `assembly/TestRelu_test_simple.s` file and run it through venus

    def test_all_negative(self):
        t = AssemblyTest(self, "relu.s")
        # create an array in the data section
        array0 = t.array([-1, -2, -3])
        # load address of `array0` into register a0
        t.input_array("a0", array0)
        # set a1 to the length of our array
        t.input_scalar("a1", len(array0))
        # call the relu function
        t.call("relu")
        # check that the array0 was changed appropriately
        t.check_array(array0, [0, 0, 0])
        # generate the `assembly/TestRelu_test_simple.s` file and run it through venus
        t.execute()

    def test_all_pos(self):
        t = AssemblyTest(self, "relu.s")
        # create an array in the data section
        array0 = t.array([1, 2, 3])
        # load address of `array0` into register a0
        t.input_array("a0", array0)
        # set a1 to the length of our array
        t.input_scalar("a1", len(array0))
        # call the relu function
        t.call("relu")
        # check that the array0 was changed appropriately
        t.check_array(array0, [1, 2, 3])
        # generate the `assembly/TestRelu_test_simple.s` file and run it through venus
        t.execute()

    def test_large(self):
        t = AssemblyTest(self, "relu.s")
        # create an array in the data section
        array0 = t.array([2147483647, -2147483647])
        # load address of `array0` into register a0
        t.input_array("a0", array0)
        # set a1 to the length of our array
        t.input_scalar("a1", len(array0))
        # call the relu function
        t.call("relu")
        # check that the array0 was changed appropriately
        t.check_array(array0, [2147483647, 0])
        # generate the `assembly/TestRelu_test_simple.s` file and run it through venus
        t.execute()

    def test_single_neg(self):
        t = AssemblyTest(self, "relu.s")
        # create an array in the data section
        array0 = t.array([-1])
        # load address of `array0` into register a0
        t.input_array("a0", array0)
        # set a1 to the length of our array
        t.input_scalar("a1", len(array0))
        # call the relu function
        t.call("relu")
        # check that the array0 was changed appropriately
        t.check_array(array0, [0])
        # generate the `assembly/TestRelu_test_simple.s` file and run it through venus
        t.execute()

    def test_single_pos(self):
        t = AssemblyTest(self, "relu.s")
        # create an array in the data section
        array0 = t.array([1])
        # load address of `array0` into register a0
        t.input_array("a0", array0)
        # set a1 to the length of our array
        t.input_scalar("a1", len(array0))
        # call the relu function
        t.call("relu")
        # check that the array0 was changed appropriately
        t.check_array(array0, [1])
        # generate the `assembly/TestRelu_test_simple.s` file and run it through venus
        t.execute()
    
    def test_no_elem(self):
        t = AssemblyTest(self, "relu.s")
        # create an array in the data section
        array0 = t.array([])
        # load address of `array0` into register a0
        t.input_array("a0", array0)
        # set a1 to the length of our array
        t.input_scalar("a1", len(array0))
        # call the relu function
        t.call("relu")
        # generate the `assembly/TestRelu_test_simple.s` file and run it through venus
        t.execute(code=78)

    @classmethod
    def tearDownClass(cls):
        print_coverage("relu.s", verbose=False)


class TestArgmax(TestCase):
    def test_simple(self):
        t = AssemblyTest(self, "argmax.s")
        # create an array in the data section
        array0 = t.array([1, 100, 2])
        # load address of the array into register a0
        t.input_array("a0", array0)
        # set a1 to the length of the array
        t.input_scalar("a1", len(array0))
        # call the `argmax` function
        t.call("argmax")
        # check that the register a0 contains the correct output
        t.check_scalar("a0", 1)
        # generate the `assembly/TestArgmax_test_simple.s` file and run it through venus
        t.execute()

    def test_negative(self):
        t = AssemblyTest(self, "argmax.s")
        # create an array in the data section
        array0 = t.array([-4, -100, -1])
        # load address of the array into register a0
        t.input_array("a0", array0)
        # set a1 to the length of the array
        t.input_scalar("a1", len(array0))
        # call the `argmax` function
        t.call("argmax")
        # check that the register a0 contains the correct output
        t.check_scalar("a0", 2)
        # generate the `assembly/TestArgmax_test_simple.s` file and run it through venus
        t.execute()        


    def test_repeats(self):
        t = AssemblyTest(self, "argmax.s")
        # create an array in the data section
        array0 = t.array([4, 6, 5, 6])
        # load address of the array into register a0
        t.input_array("a0", array0)
        # set a1 to the length of the array
        t.input_scalar("a1", len(array0))
        # call the `argmax` function
        t.call("argmax")
        # check that the register a0 contains the correct output
        t.check_scalar("a0", 1)
        # generate the `assembly/TestArgmax_test_simple.s` file and run it through venus
        t.execute()   

    def test_single_elem(self):
        t = AssemblyTest(self, "argmax.s")
        # create an array in the data section
        array0 = t.array([4])
        # load address of the array into register a0
        t.input_array("a0", array0)
        # set a1 to the length of the array
        t.input_scalar("a1", len(array0))
        # call the `argmax` function
        t.call("argmax")
        # check that the register a0 contains the correct output
        t.check_scalar("a0", 0)
        # generate the `assembly/TestArgmax_test_simple.s` file and run it through venus
        t.execute()    

    def test_large_array(self):
        t = AssemblyTest(self, "argmax.s")
        # create an array in the data section
        array0 = t.array(list(range(1000)))
        # load address of the array into register a0
        t.input_array("a0", array0)
        # set a1 to the length of the array
        t.input_scalar("a1", len(array0))
        # call the `argmax` function
        t.call("argmax")
        # check that the register a0 contains the correct output
        t.check_scalar("a0", 999)
        # generate the `assembly/TestArgmax_test_simple.s` file and run it through venus
        t.execute()

    def test_zeroes(self):
        t = AssemblyTest(self, "argmax.s")
        # create an array in the data section
        array0 = t.array([0, -1, 0])
        # load address of the array into register a0
        t.input_array("a0", array0)
        # set a1 to the length of the array
        t.input_scalar("a1", len(array0))
        # call the `argmax` function
        t.call("argmax")
        # check that the register a0 contains the correct output
        t.check_scalar("a0", 0)
        # generate the `assembly/TestArgmax_test_simple.s` file and run it through venus
        t.execute()                      

    def test_error(self):
        t = AssemblyTest(self, "argmax.s")
        # create an array in the data section
        array0 = t.array([])
        # load address of the array into register a0
        t.input_array("a0", array0)
        # set a1 to the length of the array
        t.input_scalar("a1", len(array0))
        # call the `argmax` function
        t.call("argmax")
        # check that the register a0 contains the correct output
        # generate the `assembly/TestArgmax_test_simple.s` file and run it through venus
        t.execute(code=77)                            

    @classmethod
    def tearDownClass(cls):
        print_coverage("argmax.s", verbose=False)


class TestDot(TestCase):
    def test_simple(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        array1 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 9)
        t.input_scalar("a3", 1)
        t.input_scalar("a4", 1)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.check_scalar("a0", 285)
        t.execute()

    def test_stride_len(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        array1 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 3)
        t.input_scalar("a3", 1)
        t.input_scalar("a4", 2)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.check_scalar("a0", 22)
        t.execute()

    def test_diff_arrays(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([1, 2, 3])
        array1 = t.array([4, 5, 6])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 3)
        t.input_scalar("a3", 1)
        t.input_scalar("a4", 1)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.check_scalar("a0", 32)
        t.execute()   

    def test_diff_arrays_stride(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([1, 2, 3, 4, 5])
        array1 = t.array([10, 20, 30, 40, 50])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 3)
        t.input_scalar("a3", 2)
        t.input_scalar("a4", 2)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.check_scalar("a0", 350)
        t.execute()         

    def test_another_stride(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([1, 2, 3, 4, 5])
        array1 = t.array([5, 4, 3, 2, 1])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 2)
        t.input_scalar("a3", 1)
        t.input_scalar("a4", 3)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.check_scalar("a0", 9)
        t.execute()                 

    def test_single_elem(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([3])
        array1 = t.array([7])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 1)
        t.input_scalar("a3", 1)
        t.input_scalar("a4", 1)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.check_scalar("a0", 21)
        t.execute()                        

    def test_75(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        array1 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 0)
        t.input_scalar("a3", 1)
        t.input_scalar("a4", 2)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.execute(code=75)

    def test_76_1(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        array1 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 3)
        t.input_scalar("a3", 0)
        t.input_scalar("a4", 2)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.execute(code=76)        

    def test_76_2(self):
        t = AssemblyTest(self, "dot.s")
        # create arrays in the data section
        array0 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        array1 = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        # load array addresses into argument registers
        t.input_array("a0", array0)
        t.input_array("a1", array1)
        # load array attributes into argument registers
        t.input_scalar("a2", 3)
        t.input_scalar("a3", 1)
        t.input_scalar("a4", -45)
        # call the `dot` function
        t.call("dot")
        # check the return value
        t.execute(code=76)                        

    @classmethod
    def tearDownClass(cls):
        print_coverage("dot.s", verbose=False)


class TestMatmul(TestCase):

    def do_matmul(self, m0, m0_rows, m0_cols, m1, m1_rows, m1_cols, result, code=0):
        t = AssemblyTest(self, "matmul.s")
        # we need to include (aka import) the dot.s file since it is used by matmul.s
        t.include("dot.s")

        # create arrays for the arguments and to store the result
        array0 = t.array(m0)
        array1 = t.array(m1)
        array_out = t.array([0] * len(result))

        # load address of input matrices and set their dimensions
        t.input_array("a0", array0)
        t.input_scalar("a1", m0_rows)  
        t.input_scalar("a2", m0_cols)                
        t.input_array("a3", array1)
        t.input_scalar("a4", m1_rows)  
        t.input_scalar("a5", m1_cols)           


        # load address of output array
        t.input_array("a6", array_out)

        # call the matmul function
        t.call("matmul")

        # check the content of the output array
        t.check_array(array_out, result)

        # generate the assembly file and run it through venus, we expect the simulation to exit with code `code`
        t.execute(code=code)

    def test_simple(self):
        self.do_matmul(
            [1, 2, 3, 4, 5, 6, 7, 8, 9], 3, 3,
            [1, 2, 3, 4, 5, 6, 7, 8, 9], 3, 3,
            [30, 36, 42, 66, 81, 96, 102, 126, 150]
        )
    def test_simple_diff_matrices(self):
        self.do_matmul(
            [1, 2, 3, 4, 5, 6], 2, 3,
            [1, 2, 3, 4, 5, 6], 3, 2,
            [22, 28, 49, 64]
        )

    def test_rectangular_matrices(self):
        self.do_matmul(
            [1, 2, 3, 4], 2, 2,
            [2, 0, 1, 2], 2, 2,
            [4, 4, 10, 8]
        )

    def test_identity_matrix(self):
        self.do_matmul(
            [1, 2, 3, 4], 2, 2,
            [1, 0, 0, 1], 2, 2,
            [1, 2, 3, 4]
        )

    def test_zero(self):
        self.do_matmul(
            [1, 2, 3, 4], 2, 2,
            [0, 0, 0, 0], 2, 2,
            [0, 0, 0, 0]
        )

    def test_large_matrix_multiplication(self):
        matrix_a = [
            86, 27, 45, 71, 11, 97, 56, 24, 71, 46, 85, 77, 95,
            96, 24, 43, 83, 77, 60, 34, 26, 52, 79, 97, 53, 66,
            37, 46, 34, 69, 14, 29, 26, 70, 84, 57, 54, 66, 34,
            97, 11, 94, 58, 66, 88, 39, 30, 83, 13, 58, 67, 14,
            74, 27, 79, 55, 87, 64, 60, 18, 15, 31, 82, 12, 37,
            41, 93, 69, 91, 99, 99, 49, 91, 21, 78, 22, 14, 13,
            84, 78, 67, 98, 96, 99, 75, 44, 96, 69, 68, 42, 18,
            61, 79, 19, 77, 98, 39, 31, 47, 91, 75, 44, 45, 22,
            15, 87, 90, 11, 32, 15, 23, 17, 46, 35, 49, 64, 13,
            59, 30, 62, 16, 46, 55, 28, 13, 98, 78, 69, 92, 80
        ]

        matrix_b = [
            43, 68, 52, 86, 60, 90, 76, 53, 94, 81, 23, 23, 14, 21, 15,
            94, 66, 73, 88, 98, 70, 11, 33, 89, 30, 56, 72, 31, 78, 84,
            78, 26, 29, 81, 89, 80, 75, 52, 55, 95, 56, 94, 49, 56, 65,
            11, 85, 79, 21, 82, 85, 73, 31, 43, 13, 99, 48, 34, 56, 81,
            35, 12, 22, 50, 41, 25, 20, 19, 26, 13, 95, 71, 19, 82, 78,
            85, 45, 54, 97, 87, 45, 21, 87, 34, 68, 20, 66, 48, 46, 67,
            42, 39, 38, 68, 61, 66, 60, 25, 10, 47, 68, 56, 82, 84, 30,
            85, 67, 76, 29, 50, 82, 23, 47, 79, 92, 46, 20, 93, 74, 30,
            35, 36, 87, 22, 43, 21, 95, 63, 63, 62, 98, 71, 56, 20, 42,
            35, 12, 23, 13, 81, 79, 76, 19, 44, 36, 45, 72, 60, 52, 33,
            76, 47, 47, 21, 83, 26, 75, 29, 75, 64, 47, 39, 45, 30, 87,
            49, 49, 75, 21, 99, 96, 71, 33, 80, 94, 40, 21, 94, 62, 68,
            86, 53, 77, 56, 35, 64, 24, 94, 46, 98, 99, 66, 34, 58, 85
        ]

        expected_result = [
            46047, 39035, 47109, 40253, 55921, 50376, 45923, 40449, 45457, 52599, 46444, 41914, 39378, 38898, 47197,
            42314, 36451, 42707, 37347, 54739, 48983, 45848, 34987, 44833, 46457, 48064, 42445, 35809, 39970, 47244,
            34520, 30760, 38626, 25757, 44030, 40429, 36828, 27430, 38243, 38572, 38401, 32239, 34170, 32307, 35618,
            39408, 32874, 39470, 38904, 50820, 44200, 43291, 34170, 41384, 46302, 41426, 39343, 34555, 35405, 41008,
            36634, 28328, 31094, 36233, 44767, 38452, 34630, 27979, 33650, 37031, 38398, 37022, 27709, 34876, 39130,
            45502, 36207, 41119, 42694, 56276, 50432, 35928, 32774, 41581, 40651, 46376, 45737, 37650, 47355, 46265,
            50462, 42904, 51070, 48679, 66156, 56542, 51810, 40337, 51197, 50836, 57149, 53486, 44163, 50266, 54407,
            37671, 33305, 41364, 33586, 49924, 43898, 39639, 29441, 41963, 37368, 47213, 40755, 34053, 39837, 42675,
            31583, 21336, 27201, 26147, 38879, 31943, 27937, 20292, 31322, 31137, 29265, 30386, 25825, 28193, 31782,
            41727, 30409, 40977, 34019, 50506, 44235, 43551, 35020, 42508, 48415, 44022, 41314, 36998, 35993, 42983
        ]

        self.do_matmul(
            matrix_a, 10, 13,
            matrix_b, 13, 15,
            expected_result
        )
        
    def test_m0_bad_first_dim(self):
        self.do_matmul(
            [1, 2, 3, 4], -1, 2,
            [0, 0, 0, 0], 2, 2,
            [0, 0, 0, 0], 72
        )

    def test_m0_bad_second_dim(self):
        self.do_matmul(
            [1, 2, 3, 4], 3, 0,
            [0, 0, 0, 0], 2, 2,
            [0, 0, 0, 0], 72
        )  

    def test_m1_bad_first_dim(self):
        self.do_matmul(
            [1, 2, 3, 4], 2, 2,
            [0, 0, 0, 0], 0, 2,
            [0, 0, 0, 0], 73
        )

    def test_m1_bad_second_dim(self):
        self.do_matmul(
            [1, 2, 3, 4], 2, 2,
            [0, 0, 0, 0], 2, -1,
            [0, 0, 0, 0], 73
        )  

    def test_mismatch_dim(self):
        self.do_matmul(
            [1, 2, 3, 4], 10, 2,
            [0, 0, 0, 0], 10, 2,
            [0, 0, 0, 0], 74
        )          
    

                

    @classmethod
    def tearDownClass(cls):
        print_coverage("matmul.s", verbose=False)


class TestReadMatrix(TestCase):

    def do_read_matrix(self, fail='', code=0):
        t = AssemblyTest(self, "read_matrix.s")
        # load address to the name of the input file into register a0
        t.input_read_filename("a0", "inputs/test_read_matrix/test_input.bin")

        # allocate space to hold the rows and cols output parameters
        rows = t.array([-1])
        cols = t.array([-1])

        # load the addresses to the output parameters into the argument registers
        t.input_array("a1", rows)
        t.input_array("a2", cols)


        # call the read_matrix function
        t.call("read_matrix")

        # check the output from the function
        result = [1, 2, 3, 4, 5, 6, 7, 8, 9]
        t.check_array_pointer("a0", result)
        t.check_array(rows, [3])
        t.check_array(cols, [3])

        # generate assembly and run it through venus
        t.execute(fail=fail, code=code)

    def test_simple(self):
        self.do_read_matrix()

    def test_malloc_fail(self):
        self.do_read_matrix(fail='malloc', code=88)

    def test_fopen_fail(self):
        self.do_read_matrix(fail='fopen', code=90)        

    def test_fread_fail(self):
        self.do_read_matrix(fail='fread', code=91)  

    def test_fclose_fail(self):
        self.do_read_matrix(fail='fclose', code=92)                        

    @classmethod
    def tearDownClass(cls):
        print_coverage("read_matrix.s", verbose=False)


class TestWriteMatrix(TestCase):

    def do_write_matrix(self, fail='', code=0):
        t = AssemblyTest(self, "write_matrix.s")
        outfile = "outputs/test_write_matrix/student.bin"
        # load output file name into a0 register
        t.input_write_filename("a0", outfile)
        # load input array and other arguments
        input = t.array([1, 2, 3, 4, 5, 6, 7, 8, 9])
        t.input_array("a1", input)
        t.input_scalar("a2", 3)
        t.input_scalar("a3", 3)
        # TODO
        # call `write_matrix` function
        t.call("write_matrix")
        # generate assembly and run it through venus
        t.execute(fail=fail, code=code)
        # compare the output file against the reference
        if not fail:
            t.check_file_output(outfile, "outputs/test_write_matrix/reference.bin")

    def test_simple(self):
        self.do_write_matrix()

    def test_malloc_fail(self):
        self.do_write_matrix(fail='malloc', code=88)

    def test_fopen_fail(self):
        self.do_write_matrix(fail='fopen', code=93)

    def test_fread_fail(self):
        self.do_write_matrix(fail='fwrite', code=94)

    def test_fclose_fail(self):
        self.do_write_matrix(fail='fclose', code=95)

    @classmethod
    def tearDownClass(cls):
        print_coverage("write_matrix.s", verbose=False)


class TestClassify(TestCase):

    def make_test(self):
        t = AssemblyTest(self, "classify.s")
        t.include("argmax.s")
        t.include("dot.s")
        t.include("matmul.s")
        t.include("read_matrix.s")
        t.include("relu.s")
        t.include("write_matrix.s")
        return t

    def test_simple0_input0(self):
        t = self.make_test()
        out_file = "outputs/test_basic_main/student0.bin"
        ref_file = "outputs/test_basic_main/reference0.bin"
        args = ["inputs/simple0/bin/m0.bin", "inputs/simple0/bin/m1.bin",
                "inputs/simple0/bin/inputs/input0.bin", out_file]
        # call classify function
        t.call("classify")
        # generate assembly and pass program arguments directly to venus
        t.execute(args=args)

        # compare the output file and
        raise NotImplementedError("TODO")
        # TODO
        # compare the classification output with `check_stdout`

    @classmethod
    def tearDownClass(cls):
        print_coverage("classify.s", verbose=False)


class TestMain(TestCase):

    def run_main(self, inputs, output_id, label):
        args = [f"{inputs}/m0.bin", f"{inputs}/m1.bin", f"{inputs}/inputs/input0.bin",
                f"outputs/test_basic_main/student{output_id}.bin"]
        reference = f"outputs/test_basic_main/reference{output_id}.bin"
        t = AssemblyTest(self, "main.s", no_utils=True)
        t.call("main")
        t.execute(args=args, verbose=False)
        t.check_stdout(label)
        t.check_file_output(args[-1], reference)

    def test0(self):
        self.run_main("inputs/simple0/bin", "0", "2")

    def test1(self):
        self.run_main("inputs/simple1/bin", "1", "1")

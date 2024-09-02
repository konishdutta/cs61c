from unittests import *

"""
For each operation, you should write tests to test  on matrices of different sizes.
Hint: use dp_mc_matrix to generate dumbpy and numc matrices with the same data and use
      cmp_dp_nc_matrix to compare the results
"""


print("Testing matrix addition")
a = TestAdd()
a.test_small_add()
a.test_medium_add()
a.test_large_add()

print("\nTesting matrix subtraction")
b = TestSub()
b.test_small_sub()
b.test_medium_sub()
b.test_large_sub()

print("\nTesting absolute value")
c = TestAbs()
c.test_small_abs()
c.test_medium_abs()
c.test_large_abs()

print("\nTesting negation")
d = TestNeg()
d.test_small_neg()
d.test_medium_neg()
d.test_large_neg()

print("\nTesting matrix multiplication")
e = TestMul()
e.test_small_mul()
e.test_medium_mul()
e.test_large_mul()

print("\nTesting matrix pow")
f = TestPow()
f.test_small_pow()
f.test_medium_pow()
f.test_large_pow()

print("\nTesting get pow")
g = TestGet()
g.test_get()

print("\nTesting set pow")
h = TestGet()
h.test_get()
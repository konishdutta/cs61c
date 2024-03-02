import numpy as np

# Generate two matrices with random double-digit elements
A = np.random.randint(10, 100, size=(10, 13))
B = np.random.randint(10, 100, size=(13, 15))

# Perform matrix multiplication
C = np.dot(A, B)

print("Matrix A (10x13):\n", A)
print("\nMatrix B (13x15):\n", B)
print("\nResultant Matrix C (10x15):\n", C)
# Test for equality
li x1, 10
li x2, 10
beq x1, x2, label_equal   # Should branch

label_notequal:
# Execution continues here otherwise
add x3, x3, x3
jal x0, end1


label_equal:
# Execution continues here if x1 == x2
add x3, x1, x2

# Test for inequality
li x3, 10
li x4, 5
beq x3, x4, label_notequal # Should not branch

end1:
add x0, x0, x0

# Test for inequality
li x1, 10
li x2, 5
bne x1, x2, label_not_equal2   # Should branch

label_equal2:
addi x1, x1, 2

label_not_equal2:
addi x1, x1, 31

# Test for equality
li x3, 10
li x4, 10
bne x3, x4, label_equal2 # Should not branch

# Test for less than condition
li x1, -1
li x2, 5
blt x1, x2, label_less_than   # Should branch (as -1 is less than 5)

label_not_less_than:
# Execution continues here otherwise
addi x1, x1, 2

label_less_than:
# Execution continues here otherwise
addi x1, x1, 31

# Test for not less than condition
li x3, 10
li x4, 10
blt x3, x4, label_not_less_than # Should not branch

# Test for not less than condition
li x3, 100
li x4, 10
blt x3, x4, label_not_less_than # Should not branch

# Test for greater or equal
li x1, 10
li x2, 5
bge x1, x2, label_greater_equal   # Should branch

label_less:
addi x1, x1, 2

label_greater_equal:
addi x1, x1, 30

# Test for less than
li x3, 3
li x4, 5
bge x3, x4, label_less # Should not branch

# Test for greater or equal
li x1, 10
li x2, 10
bge x1, x2, label_greater_equal2   # Should branch
addi x1, x1, 2

label_greater_equal2:
addi x1, x1, 30

# Test for less than condition with unsigned values
li x10, 0  # Used to store results of branch tests

# Case where first operand is smaller, but would be larger if signed
li x1, 0xFFFFFFFF  # -1 if signed, 4294967295 if unsigned
li x2, 0x7FFFFFFF  # 2147483647 if signed
bltu x1, x2, label_bltu_not_taken  # Should not branch because 4294967295 > 2147483647

# Case where first operand is indeed smaller when considered unsigned
li x3, 0x1
li x4, 0x80000000  # -2147483648 if signed
bltu x3, x4, label_bltu_taken  # Should branch because 1 < 2147483648
j label_continue_bltu

label_bltu_not_taken:
li x10, 2  # Indicate branch was not taken

label_bltu_taken:
li x10, 1  # Indicate branch was taken

label_continue_bltu:
# Execution continues here after branch test

li x10, 0  # Used to store results of branch tests

# Case where first operand is greater and should be greater even if signed
li x1, 0x80000000  # -2147483648 if signed, 2147483648 if unsigned
li x2, 0x7FFFFFFF  # 2147483647 if signed
bgeu x1, x2, label_bgeu_taken  # Should branch because 2147483648 >= 2147483647
j label_continue_bgeu

label_bgeu_taken:
li x10, 1  # Indicate branch was taken

# Case where first operand is less, and would be less even if signed
li x3, 0x1
li x4, 0xFFFFFFFF  # -1 if signed, 4294967295 if unsigned
bgeu x3, x4, label_bgeu_not_taken  # Should not branch because 1 < 4294967295
bgeu x3, x3, label_continue_bgeu

label_bgeu_not_taken:
li x10, 2  # Indicate branch was not taken

label_continue_bgeu:
# Execution continues here after branch test
.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
# 	d = matmul(m0, m1)
# Arguments:
# 	a0 (int*)  is the pointer to the start of m0 
#	a1 (int)   is the # of rows (height) of m0
#	a2 (int)   is the # of columns (width) of m0
#	a3 (int*)  is the pointer to the start of m1
# 	a4 (int)   is the # of rows (height) of m1
#	a5 (int)   is the # of columns (width) of m1
#	a6 (int*)  is the pointer to the the start of d
# Returns:
#	None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 72.
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 73.
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 74.
# =======================================================
matmul:

    # Error checks
    addi, t0, x0, 1

    # if the rows of A are less than 1
    blt a1, t0, error_m0_dimensions
    # if the columns of A are less than 1
    blt a2, t0, error_m0_dimensions
    # if the rows of B are less than 1
    blt a4, t0, error_m1_dimensions
    # if the columns of B are less than 1
    blt a5, t0, error_m1_dimensions
    # if the columns of A don't match the rows of B
    bne a2, a4, error_dimension_mismatch

    # Prologue
    #save s0, s1, s2
    addi sp, sp, -44
    sw s0, 0(sp)
    sw s1, 4(sp)
    sw s2, 8(sp)
    sw s3, 12(sp)
    sw s4, 16(sp)
    sw s5, 20(sp)
    sw s6, 24(sp)
    sw s7, 28(sp)
    sw s8, 32(sp)
    sw s9, 36(sp)
    sw ra, 40(sp)

    #load up arguments in saved register
    #a0 --> s0
    add s0, a0, x0
    #a1 --> s1
    add s1, a1, x0
    #a3 --> s2
    add s2, a3, x0
    # the other arguments don't change so leave them

    # Thinking
    # We have some offset pointer that points to m0
    # m0_offset = colum * n
    # We have a different offset pointer for m1
    # m0_offset = row + k
    # FIRST OP
    # First row of m0 (stride = 1, length = row)
    # First column of m1 (stride = row, col = 1)
    # SECOND OP
    # First row of m0 (stride = 1, length = row)
    # Second column of m1 (stride = row, col = 2)
    # THIRD OP
    # First row of m0 (stride = 1, length = row)
    # Second column of m1 (stride = row, col = 2)    

    # s0 = m0_origin
    # s1 = rows in m0
    # s2 = m1_origin
    # s3 = outer_counter
    # s4 = inner_counter
    # s5 = d_offset
    # s6 = m0_offset
    # s7 = m1_offset
    # s8 = m1 cols
    # s9 = m0 cols
    # set outer_counter to 0
    add s3, x0, x0
    # set d_offset
    add s5, a6, x0
    # save m1 cols
    add s8, a5, x0
    # save m0 cols
    add s9, a2, x0

outer_loop_start:
    # if outer counter = s1 jump to outer_loop_end
    beq s3, s1, outer_loop_end
    # calculate m0 offset
    mul s6, s3, s9
    slli s6, s6, 2
    # set t0 to point to m0 offset
    add s6, s6, s0
    # reset m1 offset to m1 origin
    add s7, s2, x0
    # reset inner counter to 0
    add s4, x0, x0

inner_loop_start:
    # if inner counter = a5, jump to inner_loop_end
    beq s4, s8, inner_loop_end
    # offset increments by 1 at end of loop, we don't need to worry about it.
    # let's get the dot product
    # Dot Arguments:
    #   a0 (int*) is the pointer to the start of v0
    #   a1 (int*) is the pointer to the start of v1
    #   a2 (int)  is the length of the vectors
    #   a3 (int)  is the stride of v0
    #   a4 (int)  is the stride of v1
    add a0, s6, x0
    add a1, s7, x0
    add a2, s9, x0
    addi a3, x0, 1
    add a4, s8, x0
    jal dot
    # load the word into d
    sw a0, 0(s5)

    #increment d by 4 bytes
    addi, s5, s5, 4
    #increment m1 offset by 4 bytes
    addi, s7, s7, 4
    #increment inner counter by 1
    addi s4, s4, 1

    j inner_loop_start

inner_loop_end:
    # increment the outer loop
    addi s3, s3, 1
    j outer_loop_start

outer_loop_end:
    # Epilogue
    # reset the arguments
    add a0, s0, x0
    add a1, s1, x0
    add a3, s2, x0
    #re claim the stack
    lw s0, 0(sp)
    lw s1, 4(sp)
    lw s2, 8(sp)
    lw s3, 12(sp)
    lw s4, 16(sp)
    lw s5, 20(sp)
    lw s6, 24(sp)
    lw s7, 28(sp)
    lw s8, 32(sp)  
    lw s9, 36(sp)    
    lw ra, 40(sp)              
    addi sp, sp, 44
    
    ret

error_m0_dimensions:
    li a1, 72
    j exit2

error_m1_dimensions:
    li a1, 73
    j exit2

error_dimension_mismatch:
    li a1, 74
    j exit2
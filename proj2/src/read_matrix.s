.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
read_matrix:

    # Prologue
	# need to save s0, s1
    addi sp, sp, -36
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)    

    # The actual function
    # a0 = pointer to filename string
    # a1 = some int pointer for rows
    # a2 = some int pointer for columns
    # size of file read = 4 * (2 + m * n)
    # s0 = file descriptor
    # s1 = rownumpointer
    # s2 = colnumpointer
    # s3 = rownum
    # s4 = colnum
    # s5 = pointer to matrix (will increment)
    # s7 = final res
    add s1, a1, x0
    add s2, a2, x0
    #set args for fopen
    add a1, a0, x0
    add a2, x0, x0
    jal fopen # open the file
    # handling errors
    li t0, -1
    beq t0, a0, open_error
    # save file descr, we don't need to save the file string
    add s0, a0, x0

    # read number of rows
    add a1, s0, x0 # set args for fread rows
    add a2, s1, x0 # set buffer for rownum in a2
    li a3, 4
    jal fread
    li t1, 4
    bne t1, a0, read_error

    # read number of cols
    add a1, s0, x0 # set args for fread rows
    add a2, s2, x0 # set buffer for colnum in a2
    li a3, 4
    jal fread
    li t1, 4
    bne t1, a0, read_error

    # capture actual rows and cols
    lw s3, 0(s1)
    lw s4, 0(s2)

    # malloc enough space for the matrix
    # keep s6 as counter
    mul s6, s3, s4 # multiply s3 * s4
    slli s6, s6, 2 # multiply by 4    
    add a0, s6, x0
    jal malloc
    # handle malloc error
    beq a0, x0, malloc_error
    # else store matrix in s5
    add s5, a0, x0

    # read next int into matrix
    # set the args
    add a1, s0, x0 # set a1 to file descriptor
    add a2, s5, x0 # set matrix pointer in a2
    add a3, s6, x0
    jal fread
    bne s6, a0, read_error

    # close the file
    add a1, s0, x0 # set arg
    jal fclose
    bne a0, x0, close_error

    # set a0 to matrix
    add a0, s5, x0

    # Epilogue
    lw s7, 32(sp)
    lw s6, 28(sp)
    lw s5, 24(sp)
    lw s4, 20(sp)
    lw s3, 16(sp)  
    lw s2, 12(sp)  
    lw s1, 8(sp)   
    lw s0, 4(sp)
    lw ra, 0(sp)             
    addi sp, sp, 36

    ret

malloc_error:
    # close the file
    jal fclose    
    add a1, s0, x0 # set arg
    li a1, 88
    j exit2

open_error:
    li a1, 90
    j exit2

read_error:
    # close the file
    jal fclose    
    add a1, s0, x0 # set arg
    li a1, 91
    j exit2

close_error:
    li a1, 92
    j exit2

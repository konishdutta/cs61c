.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
# - If you receive an fopen error or eof,
#   this function terminates the program with error code 93.
# - If you receive an fwrite error or eof,
#   this function terminates the program with error code 94.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 95.
# ==============================================================================
write_matrix:

    # Prologue
    
    # Save stuff in stack
    addi sp, sp, -28
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)

    # Save argument registers
    mv s1, a1 # s1 = matrix_ptr
    mv s2, a2 # s2 = row
    mv s3, a3 # s3 = cols

    # Open the file in write mode
    # Set the arguments for fopen
    add a1, a0, x0 # a1 = filename_ptr
    li a2, 1 # a2 = write
    jal fopen
    # Handle open error
    li t0, -1
    beq t0, a0, open_error
    mv s0, a0 # s0 = file_descriptor

    # Write rows and cols in a buffer
    # Set up malloc for 2 integers (8 bytes)
    li a0, 8
    jal malloc
    # handle malloc error
    beq a0, x0, malloc_error
    mv s4, a0 # s4 = row and col buffer
    sw s2, 0(s4) # rc_buff[0] = row
    sw s3, 4(s4) # rc_buff[1] = col

    # Write the rows and cols
    # Set up arguments for fwrite
    mv a1, s0 # a1 = file_descriptor
    mv a2, s4 # a2 = @rc_buff
    li a3, 2 # a3 = read 2 items
    li a4, 4 # a4 = 4 bytes (size of each item)
    # Call function
    jal fwrite
    # handle write errors
    li t0, 2
    bne t0, a0, write_error

    # free the malloc
    mv a0, s4
    jal free

    # Create the counter
    # s5 = counter
    mul s5, s2, s3 # s5 = rows * cols

write_loop:
    # if counter == 0, exit
    beq s5, x0, exit_write

    # write a single number
    # Set up arguments for fwrite
    mv a1, s0 # a1 = file_descriptor
    mv a2, s1 # a2 = matrix_ptr
    li a3, 1 # a3 = read 1 item
    li a4, 4 # a4 = 4 bytes (size of each item)
    # Call function
    jal fwrite
    # handle write errors
    li t0, 1
    bne t0, a0, write_error

    # increment matrix_ptr by 1 int
    addi, s1, s1, 4

    # decrement counter
    addi, s5, s5, -1

    # restart loop
    j write_loop

exit_write:
    # Close the file
    add a1, s0, x0 # set arg
    jal fclose
    bne a0, x0, close_error

    # Epilogue
    lw s5, 24(sp)
    lw s4, 20(sp)
    lw s3, 16(sp)
    lw s2, 12(sp)
    lw s1, 8(sp)
    lw s0, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 28

    ret

open_error:
    li a1, 93
    j exit2

malloc_error:
    # close the file
    add a1, s0, x0 # set arg    
    jal fclose    
    li a1, 88
    j exit2

write_error:
    # close the file
    add a1, s0, x0 # set arg    
    jal fclose   
    li a1, 94
    j exit2

close_error:
    li a1, 95
    j exit2

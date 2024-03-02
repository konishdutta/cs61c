.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
# 	a0 (int*) is the pointer to the array
#	a1 (int)  is the # of elements in the array
# Returns:
#	None
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 78.
# ==============================================================================
relu:
    # Prologue
    bge x0, a1, error #if 0 >= a1, error
    li t1, 0 # t1; counter = 0

loop_start:
    beq t1, a1, loop_end # if counter = args, end
    slli t2, t1, 2 # mult counter by 4
    add t2, a0, t2 # add counter offset to pointer
    lw t0, 0(t2) # load current array elem
    bge t0, x0, loop_continue #if elem >= 0 continue
    sw x0, 0(t2) #else load 0 into the word

loop_continue:
    addi t1, t1, 1
    beq x0, x0, loop_start

loop_end:
    # Epilogue
    
	ret

error:
    li a1, 78
    j exit2
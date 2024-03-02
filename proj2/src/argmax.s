.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the # of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================
argmax:

    # Prologue
    #t0 counter
    #t1 curr_max_index
    #t2 curr_max_elem
    #t3 candidate_max_pointer
    #t4 candidate_max_elem
    li t0, 1
    ble t0, a1, no_error
    li a1, 77
    j exit2

no_error:
    li t0, 1 # counter = 1
    add t1, x0, x0 # t1 = 0
    lw t2, 0(a0) #t2 = first elem value

loop_start:
    beq t0, a1, loop_end
    add t3, t0, x0
    slli t3, t3, 2 #t2 = counter * 4
    add t3, t3, a0 #t2 = curr elem pointer
    lw t4, 0(t3) #t4 = curr elem
    bge t2, t4, loop_continue #if max elem >= curr elem continue
    add t1, t0, x0 #t1 = curr counter
    add t2, t4, x0 #t2 = curr elem

loop_continue:
    addi t0, t0, 1
    beq x0, x0, loop_start

loop_end:
    add a0, t1, x0
    
    # Epilogue
    ret

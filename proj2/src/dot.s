.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
dot:
    # if stride, len < 1 call error
    addi t0, x0, 1
    blt a2, t0, error_len
    blt a3, t0, error_stride
    blt a4, t0, error_stride

    # t0 = curr v0 offset
    # t1 = curr v1 offset
    # t2 = curr counter
    # t3 = v0 value
    # t4 = v1 value
    # t5 = subvalue
    # t6 = res
    add t2, x0, x0 # counter
    add t6, x0, x0 # res

    # Prologue


loop_start:
    beq t2, a2, loop_end

    mul t0, t2, a3 # t0 = stride * counter
    mul t1, t2, a4 # t1 = stride * counter
    slli t0, t0, 2 # mul * 4 to get offset
    slli t1, t1, 2
    # add offset to v0, v1 addresses
    add t0, t0, a0
    add t1, t1, a1
    lw t3, 0(t0)
    lw t4, 0(t1)
    mul t5, t3, t4
    add t6, t6, t5
    addi t2, t2, 1
    beq x0, x0, loop_start

loop_end:
    add a0, t6, x0
    # Epilogue

    ret

error_len:
    li a1, 75
    j exit2

error_stride:
    li a1, 76
    j exit2
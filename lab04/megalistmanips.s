.globl map

.data
arrays: .word 5, 6, 7, 8, 9
        .word 1, 2, 3, 4, 7
        .word 5, 2, 7, 4, 3
        .word 1, 6, 3, 8, 4
        .word 5, 2, 7, 8, 1

start_msg:  .asciiz "Lists before: \n"
end_msg:    .asciiz "Lists after: \n"

.text
main:
    jal create_default_list
    mv s0, a0   # v0 = s0 is head of node list
    # moving linklist of arrays into s0

    #print "lists before: "
    la a1, start_msg
    li a0, 4
    ecall
    # load a string into a1
    # load an immediate 4 into a0
    # ecalling

    #print the list
    add a0, s0, x0
    jal print_list

    # print a newline
    jal print_newline

    # issue the map call
    add a0, s0, x0      # load the address of the first node into a0
    la a1, mystery     # load the address of the function into a1

    jal map

    # print "lists after: "
    la a1, end_msg
    li a0, 4
    ecall

    # print the list
    add a0, s0, x0
    jal print_list

    li a0, 10
    ecall

map:
    addi sp, sp, -12 # decr stack by 12
    sw ra, 0(sp) # stk[0] = ra
    sw s1, 4(sp) # stk[1] = s1
    sw s0, 8(sp) # stk[2] = s0

    beq a0, x0, done    # if we were given a null pointer, we're done.

    add s0, a0, x0      # save address of this node in s0
    # s0[0] = pointer to the array

    add s1, a1, x0      # save address of function in s1
    add t0, x0, x0      # t0 is a counter

    # remember that each node is 12 bytes long:
    # - 4 for the array pointer
    # - 4 for the size of the array
    # - 4 more for the pointer to the next node

    # also keep in mind that we should not make ANY assumption on which registers
    # are modified by the callees, even when we know the content inside the functions 
    # we call. this is to enforce the abstraction barrier of calling convention.
mapLoop:
    lw t1, 0(s0)
    #add t1, s0, x0      # load the address of the array of current node into t1
    # MISTAKE 5: you need to load the underlying pointer
    lw t2, 4(s0)        # load the size of the node's array into t2

    slli t3, t0, 2 # t3 = t0 * 4
    add t1, t1, t3      # offset the array address by the count
    # MISTAKE 1: We are not properly ofsetting the address.
    # Need to multiply the counter by 4 in order to do this.

    # MISTAKE 4: You're using temp registeries and mystery is unsaving them.
    # Need to save t0, t1, t2

    addi sp, sp, -12
    sw t0, 0(sp)
    sw t1, 4(sp)
    sw t2, 8(sp)

    lw a0, 0(t1)        # load the value at that address into a0 -- good
    jalr s1             # call the function on that value. -- good

    # Add epilogue here
    lw t2, 8(sp)    
    lw t1, 4(sp)    
    lw t0, 0(sp)
    addi sp, sp, 12

    sw a0, 0(t1)        # store the returned value back into the array
    addi t0, t0, 1      # increment the count

    bne t0, t2, mapLoop # repeat if we haven't reached the array size yet

    lw a0, 8(s0)        # load the address of the next node into a0
    # MISTAKE 2: Address of the next node is actually 12 away, not 8
    # You also cannot load an address like that. Needs to be an add
    
    add a1, s1, x0
    #lw a1, 0(s1)        # put the address of the function back into a1 to prepare for the recursion
    # MISTAKE 3 - we are loading whatever the address points to, not the address itself
    # Need to change this to an add function

    jal  map            # recurse
done:
    lw s0, 8(sp) # load everything back
    lw s1, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 12 # incr the stack
    jr ra

mystery:
    # returns n^2 + n
    mul t1, a0, a0
    add a0, t1, a0
    jr ra

create_default_list:
    addi sp, sp, -4
    # decr sp by -4
    sw ra, 0(sp)
    # add ra to stack
    li s0, 0  # pointer to the last node we handled
    li s1, 0  # number of nodes handled
    li s2, 5  # size
    la s3, arrays
loop: #do...
    li a0, 12   # each node is 12, 4 for address to array, 4 for size, 4 for next
    jal malloc      # get memory for the next node
    mv s4, a0
    li a0, 20
    jal  malloc     # get memory for this array

    sw a0, 0(s4)    # node->arr = malloc
    lw a0, 0(s4)
    mv a1, s3
    jal fillArray   # copy ints over to node->arr -- good

    sw s2, 4(s4)    # node->size = size (4) -- good
    sw  s0, 8(s4)   # node-> next = previously created node -- good but creates it backwards

    add s0, x0, s4  # last = node
    addi s1, s1, 1  # i++
    addi s3, s3, 20 # s3 points at next set of ints -- good
    li t6 5
    bne s1, t6, loop # ... while i!= 5
    mv a0, s4
    # move the array to a0
    lw ra, 0(sp)
    addi sp, sp, 4
    # incr sp by 4
    jr ra

fillArray: lw t0, 0(a1) #t0 gets array element
    sw t0, 0(a0) #node->arr gets array element
    lw t0, 4(a1)
    sw t0, 4(a0)
    lw t0, 8(a1)
    sw t0, 8(a0)
    lw t0, 12(a1)
    sw t0, 12(a0)
    lw t0, 16(a1)
    sw t0, 16(a0)
    jr ra

print_list:
    bne a0, x0, printMeAndRecurse
    jr ra   # nothing to print
printMeAndRecurse:
    mv t0, a0 # t0 gets address of current node
    lw t3, 0(a0) # t3 gets array of current node
    li t1, 0  # t1 is index into array
printLoop:
    slli t2, t1, 2 # t2 = t1 * 4
    add t4, t3, t2 # adds 4 * i to t3, which gets address of curr elem
    lw a1, 0(t4) # a1 gets value in current node's array at index t1
    li a0, 1  # prepare for print integer ecall
    ecall
    li a1, ' ' # a1 gets address of string containing space
    li a0, 11  # prepare for print string ecall
    ecall
    addi t1, t1, 1 # t1 += 1
    li t6 5 # t6 = 5
    bne t1, t6, printLoop # ... while i!= 5
    li a1, '\n'
    li a0, 11
    ecall
    lw a0, 8(t0) # a0 gets address of next node
    j print_list # recurse. We don't have to use jal because we already have where we want to return to in ra

print_newline:
    li a1, '\n'
    li a0, 11
    ecall
    jr ra

malloc:
    mv a1, a0 # Move a0 into a1 so that we can do the syscall correctly
    li a0, 9
    ecall
    jr ra

.globl classify

.text
classify:
    # =====================================
    # COMMAND LINE ARGUMENTS
    # =====================================
    # Args:
    #   a0 (int)    argc
    #   a1 (char**) argv
    #   a2 (int)    print_classification, if this is zero, 
    #               you should print the classification. Otherwise,
    #               this function should not print ANYTHING.
    # Returns:
    #   a0 (int)    Classification
    # Exceptions:
    # - If there are an incorrect number of command line args,
    #   this function terminates the program with exit code 89.
    # - If malloc fails, this function terminats the program with exit code 88.
    #
    # Usage:
    #   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>

    # Handle arg count errors
    li t0, 5
    bne t0, a0, incorrect_args

    # Prologue
    addi sp, sp, -40
    sw ra, 0(sp)
    sw s0, 4(sp)
    sw s1, 8(sp)
    sw s2, 12(sp)
    sw s3, 16(sp)
    sw s4, 20(sp)
    sw s5, 24(sp)
    sw s6, 28(sp)
    sw s7, 32(sp)
    sw s8, 36(sp)

    # Save arguments
    lw s0, 4(a1) # s0 = @m0_file
    lw s1, 8(a1) # s1 = @m1_file
    lw s2, 12(a1) # s2 = input    
    lw s3, 16(a1) # s3 = output
    mv s7, a2


	# =====================================
    # LOAD MATRICES
    # =====================================

    # malloc space for 4 integers
    li a0, 24
    jal malloc
    # handle malloc error
    beq a0, x0, malloc_error
    mv s4, a0
    # s4[0] = m0_rows
    # s4[1] = m0_cols
    # s4[2] = m1_rows
    # s4[3] = m1_cols
    # s4[4] = input_rows
    # s4[5] = input_cols     

    # Load pretrained m0

    # set args
    # a0 = @m0_file
    mv a0, s0
    mv a1, s4 # a1 = s4[0]
    addi a2, s4, 4 # a2 = s4[1]
    jal read_matrix

    # we are overwriting the filenames
    # since we no longer use them.
    # s0 = @m0_matrix
    mv s0, a0

    # Load pretrained m1
    # set args
    # a0 = @m1_file
    mv a0, s1
    addi a1, s4, 8 # a2 = s4[2]
    addi a2, s4, 12 # a2 = s4[3]
    jal read_matrix
    
    # s1 = @m1_matrix
    mv s1, a0

    # Load input matrix
    # set args
    # a0 = @input_file
    mv a0, s2
    addi a1, s4, 16 # a2 = s4[4]
    addi a2, s4, 20 # a2 = s4[5]
    jal read_matrix
    
    # s2 = @input_matrix
    mv s2, a0

    # =====================================
    # RUN LAYERS
    # =====================================
    # 1. LINEAR LAYER:    m0 * input

    # malloc an output matrix
    lw t0, 0(s4) # t0 = m0_row
    lw t1, 4(s4) # t1 = m0_col
    lw t2, 16(s4) # t3 = input_row
    lw t3, 20(s4) # t4 = input_col
    # output matrix = m0_row x input_col
    # s6 = num_elem_in_output
    mul s6, t0, t3
    slli a0, s6, 2 # multiply by 4 bytes
    jal malloc
    # handle malloc error
    beq a0, x0, malloc_error
    mv s5, a0 # s5 = hidden_layer_matrix

    # malloc an output matrix
    lw t0, 0(s4) # t0 = m0_row
    lw t1, 4(s4) # t1 = m0_col
    lw t2, 16(s4) # t3 = input_row
    lw t3, 20(s4) # t4 = input_col    

    # Set arguments to call matmul
    # a0 = m0
    mv a0, s0
    # load m0 rows and cols
    mv a1, t0
    mv a2, t1

    # a3 = input
    mv a3, s2
    # load input rows and cols
    mv a4, t2
    mv a5, t3
    mv a6, s5
    
    # run matmul
    jal matmul


    # 2. NONLINEAR LAYER: ReLU(m0 * input)

    # set up args for relu
    mv a0, s5
    mv a1, s6
    jal relu

    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)

    # malloc an output matrix
    # replacing s2 and s6 with values we need to use later
    # we no longer need num_elements in hidden layer (s6)
    # we no longer need input matrix (s2)
    # free s2
    mv a0, s2
    jal free

    lw s2, 8(s4) # s2 = m1_row
    lw t1, 12(s4) # t1 = m2_col
    lw t2, 0(s4) # t3 = output_row (m0_row)
    lw s6, 20(s4) # s6 = output_col (input_col)
    
    # output matrix = m1_row x output_col
    # s6 = num_elem_in_output (overwriting hidden layer)
    mul a0, s2, s6
    slli a0, a0, 2 # multiply by 4 bytes
    jal malloc

    # handle malloc error
    beq a0, x0, malloc_error

    mv s8, a0 # s8 = final_output_matrix

    # Set arguments to call matmul
    # a0 = m1
    mv a0, s1
    # load m1 rows and cols
    mv a1, s2
    lw a2, 12(s4)

    # a3 = hidden_layer_matrix
    mv a3, s5

    # load input rows and cols
    lw a4, 0(s4)
    mv a5, s6
    mv a6, s8
    
    # run matmul
    jal matmul

    # =====================================
    # WRITE OUTPUT
    # =====================================
    # Write output matrix

    # set args
    # a0 = output_file
    mv a0, s3
    # a1 = output_matrix
    mv a1, s8
    mv a2, s2
    mv a3, s6
    jal write_matrix

    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================
    # Call argmax
    mv a0, s8
    mul a1, s2, s6
    jal argmax

    # Print classification
    bne s7, x0, end
    #slli a0, a0, 2 # multiply argmax by 4
    #add a0, a0, s6 # increment s6 by argmax idx
    #lw a1, 0(a0)
    mv a1, a0
    jal print_int

    # Print newline afterwards for clarity
    li a1 '\n'
    jal print_char

end:

    # Free everything
    mv a0, s4
    jal free
    mv a0, s0
    jal free    
    mv a0, s1
    jal free      
    mv a0, s8
    jal free
    mv a0, s5
    jal free

    

    # Epilogue
    lw s8, 36(sp)
    lw s7, 32(sp)    
    lw s6, 28(sp)
    lw s5, 24(sp)
    lw s4, 20(sp)
    lw s3, 16(sp)
    lw s2, 12(sp)
    lw s1, 8(sp)
    lw s0, 4(sp)
    lw ra, 0(sp)
    addi sp, sp, 40

    ret

incorrect_args:
    li a1, 89
    j exit2

malloc_error:    
    li a1, 88
    j exit2
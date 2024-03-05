# MNIST Neural Net in RISC-V

## Project Overview

I implemented a neural network to identify handwritten digits using RISC-V assembly language as part of self-learning from UC Berkeley's CS61C "Machine Structures" curriculum. The project involved:
* Understanding the mathematics behind a neural net
* Implementing mathematics in low-level assembly
* Managing registers, memory, and stack efficiently
* Manipulating file I/O in assembly
* Building a test suite to debug assembly 

## Biggest Challenges

* **Debugging Assembly Code:** Debugging at the assembly level was particularly challenging due to the lack of high-level abstractions. Errors required stepping through each instruction and using exit codes creatively.
* **Efficient Memory Usage:** Managing memory on the heap and ensuring efficient use of resources was a complex task that required careful planning and execution.
* **Implementing Complex Mathematical Functions:** Translating high-level mathematical operations into efficient assembly code required a deep understanding of the underlying mathematical principles and assembly language optimization techniques.

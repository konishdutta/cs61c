# 2/5 Stage RISC-V CPU
I built the following components in Logism from scratch:
* CPU Datapath
* ALU
* Register File
* Control Unit
* Immediate Generator
* Branch Comparator
* CSR
* Successfully built a 2 stage (and 5 stage) pipeline
* Custom Logism test suite
## Where I got stuck and what I learned
* ALU Forwarding was challenging to implement because I was getting lost in a sea of wires. I realized the key to building it was to work at one abstraction level higher. I built a "Forwarding Unit" that managed the decision complexity of when to forward and hooked that up in the datapath.
* I used a ROM to set up most of the control signals instead of getting lost in a ton of gates. I mapped each opcode to a set of digits that I then split to inform most of the control signals.
* Handling branches required an additional layer, but I used funct3 as a data selector in a multiplexor to decide whether to turn on the PCSel flag.
* In the case of a branch, I killed the incoming instruction by setting it to 0x00000000, which is a no-op.
* My initial implementation of registers was overly complex and didn't work when the register was supposed to write "0". It involved log32 layers of OR gates with an AND gate on the write flag. Because a "0" value wouldn't flip the "AND" gate, "0"s were not recorded, which was a bug. I fixed this by adding combinational logic to route the write flag at the Regfile layer. The register itself simply passed through the incoming value and write flag with no additional logic.

# DPU
DPU - Dave Processng Unit

The Dave Processing Unit is a virtual 32-bit ARM CPU.  This CPU uses a LOAD/STORE architecture and a register file of 16 registers.  Right now, it is capable of reading a file into memory, writing memory to a file, modifying memory, and executing instructions.  The 32-bit instruction register can hold 2 instructions, as 16-bit thumb instructions are used.  Instruction types include Immediate, Load/Store, Data Processing, Branch, Conditional Branch, Data Processing, and Push/Pull.  The only instruction type left to implement is Push/Pull. 

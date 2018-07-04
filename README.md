_dpu_ - By: Dave Mariano


## DPU - Dave Processng Unit


The Dave Processing Unit is a virtual 32-bit ARM CPU.  This CPU uses a Load/Store architecture and a register file of 16 registers.  It is capable of reading a file into memory, writing memory to a file, modifying memory, memory dumps, displaying registers, and executing instructions.  The 32-bit instruction register holds 2 instructions at a time, as 16-bit thumb instructions are used.  Instruction types include Immediate, Load/Store, Data Processing, Branch, Conditional Branch, Data Processing, and Push/Pull.  


### Instruction Set


#### Load/Store
* LDB
* LDR
* STB
* STR

#### Push/Pull
* PSH
* PSHH
* PSHR
* PUL
* PULR
* PULH

#### Immediate
* MOV
* CMP
* ADD
* SUB

#### Data Processing
* AND
* EOR
* SUB
* SXB
* ADD
* ADC
* LSR
* LSL
* TST
* TEQ
* CMP
* ROR
* ORR
* MOV
* BIC
* MVN

#### Branches
* BRA
* BRL
* BEQ
* BNE
* BCS
* BCC
* BMI
* BPL
* BHI
* BLS
* BAL

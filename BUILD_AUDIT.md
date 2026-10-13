# Build audit — StonePSP360 v8.11b

- Host tests: Allegrex reset, PSP RAM/aliases, core-memory lifecycle, instruction subset.
- CPU subset: ADDU, SUBU, AND/OR/XOR/NOR, SLT/SLTU, shifts, ADDIU, SLTI/SLTIU, ANDI/ORI/XORI/LUI, BEQ/BNE/BLEZ/BGTZ, J/JAL/JR/JALR, LB/LBU/LH/LHU/LW/SB/SH/SW.
- Delay-slot PC model is included for the implemented branches/jumps.
- Unsupported instruction, invalid fetch and data-memory faults return explicit statuses.
- CPU interpreter is not yet connected to a game loader or full frame run loop.
- ELF32 PowerPC big-endian cloud build is required CI check; no hardware/game compatibility is claimed.

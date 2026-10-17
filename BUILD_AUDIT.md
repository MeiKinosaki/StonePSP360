# Build audit — StonePSP360 v8.18b

- Added dedicated host test for taken and not-taken BEQ delay-slot behaviour.
- Added assertions for PC progression after a branch, instruction execution in the delay slot, and arrival at target.
- Added memory-fault and illegal-instruction checks confirming the faulting PC is retained.
- CI compiles this new test under C99 with -Wall -Wextra -Werror.
- This improves regression coverage; it does not add missing CPU opcodes, GPU/audio, system services, or hardware validation.

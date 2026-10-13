# Build audit — StonePSP360 v8.10b

- Target: Xbox 360 RGH/JTAG using Free60/libXenon.
- CI: host tests for Allegrex reset, PSP RAM aliases, and core/memory integration; then ELF32 PowerPC big-endian build.
- PSP RAM: 32 MiB allocation, 8/16/32-bit little-endian access, range checks, mirrors at 0x08000000, 0x48000000, 0x88000000, 0xA8000000, and 0xC8000000.
- Core owns RAM after successful initialization and exposes checked read/write accessors.
- CPU fetch/decode/execute remains unimplemented; psp_core_run_frame() returns PSP_CORE_NOT_IMPLEMENTED.
- Artifact contains StonePSP360.elf32 and xenon.elf; xenon.elf is a copy, not default.xex.
- No physical-console or game-compatibility test is claimed.

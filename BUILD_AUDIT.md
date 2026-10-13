# Build audit — StonePSP360 v8.9b

- Target: Xbox 360 with Free60/libXenon.
- Cloud CI: host tests for Allegrex reset and PSP RAM, followed by ELF32 PowerPC big-endian build.
- PSP RAM: 32 MiB allocation; 8/16/32-bit little-endian access; aliases at 0x08000000, 0x48000000, 0x88000000, 0xC8000000.
- Memory module status: unit-tested in host CI if the workflow passes; not integrated with Allegrex execution.
- Output artifact: StonePSP360-v8.9b-xbox360-elf, containing StonePSP360.elf32 and xenon.elf.
- xenon.elf is an ELF32 copy, not default.xex.
- No claim of physical Xbox 360/XeLL validation or game compatibility without separate testing.

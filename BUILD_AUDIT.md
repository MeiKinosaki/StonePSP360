# Build audit — StonePSP360 v8.15b

- New host-only inspector validates PBP section offsets and basic ELF32 little-endian MIPS executable headers.
- Program-header table bounds are checked before being reported.
- PBP DATA.PSP is identified as a direct ELF only if its bytes begin with ELF magic and pass validation.
- No PRX decompression/decryption, segment loading, relocation, import resolution, or guest start from file is implemented.
- CI tests CPU reset, RAM, core integration, Allegrex instruction subset, and PBP/ELF inspector, then builds ELF32 PowerPC big-endian using cloud libXenon.
- No physical Xbox 360 test or game compatibility is claimed.

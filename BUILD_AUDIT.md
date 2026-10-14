# Build audit — StonePSP360 v8.16b

- Loader supports raw ELF32 little-endian MIPS ET_EXEC and PBP with a direct ELF in DATA.PSP.
- PT_LOAD headers are preflighted for file range, guest RAM range, p_filesz <= p_memsz, alignment, and executable entry point.
- Segment bytes are copied into PSP RAM and BSS tails are zeroed.
- All loadable segment ranges are checked before RAM is modified; tests include invalid destination preserving a marker.
- PRX crypto/decompression, relocations, imports/NIDs, disk I/O, scheduler, GPU/audio and real-hardware validation remain absent.
- CI tests CPU reset, RAM, core integration, Allegrex execution, and PBP/ELF loader before cloud libXenon ELF32 PowerPC build.

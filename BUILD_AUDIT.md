# Auditoria — StonePSP360

## v8.5b
- [x] Build cloud passou.
- [x] ELF32 PowerPC publicado.
- [ ] Teste em Xbox 360 real.

## v8.6b
- [x] Esqueleto Allegrex/API PSP adicionado.
- [x] Logs identificaram a falha: `undefined reference to main`.
- [x] Causa: Makefile recursivo procurava os fontes relativos à pasta `build/`.

## v8.7b
- [x] Makefile alterado para descobrir os fontes usando caminhos absolutos baseados em `PROJECTDIR`.
- [x] Workflow de nuvem valida ELF32, big-endian e PowerPC.
- [x] Workflow prepara artefatos `StonePSP360.elf32` e `xenon.elf` apenas após a compilação e validação.
- [ ] Build v8.7b confirmado.
- [ ] Artefato v8.7b publicado.
- [ ] Teste no XeLL/Xbox 360.
- [ ] Núcleo PSP funcional.

**Nota:** `xenon.elf` não é `default.xex`. A aplicação ainda não emula jogos PSP.

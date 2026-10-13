# Auditoria — StonePSP360

## Histórico confirmado
- v8.5b: [x] GitHub Actions compilou e publicou ELF32 PowerPC.
- v8.6b: [x] Esqueleto Allegrex adicionado; [x] build falhou com undefined reference to main.
- v8.7b: [x] descoberta de fontes melhorada; [x] logs ainda mostraram link sem main.

## v8.8b
- [x] Regra $(OUTPUT).elf: $(OFILES) adicionada, conforme o template Free60.
- [x] Teste host para reset do estado Allegrex adicionado.
- [x] Workflow executa teste host e valida cabeçalho ELF32/big-endian/PowerPC.
- [ ] Teste host aprovado no Actions.
- [ ] ELF32 v8.8b gerado e artefato publicado.
- [ ] Teste em Xbox 360/XeLL.
- [ ] Núcleo PSP funcional.

**Nota:** o teste unitário valida apenas a rotina de reset. A versão ainda não executa instruções PSP nem jogos.

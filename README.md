# StonePSP360

**Versão de desenvolvimento:** v8.12b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.12b

- Adicionadas operações HI/LO: MFHI, MTHI, MFLO, MTLO, MULT, MULTU, DIV e DIVU.
- Adicionados BLTZ, BGEZ, BLTZAL e BGEZAL.
- Testes cobrem multiplicação/divisão assinada, leitura de HI/LO e delay slot de BEQ.
- O CI executa os testes host antes da compilação ELF32 PowerPC big-endian.

## Limitações

O interpretador continua parcial. Não há exceções completas, COP0 completo, VFPU, caches, temporização, carregador ISO/CSO, GPU/GE, áudio, firmware/serviços PSP ou compatibilidade de jogos validada. DIV/DIVU com divisor zero deixam HI/LO inalterados neste modelo simplificado; esse comportamento ainda precisa de modelagem específica do Allegrex. O núcleo ainda não executa uma sessão de jogo completa.

## Build na nuvem

Veja [GitHub Actions](https://github.com/MeiKinosaki/StonePSP360/actions). O artefato `xenon.elf` é uma cópia do ELF32, não um `default.xex`. Não foi confirmado teste em console físico.

## Histórico

- v8.8b: regra de link e teste de reset Allegrex.
- v8.9b: RAM PSP isolada com testes.
- v8.10b: RAM integrada ao ciclo de vida do núcleo.
- v8.11b: primeiro subconjunto de instruções Allegrex.
- v8.12b: operações HI/LO, multiplicação/divisão e branches REGIMM.

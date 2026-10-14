# StonePSP360

**Versão de desenvolvimento:** v8.14b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.14b

- Ligada a API `allegrex_run` ao núcleo PSP por `psp_core_start` e `psp_core_run_steps`.
- O núcleo converte resultados da CPU em estados explícitos para instrução ilegal e falha de memória.
- Teste de integração escreve duas instruções na RAM, inicia a CPU no endereço de entrada e executa duas etapas pela API pública.
- `psp_core_run_frame` continua explicitamente não implementado até existir scheduler, GPU e temporização.
- CI compila os testes host e o ELF32 PowerPC big-endian na toolchain em nuvem.

## Limitações

O interpretador Allegrex é parcial. Ainda faltam carregador de executáveis/ISO/CSO, exceções completas, COP0, VFPU, caches, scheduler PSP, GPU/GE, áudio, firmware/serviços e compatibilidade de jogos validada. Não há teste confirmado em console físico.

## Build na nuvem

Veja [GitHub Actions](https://github.com/MeiKinosaki/StonePSP360/actions). O artefato `xenon.elf` é uma cópia do ELF32, não um `default.xex`. Não há teste confirmado em console físico.

## Histórico

- v8.8b: regra de link e teste de reset Allegrex.
- v8.9b: RAM PSP isolada com testes.
- v8.10b: RAM integrada ao ciclo de vida do núcleo.
- v8.11b: primeiro subconjunto de instruções Allegrex.
- v8.12b: operações HI/LO, multiplicação/divisão e branches REGIMM.
- v8.13b: loop de execução Allegrex limitado por orçamento.
- v8.14b: API pública do núcleo executa o interpretador Allegrex com orçamento limitado.

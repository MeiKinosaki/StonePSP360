# StonePSP360

**Versão de desenvolvimento:** v8.13b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.13b

- Adicionada a API `allegrex_run`, que executa até um orçamento de instruções por chamada.
- A API informa quantas instruções concluíram e para ao encontrar instrução ilegal ou falha de memória.
- Orçamento zero é um no-op; a execução é limitada para permitir que um futuro scheduler controle o tempo de CPU.
- Teste automatizado cobre execução de três instruções e orçamento zero.
- CI executa quatro testes host e compila ELF32 PowerPC big-endian na toolchain em nuvem.

## Limitações

O interpretador ainda é parcial e não está conectado a um carregador de jogos ou scheduler do PSP. Exceções completas, COP0, VFPU, caches, temporização real, firmware/serviços, ISO/CSO, GPU/GE e áudio ainda não estão implementados. Não há compatibilidade de jogos validada.

## Build na nuvem

Veja [GitHub Actions](https://github.com/MeiKinosaki/StonePSP360/actions). O artefato `xenon.elf` é uma cópia do ELF32, não um `default.xex`. Não há teste confirmado em console físico.

## Histórico

- v8.8b: regra de link e teste de reset Allegrex.
- v8.9b: RAM PSP isolada com testes.
- v8.10b: RAM integrada ao ciclo de vida do núcleo.
- v8.11b: primeiro subconjunto de instruções Allegrex.
- v8.12b: operações HI/LO, multiplicação/divisão e branches REGIMM.
- v8.13b: loop de execução Allegrex limitado por orçamento.

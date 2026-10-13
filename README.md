# StonePSP360

**Versão de desenvolvimento:** v8.11b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.11b

- Primeiro passo do interpretador Allegrex/MIPS: subset aritmético, shifts, lógica, branches/jumps com modelo de delay slot e load/store.
- Instruções não suportadas retornam `ALLEGREX_STEP_ILLEGAL_INSTRUCTION`; falhas de memória retornam status separado.
- Teste host executa uma sequência pequena de instruções, verifica registradores, load/store e o registrador zero.
- CI executa quatro testes host antes da compilação ELF32 PowerPC big-endian.

## Limitações

O interpretador ainda é parcial. Overflow/exceções, interrupções, COP0 completo, VFPU, multiplicação/divisão, caches, temporização, BIOS/firmware, carregador ISO/CSO, gráficos e áudio não estão implementados. O interpretador ainda não está ligado a um carregador de jogos nem ao loop principal de execução. Nenhum jogo é declarado compatível.

## Build na nuvem

Veja [GitHub Actions](https://github.com/MeiKinosaki/StonePSP360/actions). O artefato `xenon.elf` é uma cópia do ELF32, não um `default.xex`. É necessário testar em hardware/XeLL para confirmar o carregamento real.

## Histórico

- v8.8b: regra de link e teste de reset Allegrex.
- v8.9b: RAM PSP isolada com testes.
- v8.10b: RAM integrada ao ciclo de vida do núcleo.
- v8.11b: primeiro subset de execução de instruções Allegrex com teste automatizado.

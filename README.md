# StonePSP360

**Versão de desenvolvimento:** v8.19b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.19b

- Implementadas as instruções Allegrex/MIPS `CLZ` e `CLO` via opcode SPECIAL2.
- Implementadas `MOVZ` e `MOVN` com preservação do registrador destino quando a condição não é satisfeita.
- Adicionado teste dedicado para resultados, entradas zero, condições verdadeiras/falsas e registrador `$zero`.
- CI compila e executa o novo teste com `-Wall -Wextra -Werror`.

## Histórico recente

- v8.18b: teste dedicado de delay slots, branches, falha de memória e opcode inválido..

## Componentes e limitações

CPU Allegrex: subconjunto inteiro interpretado; ainda faltam muitas instruções Allegrex, FPU, VFPU, COP0/exceções completas e instruções branch-likely e várias instruções SPECIAL2.  
Memória: RAM simulada e aliases iniciais; VRAM, scratchpad e MMIO ainda não implementados.  
Loader: ELF32 MIPS direto e PBP com ELF simples no DATA.PSP; sem PRX criptografado/comprimido, relocations, imports/NIDs ou leitura de arquivos do console.  
Sistema: sem scheduler completo, HLE/kernel, input, áudio, GPU/GE ou execução de jogos comerciais comprovada.  
Plataforma: CI produz ELF32 PowerPC via libXenon; `xenon.elf` é cópia do ELF, não `default.xex`. Não há teste confirmado em console físico.

## Build na nuvem

Veja [GitHub Actions](https://github.com/MeiKinosaki/StonePSP360/actions).

## Histórico

- v8.8b: regra de link e teste de reset Allegrex.
- v8.9b: RAM PSP isolada com testes.
- v8.10b: RAM integrada ao ciclo de vida do núcleo.
- v8.11b: primeiro subconjunto de instruções Allegrex.
- v8.12b: operações HI/LO, multiplicação/divisão e branches REGIMM.
- v8.13b: loop de execução Allegrex limitado por orçamento.
- v8.14b: API pública do núcleo executa o interpretador Allegrex com orçamento limitado.
- v8.15b: inspeção de cabeçalhos PBP e ELF32 MIPS.
- v8.16b: validação e carga de segmentos PT_LOAD, com zeragem de BSS.
- v8.17b: API pública integra carregamento ELF e inicialização do PC da CPU.
- v8.18b: testes dedicados de delay slots, fluxo de branch e caminhos de erro.
- v8.19b: CLZ/CLO em SPECIAL2, MOVZ/MOVN e regressões dedicadas.

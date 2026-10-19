# StonePSP360

**Versão de desenvolvimento:** v8.25b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.25b

- Corrigida a validação de caminhos do VFS: o terminador NUL agora encerra corretamente cada segmento, em vez de ser rejeitado como caractere de controle.
- Acrescentados testes para arquivo vazio, além do round-trip da imagem VFS versionada iniciado na v8.24b.
- A falha do CI v8.23b foi localizada no teste de caminho válido e está sendo corrigida nesta versão; não foi mascarada nem removida do pipeline.

## Mudanças v8.24b

- VFS agora exporta/importa uma imagem binária versionada e little-endian, com validação de tamanho, caminhos, duplicatas e truncamento antes de substituir o estado em memória.
- Isso prepara a persistência: a aplicação chamadora ainda precisa gravar/carregar os bytes em um meio real. Não é ainda save persistente automático nem integração com o kernel PSP.

## Mudanças v8.23b

- Adicionada camada VFS em memória para arquivos com caminho relativo, limites de tamanho, leitura parcial, sobrescrita, consulta de existência/tamanho e remoção.
- Rejeita caminhos absolutos, separadores invertidos, segmentos `.`/`..`, caracteres de controle e caminhos com prefixo de unidade.
- Teste de regressão incluído no CI.
- **Limitação:** armazenamento somente em RAM do processo; não persiste após reinicialização e ainda não está conectado às chamadas de arquivo do kernel PSP nem ao disco/USB do Xbox 360.

## Mudanças v8.22b

- Criada a primeira fundação de serviços PSP: registro/dispatch de syscalls por NID, escalonador cooperativo de uma passagem, ticks virtuais, flags de IRQ, estado de controles, fila de comandos GE e fila de amostras de áudio.
- Adicionado snapshot versionado para estado dos serviços e teste automatizado de regressão.
- **Limitação:** isto não equivale a kernel PSP, GPU/GE renderizadora, áudio de hardware, save state completo, persistência de jogos ou execução de PRX. As filas são infraestrutura e ainda não estão ligadas a dispositivos reais.

## Mudanças v8.21b

- Adicionadas instruções Allegrex SPECIAL2 `MUL`, `MADD`, `MADDU`, `MSUB` e `MSUBU`, com operações de 64 bits em HI/LO.
- Adicionado teste de regressão para multiplicação e acumulação.

## Mudanças v8.19b

- Implementadas as instruções Allegrex/MIPS `CLZ` e `CLO` via opcode SPECIAL2.
- Implementadas `MOVZ` e `MOVN` com preservação do registrador destino quando a condição não é satisfeita.
- Adicionado teste dedicado para resultados, entradas zero, condições verdadeiras/falsas e registrador `$zero`.
- CI compila e executa o novo teste com `-Wall -Wextra -Werror`.

## Histórico recente

- v8.18b: teste dedicado de delay slots, branches, falha de memória e opcode inválido..

## Componentes e limitações

Serviços PSP v8.22b: fundação inicial de syscall registry, escalonador cooperativo simples, ticks, IRQ pendentes, input abstrato e filas GE/áudio; ainda sem ligação completa ao kernel, GPU e dispositivo de áudio. Snapshot cobre somente o estado do módulo de serviços, não CPU/RAM/estado integral do jogo.\nCPU Allegrex: subconjunto inteiro interpretado; ainda faltam muitas instruções Allegrex, FPU, VFPU, COP0/exceções completas e instruções branch-likely e várias instruções SPECIAL2.  
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
- v8.20b: expansão dos testes de ALU inteira e correção do teste SRA.
- v8.21b: SPECIAL2 MUL/MADD/MADDU/MSUB/MSUBU com regressões.\n- v8.25b: correção de validação de caminhos e teste de arquivo vazio.\n- v8.24b: exportação/importação binária versionada do VFS.\n- v8.23b: VFS em memória com validação de caminhos e testes de leitura/escrita/remoção.\n- v8.22b: primeira fundação de serviços PSP com testes de syscalls, scheduler cooperativo, timer, IRQ, input, filas GE/áudio e snapshot parcial.

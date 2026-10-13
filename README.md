# StonePSP360

**Versão de desenvolvimento:** v8.6b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças na v8.6b

- Criado um esqueleto inicial de estado arquitetural do Allegrex: GPRs, HI/LO, PC, registradores de ponto flutuante em representação de bits e alguns registradores COP0.
- Criada uma API explícita para estado do núcleo PSP e execução de frame, que retorna `NOT_IMPLEMENTED` até existir um interpretador real.
- O teste de plataforma agora mostra o estado do esqueleto PSP no ecrã.
- Makefile inclui `core/psp`; CI verifica ELF32, big-endian, PowerPC e SHA-256.

## Build na nuvem

Abra **Actions → StonePSP360 libXenon build** e execute a workflow. O artefato será `StonePSP360-v8.6b-elf32` se todas as verificações passarem.

## Limitações explícitas

Esta versão **não emula jogos PSP**. O estado Allegrex é apenas armazenamento de registradores; não há fetch/decode/execute, memória PSP, exceções completas, VFPU, temporização, GPU, áudio, carregamento de ISO/CSO ou integração do PPSSPP. A compilação v8.5b passou no GitHub Actions, mas v8.6b precisa de seu próprio resultado de CI e não foi testada em hardware.

## Próximas etapas

1. Confirmar o build v8.6b e corrigir erros de compilação.
2. Adicionar testes unitários portáveis para o reset do estado.
3. Projetar o mapa de memória PSP e a interface de leitura/escrita com testes, antes de interpretar instruções.

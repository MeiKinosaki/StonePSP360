# StonePSP360

**Versão de desenvolvimento:** v8.5b  
**Alvo:** Xbox 360 RGH/JTAG, com Free60/libXenon.

## O que mudou na v8.5b

- Mantido o teste de inicialização de vídeo, consola e USB.
- Mensagens no ecrã identificam o alvo ELF32 PowerPC e deixam claro que o núcleo PSP ainda não foi integrado.
- Workflow de CI reforçada com verificações do cabeçalho ELF: classe 32-bit, big-endian e arquitetura PowerPC, além do SHA-256.
- O artefato de build recebe o nome `StonePSP360-v8.5b-elf32`.

## Compilação em nuvem

A workflow **StonePSP360 libXenon build** é acionada por alterações em `source/**`, `Makefile` ou no próprio workflow, e também pode ser executada manualmente pela aba **Actions**. O job precisa terminar com sucesso e produzir o artefato `StonePSP360-v8.5b-elf32`.

## Estado real do projeto

O ELF32 de v8.4b foi compilado com sucesso no GitHub Actions e identificado como ELF 32-bit, big-endian, PowerPC. Isso valida a cadeia de compilação do teste de plataforma, não o funcionamento em hardware.

**Ainda não é um emulador PSP funcional.** A CPU Allegrex, o núcleo PPSSPP, GPU PSP, áudio, sistema de arquivos e compatibilidade de jogos não foram integrados. O novo build v8.5b precisa passar pelas verificações adicionais antes de ser marcado como validado.

## Próximos passos

1. Confirmar a validação ELF32 da v8.5b.
2. Planejar e testar o carregamento no ambiente Xbox 360/XeLL, sem assumir compatibilidade só porque o ELF compilou.
3. Investigar a integração do núcleo PSP de forma incremental e compatível com as licenças do código-base.

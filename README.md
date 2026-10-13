# StonePSP360

**Versão de desenvolvimento:** v8.10b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.10b

- Integrado o módulo de RAM PSP ao ciclo de vida do núcleo.
- Adicionados acessores de leitura/escrita de 8, 16 e 32 bits no núcleo.
- Acrescentado encerramento explícito da memória com `psp_core_shutdown()`.
- Ampliados os aliases testados para incluir 0xA8000000.
- Adicionado teste host para provar que o núcleo inicializa a RAM, acessa-a por aliases e continua reportando a execução de instruções como não implementada.
- CI compila e executa três testes host antes do build ELF32 PowerPC big-endian.

## Build na nuvem

Abra [GitHub Actions](https://github.com/MeiKinosaki/StonePSP360/actions) e baixe o artefato da execução verde mais recente. O workflow publica `StonePSP360.elf32` e `xenon.elf`; o segundo é uma cópia do ELF, **não** um `default.xex`.

## Limitações

A RAM está integrada ao núcleo, mas o núcleo ainda não faz fetch/decode/execute de instruções Allegrex. Não há ainda carregador ISO/CSO, GPU/GE, áudio, VFPU, sistema operacional PSP ou compatibilidade de jogos comprovada. O sucesso dos testes host/build não equivale a teste em hardware Xbox 360.

## Histórico

- v8.8b: regra de link corrigida; teste de reset Allegrex.
- v8.9b: módulo isolado de RAM PSP e testes.
- v8.10b: RAM integrada ao ciclo de vida do núcleo e acessores públicos de memória.

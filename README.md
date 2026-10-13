# StonePSP360

**Versão de desenvolvimento:** v8.9b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.9b

- Adicionado módulo isolado de RAM PSP de 32 MiB.
- Acesso de 8, 16 e 32 bits em little-endian com validação de limites.
- Mapeamento de aliases de RAM: 0x08000000, 0x48000000, 0x88000000 e 0xC8000000.
- Adicionado teste host para leituras/escritas, aliases, endianess e endereços inválidos.
- Workflow executa os testes host antes da compilação libXenon na nuvem.

## Executável em nuvem

Abra [GitHub Actions](https://github.com/MeiKinosaki/StonePSP360/actions) → **StonePSP360 Cloud Toolchain and Executable**. O workflow inicia após alterações em fontes, testes, Makefile, README ou workflow. Baixe o artefato apenas após uma execução verde.

O arquivo `xenon.elf` é uma cópia do ELF32 para teste de carregamento com XeLL; não é um `default.xex`. A execução real em console ainda precisa ser verificada.

## Limitações atuais

O módulo de RAM ainda **não está conectado** ao núcleo Allegrex. O núcleo não implementa fetch/decode/execute, GPU, áudio, VFPU, carregador ISO/CSO ou compatibilidade com jogos. Passar nos testes host e compilar um ELF não comprova execução no Xbox 360 nem compatibilidade de jogos.

## Histórico

- v8.8b: corrigida a regra de link dos objetos e adicionado teste do reset Allegrex.
- v8.9b: primeiro módulo isolado de memória RAM PSP com testes de unidade.

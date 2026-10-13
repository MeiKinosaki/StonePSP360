# StonePSP360

**Versão de desenvolvimento:** v8.8b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.8b

- Corrigida a regra de dependência $(OUTPUT).elf: $(OFILES), seguindo o template de exemplo do Free60, para que o linker receba os objetos compilados.
- Mantida a descoberta de fontes por caminhos absolutos a partir de PROJECTDIR.
- Adicionado teste unitário host para verificar se o reset zera os registradores Allegrex e define next_pc = 4.
- Workflow executa o teste host antes de usar a toolchain libXenon em Docker.
- Se a compilação e validações passarem, publica StonePSP360.elf32 e xenon.elf no artefato StonePSP360-v8.8b-xbox360-elf.

## Executável em nuvem

Abra Actions → StonePSP360 Cloud Toolchain and Executable. O workflow também inicia automaticamente após alterações em fontes, testes, Makefile ou no próprio workflow. Baixe o artefato apenas depois de uma execução verde.

O xenon.elf é uma cópia do ELF32 para teste de carregamento com XeLL; não é um default.xex. A execução real em console ainda precisa ser verificada.

## Limitações

O núcleo PSP continua em estágio inicial: há somente armazenamento de estado Allegrex e API de placeholder. Ainda não há fetch/decode/execute, mapa de memória PSP, GPU, áudio, VFPU, carregador ISO/CSO ou integração do PPSSPP. Passar nos testes e compilar ELF não comprova compatibilidade de jogos.

## Histórico
- v8.5b: build ELF32 anterior aprovado.
- v8.6b–v8.7b: builds falharam por ausência de objetos no link; a última execução confirmou que o alvo ELF era linkado sem main.
- v8.8b: adiciona dependência explícita dos objetos e teste unitário do reset. A aprovação depende da nova execução do CI.

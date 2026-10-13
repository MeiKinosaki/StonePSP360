# StonePSP360

**Versão de desenvolvimento:** v8.7b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## v8.7b — correção da toolchain na nuvem

A compilação v8.6b falhou porque a compilação recursiva executa dentro de `build/`, enquanto o Makefile procurava os arquivos-fonte com caminhos relativos. O linker acabou sem o objeto que contém `main`.

Esta versão corrige a descoberta dos fontes usando `PROJECTDIR` e caminhos absolutos. O workflow da nuvem então:

1. executa `make clean && make` dentro do contêiner `free60/libxenon:latest`;
2. verifica a existência e o cabeçalho de `StonePSP360.elf32` (ELF32, big-endian, PowerPC);
3. prepara também uma cópia chamada `xenon.elf`;
4. publica ambos no artefato `StonePSP360-v8.7b-xbox360-elf`.

## Como obter o executável

Abra **Actions → StonePSP360 Cloud Toolchain and Executable**, execute **Run workflow** na branch `main` ou aguarde o build automático. Quando o job ficar verde, baixe o artefato na página daquela execução.

O arquivo `xenon.elf` é uma cópia do ELF compilado para facilitar um teste de carregamento; não é um `default.xex`, e a compatibilidade do carregamento precisa ser verificada no XeLL e no console real.

## Estado da emulação

A v8.7b ainda é um teste de inicialização libXenon com um esqueleto inicial de estado Allegrex. **Não há ainda execução de instruções PSP nem compatibilidade de jogos.** O sucesso do build não substitui teste no Xbox 360.

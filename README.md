# StonePSP360

**Versão de desenvolvimento:** v8.4b  
**Alvo:** Xbox 360 com RGH/JTAG, usando a toolchain Free60/libXenon.

## Estado desta versão

Esta versão acrescenta um teste de inicialização da plataforma e uma workflow de build em nuvem. **Não é ainda um emulador PSP funcional.** O núcleo PSP/PPSSPP, CPU Allegrex, GPU, áudio, sistema de arquivos, entrada PSP e compatibilidade de jogos ainda não foram integrados.

## Compilação em nuvem

1. Abra a aba **Actions** deste repositório.
2. Selecione **StonePSP360 libXenon build**.
3. Clique em **Run workflow** e escolha a branch `main`.
4. Quando terminar, abra a execução e baixe o artefato `StonePSP360-v8.4b-elf32`.

A workflow depende da disponibilidade e configuração da imagem `free60/libxenon:latest`. O build só deve ser considerado aprovado se a execução terminar verde e o artefato ELF32 for gerado. A workflow ainda não foi validada por uma execução bem-sucedida neste commit.

## Compilação local

Com a toolchain Free60/libXenon instalada e `DEVKITXENON` configurada:

```sh
make clean
make
```

Saída esperada do template: `StonePSP360.elf32`.

## Teste de plataforma

O programa inicializa vídeo, consola e USB e mostra um aviso de que o núcleo PSP ainda não está integrado. Pressionar **Guide** ou **Y** encerra o teste. O comportamento real ainda precisa ser compilado e testado em hardware.

## Próximos passos

- Confirmar a execução da workflow e corrigir erros reais do compilador.
- Validar o formato de carregamento no XeLL e a cadeia de inicialização no Xbox 360.
- Só depois começar a integração incremental do núcleo PSP, sem afirmar compatibilidade de jogos antes dos testes.

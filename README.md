# StonePSP360

**Versão de desenvolvimento:** v8.17b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.17b

- Adicionada a API pública `psp_core_load_image`.
- A API aceita um buffer com ELF32 MIPS simples ou PBP cujo DATA.PSP contenha diretamente um ELF reconhecido.
- Usa o loader para validar e copiar segmentos para RAM, depois configura o PC da Allegrex com o entry point.
- Teste de integração constrói um ELF de teste, carrega-o pela API pública, executa quatro instruções e verifica o resultado de um store na RAM.
- Entradas inválidas retornam status de erro em vez de alegar carregamento bem-sucedido.

## Limitações

Ainda não há leitura de arquivos do dispositivo Xbox, PRX decriptado/descomprimido, relocations, imports/NIDs, firmware/serviços PSP, scheduler, GPU/GE ou áudio. O código de teste é um ELF sintético e não prova compatibilidade com jogos comerciais. Não há teste confirmado em console físico.

## Build na nuvem

Veja [GitHub Actions](https://github.com/MeiKinosaki/StonePSP360/actions). O artefato `xenon.elf` é uma cópia do ELF32 PowerPC, não um `default.xex`.

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

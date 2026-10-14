# StonePSP360

**Versão de desenvolvimento:** v8.15b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.15b

- Adicionado inspetor seguro de cabeçalho para PBP e ELF32 little-endian MIPS.
- Valida os oito offsets de seção PBP, ordem, limites e tamanho do cabeçalho.
- Valida identificador ELF, classe, endianness, tipo executável, máquina MIPS e tabela de program headers.
- Detecta quando DATA.PSP contém diretamente um ELF suportado e reporta entry point e tabela de segmentos.
- Testes host cobrem ELF válido, PBP vazio, PBP com payload ELF, offsets inválidos e arquivos truncados.

## Limitações

Isto é uma etapa de inspeção, não um carregador completo. Muitos EBOOT.PBP contêm PRX comprimido/criptografado em DATA.PSP; a ferramenta não descompacta/descriptografa PRX, não mapeia segmentos para RAM e não inicializa imports do PSP. Ainda faltam loader completo, exceções/COP0/VFPU, scheduler, GPU/GE, áudio, firmware/serviços e compatibilidade validada. Não há teste confirmado em console físico.

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
- v8.15b: inspeção validada de cabeçalhos PBP e ELF32 MIPS.

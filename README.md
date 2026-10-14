# StonePSP360

**Versão de desenvolvimento:** v8.16b  
**Alvo:** Xbox 360 RGH/JTAG com Free60/libXenon.

## Mudanças v8.16b

- O loader agora pode carregar segmentos ELF32 MIPS `PT_LOAD` de um ELF simples ou PBP cujo `DATA.PSP` contenha diretamente um ELF.
- Valida os intervalos do arquivo, limites da RAM, `p_filesz <= p_memsz`, alinhamento e se o entry point pertence a um segmento executável.
- Todos os segmentos são pré-validados antes de modificar a RAM.
- Copia os bytes de cada segmento e zera a região BSS (`p_memsz - p_filesz`).
- Testes host cobrem carga ELF/PBP, BSS e destino inválido sem alteração parcial.

## Limitações

O loader não descriptografa nem descomprime PRX, não resolve imports, relocations ou NIDs, e não carrega jogos comerciais de forma geral. O arquivo ainda não é lido do disco pela interface do Xbox; o loader recebe um buffer em memória. O entry point ainda precisa ser passado ao núcleo CPU pelo código chamador. VFPU, exceções/COP0, scheduler, GPU/GE, áudio e serviços PSP continuam incompletos. Não há teste confirmado em console físico.

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

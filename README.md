## Projeto 1 - Software Básico 2022/1 - UnB

### Autores:

João Pedro Assunção Coutinho - 18/0019813

William Coelho da Silva - 18/0029274

### Descrição:

**Projeto 1 - Software Básico** - Assembly Inventado visto em sala de aula.

### Instruções de compilação/execução:

Usamos Linux durante o desenvolvimento, para compilar, basta digitar **make**.

Para executar o montador, rodamos **make montar**.

É possível mudar os arquivos de entrada e saída, e o modo de execução do montador, editando as variáveis **ENTRADA_MONTADOR**, **SAIDA_MONTADOR**, e **MODO_MONTADOR** no Makefile, respectivamente.

Similarmente, para rodar o ligador, basta usar **make ligar**.

É posível mudar os arquivos de entrada 1 e 2 do ligador editando as variáveis **ENTRADA1_LIGADOR** e **ENTRADA2_LIGADOR**, no Makefile, respectivamente.

### Observações:

Adicionamos informações de realocação do tipo lista de endereços, nos arquivos objeto com begin e end.

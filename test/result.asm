MOD_A: BEGIN
MOD_B: EXTERN
PUBLIC FAT
PUBLIC N

SECAO TEXTO
INPUT N
LOAD N
FAT: SUB ONE
JMPZ FIM
JMP MOD_B
FIM: OUTPUT N
STOP

SECAO DADOS

N: SPACE
ONE: CONST 1

END

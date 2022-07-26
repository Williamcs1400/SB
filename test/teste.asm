L1: 
EQU 1 

L2: EQU 0

SECAO TEXTO
MOD_1:
INPUT N1
INPUT N2
LOAD N1  
IF
L1 
ADD N2 ;faz esta operação se L1 for verdadeiro 
IF
L2 
SUB N2 ;faz esta operação se L2 for verdadeiro 
STORE N3
OUTPUT N3
STOP

SECAO DADOS
N1: SPACE
N2: SPACE
N3: SPACE
N4: CONST L1
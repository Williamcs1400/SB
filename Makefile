ENTRADA_MONTADOR=test/fat_mod_B.asm
SAIDA_MONTADOR=test/result_B.obj
MODO_MONTADOR=-o

ENTRADA1_LIGADOR=test/result_A.obj
ENTRADA2_LIGADOR=test/result_B.obj

main:
	g++ -std=c++11 -Wall montador.cpp -o montador
	g++ -std=c++11 -Wall ligador.cpp -o ligador

montar:
	./montador $(MODO_MONTADOR) $(ENTRADA_MONTADOR) $(SAIDA_MONTADOR)

ligar:
	./ligador $(ENTRADA1_LIGADOR) $(ENTRADA2_LIGADOR)
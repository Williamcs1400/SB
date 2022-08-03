ENTRADA_MONTADOR=test/fat_mod_B.asm
SAIDA_MONTADOR=test/result_B.obj
MODO_MONTADOR=-o

ENTRADA_LIGADOR=a
SAIDA_LIGADOR=a

main:
	g++ -std=c++11 -Wall montador.cpp -o montador
	g++ -std=c++11 -Wall ligador.cpp -o ligador

run:
	./montador $(MODO_MONTADOR) $(ENTRADA_MONTADOR) $(SAIDA_MONTADOR)
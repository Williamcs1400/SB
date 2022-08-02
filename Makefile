ENTRADA=test/fat_mod_A.asm
SAIDA=test/result.asm
MODO=-p

main:
	g++ -std=c++11 -Wall montador.cpp -o montador

run:
	./montador $(MODO) $(ENTRADA) $(SAIDA)
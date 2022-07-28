ENTRADA=test/teste_p.asm
SAIDA=test/teste_obj.asm
MODO=-o

main:
	g++ -std=c++11 -Wall montador.cpp -o montador

run:
	./montador $(MODO) $(ENTRADA) $(SAIDA)
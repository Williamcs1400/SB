ENTRADA=test/teste.asm
SAIDA=test/saida.asm

main:
	g++ -std=c++11 -Wall montador.cpp -o montador

run:
	./montador -p $(ENTRADA) $(SAIDA)
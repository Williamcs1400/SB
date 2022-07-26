main:
	g++ -std=c++11 -Wall montador.cpp -o montador

run:
	./montador -p test/teste.asm test/saida.asm
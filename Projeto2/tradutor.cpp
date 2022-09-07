#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// recebe o caminho do arquivo e retorna um vetor com cada linha do arquivo por posicao
vector<string> ler_arquivo(string caminho){
    vector<string> lines;
    ifstream file(caminho);
    string line;
    while(getline(file, line)) lines.push_back(line);
    return lines;
}

int main(){
    vector<string> entrada = ler_arquivo("test/resultFinal.obj");

    cout << "Vetor de entrada: ";
    for(int i = 0; i < entrada.size(); i++){
        cout << entrada[i] << endl;
    }
    return 0;
}
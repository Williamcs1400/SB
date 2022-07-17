#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>


using namespace std;

// instrucao -> (opcode, tamanho)
map<string, pair<int, int>> INSTRUCOES;

// diretiva -> (tamanho)
map<string, int> DIRETIVAS;

// tabela de sinonimos EQUs
map<string, int> EQUS;

// inicializa as tabelas
void inicializar() {
    // tabela de instrucoes
    INSTRUCOES["ADD"] = make_pair(1, 2);
    INSTRUCOES["SUB"] = make_pair(2, 2);
    INSTRUCOES["MULT"] = make_pair(3, 2);
    INSTRUCOES["DIV"] = make_pair(4, 2);
    INSTRUCOES["JMP"] = make_pair(5, 2);
    INSTRUCOES["JMPN"] = make_pair(6, 2);
    INSTRUCOES["JMPP"] = make_pair(7, 2);
    INSTRUCOES["JMPZ"] = make_pair(8, 2);
    INSTRUCOES["COPY"] = make_pair(9, 3);
    INSTRUCOES["LOAD"] = make_pair(10, 2);
    INSTRUCOES["STORE"] = make_pair(11, 2);
    INSTRUCOES["INPUT"] = make_pair(12, 2);
    INSTRUCOES["OUTPUT"] = make_pair(13, 2);
    INSTRUCOES["STOP"] = make_pair(14, 1);

    //tabela de diretivas
    DIRETIVAS["SPACE"] = 1; 
    DIRETIVAS["CONST"] = 1; 
    DIRETIVAS["BEGIN"] = 0;
    DIRETIVAS["END"] = 0;
    DIRETIVAS["EXTERN"] = 0;
    DIRETIVAS["PUBLIC"] = 0;
    DIRETIVAS["EQU"] = 0;
    DIRETIVAS["IF"] = 0;

}

// recebe o caminho do arquivo e retorna um vetor com cada linha do arquivo por posicao
vector<string> lerArquivo(string caminho){
    vector<string> lines;
    ifstream file(caminho);
    string line;
    while(getline(file, line)){
        lines.push_back(line);
    }
    return lines;
}

// faz preprocessamento do arquivo -> diretivas EQU e IF e remove comentarios
vector<string> preProcessamento(vector<string> linhas){

    vector<string> resultado;

    // salva a label e valor na tabela de sinonimos
    for(string linha: linhas) {
        // passando pra UPPERCASE
        for (auto & c: linha) c = toupper(c);

        // a linha contem EQU
        if(linha.find("EQU") != string::npos) {
            // remover espacos
            linha.erase(remove(linha.begin(), linha.end(), ' '), linha.end());

            string label = linha.substr(0, linha.find("EQU") - 1);
            string valor = linha.substr(linha.find("EQU") + 3);
            
            EQUS[label] = stoi(valor);
        }
    }
    
    for(int i = 0; i < linhas.size(); i++) {
        // passando pra UPPERCASE
        string linha = linhas[i];
        for (auto & c: linha) c = toupper(c);
        // a linha contem IF
        if(linha.find("IF") != string::npos) {
            // remover espacos
            linha.erase(remove(linha.begin(), linha.end(), ' '), linha.end());

            string label = linha.substr(linha.find("IF") + 2);
            auto it = remove_if(label.begin(), label.end(), [](char const &c) {
                return !isalnum(c);
            });

            label.erase(it, label.end());
            // se o if for "true" pula da linha do if
            if(EQUS[label] == 1) {
                linha = "";
            }

            // se o if for "false" pula da linha do if e a subsequente
            if(EQUS[label] == 0){
                linha = "";
                linhas[i+1] = "";
            }
        }

        if(linha.find(";") != string::npos) {
            linha = linha.substr(0, linha.find(";"));
        }
        resultado.push_back(linha);
    }
    return resultado;
}

bool verificarSessoes(vector<string> linhas){

    if(!(find(linhas.begin(), linhas.end(), "SECAO TEXTO") == linhas.end())){
        cout << "Erro: nao contem SESSAO TEXTO" << endl;
        return false;
    }

    if(!(find(linhas.begin(), linhas.end(), "SECAO DADOS") == linhas.end())){
        cout << "Erro: nao contem SESSAO DADOS" << endl;
        return false;
    }

    return true;
}

int main(int argc, char *argv[]){
    
    // iniciar maps 
    inicializar();

    // Le arquivo de instrucoes linha por linha
    vector<string> entrada = lerArquivo("test/teste.asm");

    // pre processamento
    if(string(argv[1]) == "-p") {
        vector<string> entrada_processada = preProcessamento(entrada);
        ofstream output_file("preprocessado.txt");
        for(const auto & line: entrada_processada) {
            output_file << line << endl;
        }

        // cout << "Pre processamento: " << endl;
        // for(string linha: entrada_processada) {
        //     cout << linha << endl;
        // }
    }


    return 0;
}
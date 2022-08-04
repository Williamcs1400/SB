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

// Para arquivo1
vector<pair<string, int>> TABELA_DE_USO_1;
vector<pair<string, int>> TABELA_DE_DEFINICAO_1;
vector<int> RELOCACAO_1;
vector<int> COD_OBJ_1;
const int FATOR_1 = 0;

// Para arquivo2
vector<pair<string, int>> TABELA_DE_USO_2;
vector<pair<string, int>> TABELA_DE_DEFINICAO_2;
vector<int> RELOCACAO_2;
vector<int> COD_OBJ_2;
int FATOR_2 = 0;

vector<string> ler_arquivo(string caminho){
    vector<string> lines;
    ifstream file(caminho);
    string line;
    while(getline(file, line)) lines.push_back(line);
    return lines;
}

vector<string> split_string(string str) {
    vector<string> resultado;
    
    string token;
    stringstream ss(str);

    while(ss >> token) resultado.push_back(token);
    return resultado;
}

void montar_tabelas(vector<string> linhas, int tabelaUsada){

    vector<pair<string, int>> *tabelaDeUso = tabelaUsada == 1 ? &TABELA_DE_USO_1 : &TABELA_DE_USO_2;
    vector<pair<string, int>> *tabelaDeDefinicao = tabelaUsada == 1 ? &TABELA_DE_DEFINICAO_1 : &TABELA_DE_DEFINICAO_2;
    vector<int> *relocacao = tabelaUsada == 1? &RELOCACAO_1 : &RELOCACAO_2;
    vector<int> *codObj = tabelaUsada == 1 ? &COD_OBJ_1 : &COD_OBJ_2;
    bool estamos_na_tabela_de_uso = false;
    bool estamos_na_tabela_de_definicao = false;
    bool estamos_na_relocacao = false;

    string obj = "";

    for(string linha : linhas){

        // Se já saímos da tabela de uso e definição, não fazemos nada

        if(linha.size() != 1){
            obj = linha;
        }

        if(linha.size() == 1){
            estamos_na_tabela_de_uso = false;
            estamos_na_tabela_de_definicao = false;
        }
        
        // Se estamos na tabela de uso
        else if(linha.find("TABELA USO") != string::npos){
            estamos_na_tabela_de_uso = true;
            estamos_na_tabela_de_definicao = false;
            estamos_na_relocacao = false;
        }

        // Se estamos na tabela de definição
        else if(linha.find("TABELA DEF") != string::npos){
            estamos_na_tabela_de_uso = false;
            estamos_na_tabela_de_definicao = true;
            estamos_na_relocacao = false;
        }

        else if(linha.find("RELOCACAO") != string::npos) {
            estamos_na_tabela_de_definicao = false;
            estamos_na_tabela_de_uso = false;
            estamos_na_relocacao = true;
        }

        else if(estamos_na_tabela_de_uso){
            vector<string> temp = split_string(linha);
            if(temp.size() == 2){
                (*tabelaDeUso).push_back(make_pair(temp[0], stoi(temp[1])));
            }
        }

        else if(estamos_na_tabela_de_definicao){
            vector<string> temp = split_string(linha);
            if(temp.size() == 2){
                (*tabelaDeDefinicao).push_back(make_pair(temp[0], stoi(temp[1])));
            }
        }  

        else if(estamos_na_relocacao) {
            vector<string> temp = split_string(linha);
            if(temp.size()) {
                for(auto a: temp) {
                    (*relocacao).push_back(stoi(a));
                }
            }
            estamos_na_relocacao = false;
        }

    }

    //transformar a obj em um vector de inteiros
    obj.pop_back(); //remover o espaço final
    stringstream ss(obj);
    string token;
    while(getline(ss, token, ' ')){
        int valor = stoi(token);
        (*codObj).push_back(valor);
    }
}

// busca e substitui o valor os valores ta tabela de definição e da tabela de uso
void substituicao(){

    // substituir valor do código objeto com base na tabela de uso
    for(unsigned int i = 0; i < TABELA_DE_DEFINICAO_1.size(); i++){
        string key = TABELA_DE_DEFINICAO_1[i].first;
        int valor = TABELA_DE_DEFINICAO_1[i].second;
        //pegar elemento da tabela de uso 2 que tem a key igual a key
        for(unsigned int j = 0; j < TABELA_DE_USO_2.size(); j++){
            if(TABELA_DE_USO_2[j].first == key){
                int posicao = TABELA_DE_USO_2[j].second;
                COD_OBJ_2[posicao] = valor;
            }
        }
    }

    for(unsigned int i = 0; i < TABELA_DE_DEFINICAO_2.size(); i++){
        string key = TABELA_DE_DEFINICAO_2[i].first;
        int valor = TABELA_DE_DEFINICAO_2[i].second;
        //pegar elemento da tabela de uso 2 que tem a key igual a key
        for(unsigned int j = 0; j < TABELA_DE_USO_1.size(); j++){
            if(TABELA_DE_USO_1[j].first == key){
                int posicao = TABELA_DE_USO_1[j].second;
                COD_OBJ_1[posicao] = valor;
            }
        }
    }
}

int main(int argc, char *argv[]){
    
    if(argc < 2){
        cout << "Usage: ./ligador <arquivo1.o> <arquivo2.o> ou ./ligador <arquivoUnico.o>" << endl;
        return 0;
    }

    // se for um arquivo unico, nao precisamos de tabelas de uso e definição
    if(argc == 2){
        vector<string> arquivo_unico = ler_arquivo(argv[1]);
        montar_tabelas(arquivo_unico, 1);
        ofstream saida;
        saida.open("test/resultFinal.obj");
        for(int i : COD_OBJ_1){
            saida << i << " ";
        } 
        saida.close();
        return 0;
    }

    //le separadamente cada arquivo
    vector<string> arquivo1 = ler_arquivo(argv[1]);
    vector<string> arquivo2 = ler_arquivo(argv[2]);

    if(arquivo1.size() == 0 || arquivo2.size() == 0){
        cout << "Arquivo(s) vazio(s) ou não encontrado(s)" << endl;
        return 0;
    }

    montar_tabelas(arquivo1, 1);
    montar_tabelas(arquivo2, 2);

    FATOR_2 = COD_OBJ_1.size();

    // corrigindo a tabela de uso do segundo arquivo

    for(unsigned int i=0; i<TABELA_DE_DEFINICAO_2.size(); i++){
        TABELA_DE_DEFINICAO_2[i].second += FATOR_2;
    }

    // como os arquivos sao tratados separadamente, nao precisamos
    // corrigir a tabela de uso do segundo arquivo 

    // corrigindo os enderecos relativos
    for(int i: RELOCACAO_2) {
        COD_OBJ_2[i] += FATOR_2;
    }

    substituicao();

    //salvar codigo objeto 1 e 2 concatenado em um arquivo
    ofstream saida;
    saida.open("test/resultFinal.obj");
    for(int i : COD_OBJ_1){
        saida << i << " ";
    } 
    for(int i : COD_OBJ_2){
        saida << i << " ";
    }
    saida.close();

    return 0;
}
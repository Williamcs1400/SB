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
vector<int> COD_OBJ_1;

// Para arquivo2
vector<pair<string, int>> TABELA_DE_USO_2;
vector<pair<string, int>> TABELA_DE_DEFINICAO_2;
vector<int> COD_OBJ_2;

vector<string> ler_arquivo(string caminho){
    vector<string> lines;
    ifstream file(caminho);
    string line;
    while(getline(file, line)) lines.push_back(line);
    return lines;
}

void montar_tabelas(vector<string> linhas, int tabelaUsada){

    vector<pair<string, int>> *tabelaDeUso = tabelaUsada == 1 ? &TABELA_DE_USO_1 : &TABELA_DE_USO_2;
    vector<pair<string, int>> *tabelaDeDefinicao = tabelaUsada == 1 ? &TABELA_DE_DEFINICAO_1 : &TABELA_DE_DEFINICAO_2;
    vector<int> *codObj = tabelaUsada == 1 ? &COD_OBJ_1 : &COD_OBJ_2;
    bool estamos_na_tabela_de_uso = false;
    bool estamos_na_tabela_de_definicao = false;

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
        }

        // Se estamos na tabela de definição
        else if(linha.find("TABELA DEF") != string::npos){
            estamos_na_tabela_de_uso = false;
            estamos_na_tabela_de_definicao = true;
        }

        else if(estamos_na_tabela_de_uso){
            string key = linha.substr(0, linha.find(" "));
            string value = linha.substr(linha.find(" ") + 1);
            (*tabelaDeUso).push_back(make_pair(key, stoi(value)));
        }

        else if(estamos_na_tabela_de_definicao){
            string key = linha.substr(0, linha.find(" "));
            string value = linha.substr(linha.find(" ") + 1);
            // put <key, value> in the table
            (*tabelaDeDefinicao).push_back(make_pair(key, stoi(value)));
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
    // buscar definição e substituir valor que está no código objeto no par
    for(int i = 0; i < TABELA_DE_DEFINICAO_1.size(); i++){
        int posicaoDefinicao = TABELA_DE_DEFINICAO_1[i].second;
        int valor = COD_OBJ_1[posicaoDefinicao];
        TABELA_DE_DEFINICAO_1[i].second = valor;
    }

    for(int i = 0; i < TABELA_DE_DEFINICAO_2.size(); i++){
        int posicaoDefinicao = TABELA_DE_DEFINICAO_2[i].second;
        int valor = COD_OBJ_2[posicaoDefinicao];
        TABELA_DE_DEFINICAO_2[i].second = valor;
    }

    // substituir valor do código objeto com base na tabela de uso
    for(int i = 0; i < TABELA_DE_DEFINICAO_1.size(); i++){
        string key = TABELA_DE_DEFINICAO_1[i].first;
        int valor = TABELA_DE_DEFINICAO_1[i].second;
        //pegar elemento da tabela de uso 2 que tem a key igual a key
        for(int j = 0; j < TABELA_DE_USO_2.size(); j++){
            if(TABELA_DE_USO_2[j].first == key){
                int posicao = TABELA_DE_USO_2[j].second;
                COD_OBJ_1[posicao] = valor;
            }
        }
    }

    for(int i = 0; i < TABELA_DE_DEFINICAO_2.size(); i++){
        string key = TABELA_DE_DEFINICAO_2[i].first;
        int valor = TABELA_DE_DEFINICAO_2[i].second;
        //pegar elemento da tabela de uso 2 que tem a key igual a key
        for(int j = 0; j < TABELA_DE_USO_1.size(); j++){
            if(TABELA_DE_USO_1[j].first == key){
                int posicao = TABELA_DE_USO_1[j].second;
                COD_OBJ_2[posicao] = valor;
            }
        }
    }
}

int main(int argc, char *argv[]){
    
    if(argc != 3){
        cout << "Usage: ./ligador <arquivo1.o> <arquivo2.o>" << endl;
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

    cout << "Codigo objeto 1 antes de substituir: " << endl;
    for(int i : COD_OBJ_1){
        cout << i << " ";
    }
    cout << endl;
    cout << "Codigo objeto 2 antes de substituir: " << endl;
    for(int i : COD_OBJ_2){
        cout << i << " ";
    }
    cout << endl << endl;

    substituicao();

    cout << "Codigo objeto 1 depois de substituir: " << endl;
    for(int i : COD_OBJ_1){
        cout << i << " ";
    }
    cout << endl;
    cout << "Codigo objeto 2 depois de substituir: " << endl;
    for(int i : COD_OBJ_2){
        cout << i << " ";
    }
    cout << endl << endl;

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
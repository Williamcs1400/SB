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
map<string, pair<int, int> > INSTRUCOES;

// diretiva -> (tamanho)
map<string, int> DIRETIVAS;

// tabela de sinonimos EQUs
map<string, int> EQUS;

map<string, int> TABELA_SIMBOLOS;

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

    // tabela de diretivas
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

// faz preprocessamento do arquivo -> diretivas EQU e IF e remove comentarios
vector<string> pre_processamento(vector<string> linhas){
    vector<string> resultado;

    string ultima_label = "";
    
    // foi encontrado um equ que ainda espera por argumentos
    bool equ_encontrado = false;

    // foi encontrado um if que ainda espera por argumentos
    bool if_encontrado = false;

    // ignorar proxima linha
    bool ignorar_prox = false;
    // ignorar linha atual
    bool ignorar_atual = false;
    
    bool secao_texto_encontrado = false;

    for(string linha: linhas) {
        
        // ha um comentario, entao removemos da string
        if(linha.find(";") != string::npos) {
            linha = linha.substr(0, linha.find(";"));
        }

        // se devemos ignorar, pulamos esssa linha
        if(ignorar_prox) {
            ultima_label = "";
            equ_encontrado = false;
            if_encontrado = false;
            ignorar_prox = false;
            ignorar_atual = false;
            continue;
        }

        // passando pra UPPERCASE
        transform(linha.begin(), linha.end(), linha.begin(), ::toupper);

        // tokenizando a linha
        vector<string> tokens = split_string(linha);
        
        if(linha.find("SECAO TEXTO") != string::npos) secao_texto_encontrado = true;

        for (string token: tokens) {
            // encontramos uma label, e salvamos retirando o :
            if(token.find(":") != string::npos) ultima_label = token.substr(0, token.size() - 1);
            
            else if(token == "EQU") {
                //se encontramos um equ apos uma label, essa declaracao eh valida
                if(ultima_label != "") equ_encontrado = true;
                else ultima_label = "";
            }
            else if(equ_encontrado) {
                EQUS[ultima_label] = stoi(token);
                equ_encontrado = false;
                ultima_label = "";
            }
            
            else if(token == "IF") {
                if_encontrado = true;
                ignorar_atual = true;
            } 
            //o token anterior eh um if
            else if (if_encontrado) {
                // verificamos se ha um EQU anterior, com a label usada pelo if
                if(EQUS.count(token)) {
                    if_encontrado = false;
                    // o token if e o token posterior devem ser removidos do codigo final (if l1)
                    if(EQUS[token] <= 0) {
                        ignorar_prox = true;
                    }
                }
                else {
                    cout << linha << endl;
                    cout << "Erro semântico - IF usado sem EQU anterior" << endl;
                }
            }
           
           // se o token nao eh uma label, nem equ nem if, resetamos as variaveis de estado
            else {
                ultima_label = "";
                equ_encontrado = false;
                if_encontrado = false;
                ignorar_atual = false;
                if(EQUS.count(token)) linha.replace(linha.find(token), token.size(), to_string(EQUS[token]));
            }

        }
        // antes da secao texto,  nao precisamos salvar as linhas

        if(secao_texto_encontrado && !ignorar_atual) {
            ignorar_atual = false;
            resultado.push_back(linha);
        }
        
    }
    return resultado;
}

/*

bool verificar_sessoes(vector<string> linhas){

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

*/

int posicao;
void primeira_passagem (vector<string> linhas) {
    posicao = 0;
    int tokens_a_pular = 0;

    for(string linha: linhas) {

        for(string token: split_string(linha)) {
            
            if(tokens_a_pular) {
                tokens_a_pular--;
                continue;
            }

            // encontramos uma label
            if(token.find(":") != string::npos) {
                string label = token.substr(0, token.size()-1);

                // label já existe
                if(TABELA_SIMBOLOS.count(label)) {
                    cout << "Erro semântico - Label redefinida" << endl;
                }
                else {
                    TABELA_SIMBOLOS[label] = posicao;
                    cout << label << " " << posicao << endl;
                }
            }

            // encontramos uma instrucao
            else if(INSTRUCOES.count(token)) {
                posicao += INSTRUCOES[token].second;
                tokens_a_pular = INSTRUCOES[token].second - 1;
            }

            // encontramos uma diretiva
            else if(DIRETIVAS.count(token)) {
                // se for space ou const, a posicao é
                if(token == "SPACE" || token == "CONST") {
                    posicao += DIRETIVAS[token];
                    
                    // se for CONST precisamos pular o proximo token (argumento)
                    if(token == "CONST") {
                        tokens_a_pular = 1;
                    }
                }
            }

            // se não é diretiva nem instrução
            else {
                cout << "Erro semântico - Label/Operação não identificada" << endl;
            }
        }
    }
}


// retorna true se str contem parte
bool contains (string str, string parte) {
    if(str.find(parte) != string::npos) return true;
    return false;
}

vector<string> segunda_passagem (vector<string> linhas) {
    vector<string> resultado;
    posicao = 0;

    bool secao_texto_encontrada = false;
    bool secao_dados_encontrada = false;

    int argumentos_esperados = 0;

    for(string linha: linhas) {

        if(contains(linha, "SECAO TEXTO")) secao_texto_encontrada = true;
        if(contains(linha, "SECAO DADOS")) secao_dados_encontrada = true;

        for(string token: split_string(linha)) {

            // se for uma label, nao fazemos nada
            if(contains(token, ":")) continue;

            // se for uma instrucao
            else if(INSTRUCOES.count(token)) {

            }
            
            // se for uma diretiva
            else if(DIRETIVAS.count(token)) {
                
            }
        }
    }
    return resultado;
}

int main(int argc, char *argv[]){
    if(argc < 4) {
        cout << "Erro: Faltam argumentos" << endl;
        return 0;
    }

    else {
        string modo_execucao(argv[1]), arquivo_entrada(argv[2]), arquivo_saida(argv[3]);

        inicializar();
        vector<string> entrada = ler_arquivo(arquivo_entrada);

        // pre processamento
        if(modo_execucao == "-p") {
            vector<string> entrada_processada = pre_processamento(entrada);

            ofstream arquivo;
            arquivo.open(arquivo_saida);
            for(auto linha: entrada_processada) arquivo << linha << endl;
            
            arquivo.close();
        }

        else if (modo_execucao == "-o") {

            primeira_passagem(entrada);
            vector<string> codigo_objeto = segunda_passagem(entrada);
            cout << "FIM" << endl;
        }

    }
    
    return 0;
}
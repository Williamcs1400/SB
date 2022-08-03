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

map<string, int> TABELA_DEFINICOES;

vector<string> TABELA_USO;

vector<int> RELOCACAO;

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

// duvide str de acordo com os espacos e retorna um vector
vector<string> split_string(string str) {
    vector<string> resultado;
    
    string token;
    stringstream ss(str);

    while(ss >> token) resultado.push_back(token);
    return resultado;
}

// retorna true se str contem parte
bool contains (string str, string parte) {
    if(str.find(parte) != string::npos) return true;
    return false;
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

    string temp = "";

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
            if(contains(token, ":") && !secao_texto_encontrado) {
                ignorar_atual = true;
                temp = linha;
                ultima_label = token.substr(0, token.size() - 1);
            }
            
            else if (ultima_label.size() && !equ_encontrado) {
                if(token == "EQU") {
                    equ_encontrado = true;
                    ignorar_atual = true;
                }
                else {
                    if(contains(linha, ultima_label)) ignorar_atual = true;
                    resultado.push_back(temp);
                    ultima_label = "";
                    temp = "";
                }
            }
            else if(equ_encontrado) {
                EQUS[ultima_label] = stoi(token);
                equ_encontrado = false;
                ignorar_atual = true;
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

        if(!ignorar_atual) {
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
bool contem_begin = false;
bool contem_end = false;

void primeira_passagem (vector<string> linhas) {
    posicao = 0;
    int tokens_a_pular = 0;

    //variaveris para verificacao de erros
    bool contem_public_or_extern = false;

    string token_anterior = "";

    for(string linha: linhas) {

        if(contains(linha, "SECAO")) {
            continue;
        }

        //se contem BEGIN, deve conter END
        if(contains(linha, "BEGIN")) {
            contem_begin = true;
        }

        if(contains(linha, "END")) {
            contem_end = true;
        }

        if(contains(linha, "EXTERN") || contains(linha, "PUBLIC")) {
            contem_public_or_extern = true;
        }

        for(string token: split_string(linha)) {

            if(token_anterior == "PUBLIC") {
                token_anterior = "";
                // colocamos o valor -1, ja que vamos atualizar depois
                TABELA_DEFINICOES[token] = -1;
                continue;
            }

            if(tokens_a_pular) {
                token_anterior = "";
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
                }
            }

            // encontramos uma instrucao
            else if(INSTRUCOES.count(token)) {
                posicao += INSTRUCOES[token].second;
                tokens_a_pular = INSTRUCOES[token].second - 1;
            }

            // encontramos uma diretiva
            else if(DIRETIVAS.count(token)) {
                // se for space ou const, adicionamos 1 a posicao
                if(token == "SPACE" || token == "CONST") {
                    posicao += DIRETIVAS[token];
                    
                    // se for CONST precisamos pular o proximo token (argumento)
                    if(token == "CONST") {
                        tokens_a_pular = 1;
                    }
                }
                else if(token == "EXTERN") {
                    // inserimos um zero absoulto (aqui representado por -1) como endereco da ultima label
                    if(contains(token_anterior, ":")) {
                        TABELA_SIMBOLOS[token_anterior.substr(0, token_anterior.size()-1)] = -1;
                    }
                    else {
                        cout << "Erro!" << endl;
                    }
                }
            }

            // se não é diretiva nem instrução
            else {

                //verificar se há um problema no número de argumentos
                if((INSTRUCOES.count(token_anterior) && !INSTRUCOES.count(token)) || (DIRETIVAS.count(token_anterior) && !DIRETIVAS.count(token))) {
                    cout << "Número de operandos inválidos do token: " << token_anterior << endl;
                }else{
                    cout << token << endl;
                    cout << "Erro semântico - Label/Operação não identificada" << endl;
                }
            }

            token_anterior = token;

        }
    }

    // verficação de erros
    if((contem_begin && !contem_end) || (!contem_begin && contem_end)) {
        cout << "Erro semântico - Falta BEGIN ou Falta END" << endl;
    }

    if(contem_public_or_extern && (!contem_begin || !contem_end)) {
        cout << "Erro semântico - PUBLIC ou EXTERN definido mas falta a definição de BEGIN e END" << endl;
    }

    // criando a tabela de definições
    for(auto it = TABELA_DEFINICOES.begin(); it != TABELA_DEFINICOES.end(); it++) {
        string label = it->first;

        if(TABELA_SIMBOLOS.count(label)) {
            TABELA_DEFINICOES[label] = TABELA_SIMBOLOS[label];
        }
        else {
            cout << "Erro!" << endl;
        }
    }
}

vector<string> segunda_passagem (vector<string> linhas) {
    vector<string> resultado;
    posicao = 0;

    bool esta_na_secao_texto = false;
    bool esta_na_secao_dados = false;

    int argumentos_esperados = 0;

    // o ulimo token foi uma diretiva const
    bool ultimo_const = false;

    // o ultimo token foi um copy (usamos para remover a virgula do argumento)
    bool ultimo_copy = false;

    for(string linha: linhas) {

        //verificar em qual secao estamos para poder identificar erros
        if(contains(linha, "SECAO TEXTO")){
            esta_na_secao_texto = true;
            esta_na_secao_dados = false;
        }
        if(contains(linha, "SECAO DADOS")){ 
            esta_na_secao_dados = true;
            esta_na_secao_texto = false;
        }

        for(string token: split_string(linha)) {

            // se for uma label, nao fazemos nada
            if(contains(token, ":")) continue;

            // se for o argumento de uma instrução/diretiva
            if(argumentos_esperados) {
                
                if(ultimo_const) {
                    resultado.push_back(token);
                    ultimo_const = false;
                }

                else {
                    // se o ultimo foi um copy, precisamos remover a virgula do argumento atual
                    if(ultimo_copy) {
                        ultimo_copy = false;
                        token = token.substr(0, token.size() - 1);
                    }

                    // o argumento é um símobolo conhecido
                    if(TABELA_SIMBOLOS.count(token)) {

                        RELOCACAO.push_back(posicao-argumentos_esperados);

                        // é um símbolo externo
                        if(TABELA_SIMBOLOS[token] == -1) {
                            resultado.push_back("0");
                            stringstream ss;
                            ss << token << " " << posicao-argumentos_esperados;
                            TABELA_USO.push_back(ss.str());
                        }
                        else resultado.push_back(to_string(TABELA_SIMBOLOS[token]));
                    }

                    // o argumento é desconhecido, então erro
                    else cout << "Erro" << endl;
                }
                // argumentos são labels ou números
                argumentos_esperados--;
            }

            // se for uma instrucao
            else if(INSTRUCOES.count(token)) {

                //verificar se estamos na secao certa
                if(!esta_na_secao_texto) {
                    cout << "Erro semântico - Instrução " << token << " fora da secao texto" << endl;
                    exit(1);
                }

                if(token == "COPY") {
                    ultimo_copy = true;
                }

                posicao+=INSTRUCOES[token].second;
                argumentos_esperados = INSTRUCOES[token].second - 1;
                resultado.push_back(to_string(INSTRUCOES[token].first));
            }
            
            // se for uma diretiva
            else if(DIRETIVAS.count(token)) {
                posicao+=DIRETIVAS[token];

                if(token == "CONST") {
                    // verifica se estamos na secao certa
                    if(!esta_na_secao_dados){
                        cout << "Erro semântico - Diretiva " << token << " fora da secao de dados" << endl;
                        exit(1);
                    }

                    ultimo_const = true;
                    argumentos_esperados = 1;
                }
                else if(token == "SPACE") {
                    // verifica se estamos na secao certa
                    if(!esta_na_secao_dados){
                        cout << "Erro semântico - Diretiva " << token << " fora da secao de dados" << endl;
                        exit(1);
                    }

                    resultado.push_back("0");
                } 
            }

        }
    }
    return resultado;

    for(auto a:TABELA_USO) cout << a << endl;
}

int main(int argc, char *argv[]){
    if(argc < 4) {
        cout << "Erro: Faltam argumentos deve ser da seguinte maneira: ./montador <modo de execução> <arquivo de entrada> <arquivo de saída>" << endl;
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

            ofstream arquivo;
            arquivo.open(arquivo_saida);

            if(contem_begin && contem_end) {
                
                arquivo << "TABELA USO" << endl;
                for(auto a: TABELA_USO) arquivo << a << endl;
                arquivo << endl;

                arquivo << "TABELA DEF" << endl;
                for(auto it = TABELA_DEFINICOES.begin(); it != TABELA_DEFINICOES.end(); it++) arquivo << it->first << " " << it->second << endl;
                arquivo << endl;

                arquivo << "RELOCACAO" << endl;
                for(auto a: RELOCACAO) arquivo << a << " ";
                arquivo << endl << endl;
                
            }


            for(auto a: codigo_objeto) arquivo << a << " ";
            arquivo << endl;

            arquivo.close();
        }

    }
    
    return 0;
}
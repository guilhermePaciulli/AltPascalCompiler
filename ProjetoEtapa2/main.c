//
//  main.c
//  ProjetoEtapa2
//
// Guilherme Horcaio Paciulli - 41606574
// Matheus Tuma - 31629326
// Ane Caroline Gomes - 41627571
// Ygor Lima - 41607589
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Valores de controle
#define IGNORE -100 // tokens ignorados: quebra de linha
#define SEMANTIC_ERROR_TYPE -6
#define SEMANTIC_ERROR_TOKEN_DOES_NOT_EXIST -5
#define SEMANTIC_ERROR_TOKEN_EXISTS -4
#define SEMANTIC_ERROR -3
#define SYNTAX_ERROR -2
#define END_OF_STRING -1
#define LEXICAL_ERROR 0
#define END_OF_TOKEN 1
//Palvaras reservadas
#define PROGRAM 2
#define BEGIN 3
#define END 4
#define PROCEDURE 5
#define IF 6
#define THEN 7
#define ELSE 8
#define WHILE 9
#define DO 10
#define AND 11
#define OR 12
#define NOT 13
#define VAR 14
#define _INT 15
#define BOOL 16
#define _TRUE 17
#define _FALSE 18
#define WRITE 19
// Identificadores
#define IDENTIFIER 20
// Operadores
#define PLUS 21
#define MINUS 22
#define DIV 23
#define TIMES 24
#define LESS_THAN 25
#define GREATER_THAN 26
#define EQUALS 27
#define LESS_THAN_OR_EQUAL 28
#define GREATER_THAN_OR_EQUAL 29
#define RECEIVES 30   // :=
#define EQUIVALENT 31 // <>
// Delimitadores
#define OPEN_BRACKETS 32
#define CLOSE_BRACKTES 33
#define COMMA 34
#define COLON 35
#define DOT 36
#define SEMICOLON 37
// Comentários
#define COMMENT 38
//Números
#define NUMBER 39

typedef struct simbolo { // A tabela de símbolos é uma lista ligada
    int indice; // com o índice para criar um identificador único para cada símbolo encontrado no código (e achá-lo posteriormente)
    int categoria; // variável ou procedimento
    int tipo; // tipo INT ou BOOL para variáveis
    int escopo; // escopo encontrado
    char *palavra; // identificador 
    struct simbolo *prox; // próximo da lista 
} Simbolo;

Simbolo *tabela; // tabela de símbolos
int escopo = 1; // escopo atual
int hasSemanticError = 0; // flag de erro semântico
int erroSemantico; // erro semântico encontrado

void imprimeTabelaSimbolos() { // imprime a tabela de símbolos
    Simbolo *aux;
    
    printf("Tabela de símbolos: \n");
    aux = tabela;
    while (aux != NULL) {
        char* categoria;
        char* tipo;
        
        if (aux->categoria == VAR) { // formatações
            categoria = "VAR";
        } else {
            categoria = "PROCEDURE";
        }
        
        if (aux->tipo == _INT) { // formatações
            tipo = "INT";
        } else if (aux->tipo == BOOL) {
            tipo = "BOOL";
        } else {
            tipo = "-";
        }
        
        printf("Símbolo | %s | categoria %s | tipo %s | escopo %d \n", aux->palavra, categoria, tipo, aux->escopo);
        aux = aux->prox;
    }
}

int existeSimbolo(char* pal, int categoria, int escopo) { // procura o símbolo na tabela de símbolos
    Simbolo *aux;

    aux = tabela;
    while (aux != NULL) {
        if (strcmp(aux->palavra, pal) == 0 && aux->categoria == categoria && aux->escopo == escopo) { // ele deve ter o mesmo nome, categoria e escopo
            return 1;
        }
        aux = aux->prox;
    }
    return 0;
}

int insereSimbolo(char* pal, int categoria) { // insere o símbolo na tabela 
    Simbolo *aux;
    Simbolo *s1;
    int indice;

    if (existeSimbolo(pal, categoria, escopo)) { // se o símbolo existe na tabela no mesmo escopo
        erroSemantico = SEMANTIC_ERROR_TOKEN_EXISTS;
        hasSemanticError = 1;
        return SEMANTIC_ERROR; // estoura erro de token já existe
    }

    s1 = (Simbolo *) malloc(sizeof(Simbolo));
    s1->categoria = categoria;
    s1->palavra = pal;
    s1->escopo = escopo;
    s1->prox = NULL;
    
    indice = -1;
    indice = indice + 1;
    if (tabela == NULL) {
        tabela = s1;
    } else {
        aux = tabela;
        indice = indice + 1;
        while (aux->prox != NULL) {
            aux = aux->prox;
            indice = indice + 1;
        }
        aux->prox = s1;
    }
    s1->indice = indice;
    
    return indice;
}

int obtemTipoSimbolo(char* pal) { // procura na tabela de símbolos uma variável com o nome pal e que esteja no mesmo escopo que o atual
    Simbolo *aux;
    
    aux = tabela;
    while (aux != NULL) {
        if (strcmp(aux->palavra, pal) == 0 && aux->escopo == escopo) {
            if (aux->categoria != VAR) { // houve declaração de um procedimento como se fosse uma variável
                erroSemantico = SEMANTIC_ERROR_TYPE;
                hasSemanticError = 1;
                return SEMANTIC_ERROR;
            }
            return aux->tipo;
        }
        aux = aux->prox;
    }
    
    if (aux == NULL) { // variável não foi encontrada no escopo em questão
        erroSemantico = SEMANTIC_ERROR_TOKEN_DOES_NOT_EXIST;
        hasSemanticError = 1;
        return SEMANTIC_ERROR;
    }
    
    return 0;
}

// MARK :- ENTRADA
char string[1000]; // String a ser avaliada pelo analisador
int i; // índice do token analisado

int scanner(char string[]); // função para retornar o próximo token válido (ou LEXICAL_ERROR caso haja erro)
int programa(void); // estado inicial do analisador sintático recursivo descendente
int lookahead;
char* outputToken(int result, char string[]); // converte os códigos numéricos em string para saída
int from; // marca o ponto do último token para ser lido pelo output token

int compila() {
    lookahead = scanner(string);
    return programa();
}
    
int main(int argc, const char * argv[]) {
    FILE * file; // arquivo a ser lido 
    char line[128]; // variável temporária com as linhas do arquvio
    int m; // variável auxiliar para imprimir o programa caso haja erro 
    int resultado; // resultado da compilação
    
    file = fopen("/Users/guilhermepaciulli/Documents/stuff/AltPascalCompiler/ProjetoEtapa2/entrada.txt", "r"); // Endereço do arquivo a ser criado
    
    if (file == NULL) { printf("Arquivo não encontrado \n"); return 0; }
    
    i = -1;

    while(fgets(line, 128, file) != NULL) { // concatena as linhas do arquivo em uma string só 
        strcat(string, &line[0]);
    }
    resultado = compila();
    if (resultado == 1) { // caso o resultado for 1, não houve erros
        printf("Programa lexicalmente, sintaticamente e semanticamente correto \n");
    }else if (hasSemanticError) { // senão se houveram erros semânticos
        printf("Erro semântico \n");
        for (m = 0; m < i; m++) { // imprimimos o código até o erro ser encontrado
            printf("%c", string[m]);
        }
        if (erroSemantico == SEMANTIC_ERROR_TOKEN_EXISTS) { // verifica o tipo do erro e imprime mensagem conforme
            printf("\nIdentificador já existe no escopo \n");
        } else if (erroSemantico == SEMANTIC_ERROR_TYPE) {
            printf("\nErro de tipo, atribuição é inválida \n");
        } else if (erroSemantico == SEMANTIC_ERROR_TOKEN_DOES_NOT_EXIST) {
            printf("\nUso de um identifcador não declarado  \n");
        }
    } else if (resultado == 0) { // se o resultado for 0 então houve erro de sintaxe
        printf("Erro de sintaxe \n");
        for (m = 0; m < i; m++) { // imprimimos o código até o erro ser encontrado
            printf("%c", string[m]);
        }
        printf("\nToken %s não esperado \n", outputToken(lookahead, string));
    } else { // senão o erro foi léxico
        printf("Erro léxico \nPalavra: \n\"");
        for (m = from; m < i; m = m + 1) { // imprimimos o código até o erro ser encontrado
            printf("%c", string[m]);
        }
        printf("\"\nÉ inválida\n");
    }
    fclose(file); // fecha o arquivo
    return 0;
}

// MARK:- ANALISADOR LÉXICO
char c; // caratere atual sendo avaliado
int getNext(char str[]); // obtem o próximo caractere incrementando i
int isNumber(void); // verifica se o caractere atual (variável c) é número
int isLetter(void); // verifica se o caractere atual (variável c) é letra
int checkFileEnd(char str[]); // avança o índice e verifica se o caractere c indica um fim de string
int isEndOfFile(char* str); // apenas verifica se a string chegou no seu final
int isEndOfToken(char* str); // verifica se o caracter é um espaço em branco indicando fim do token

int getNext(char str[]) {
    i = i + 1;
    
    if (i >= strlen(str)) return END_OF_STRING;
    
    c = str[i];
    if (c == ' ') return END_OF_TOKEN;
    
    return 0;
}

int isNumber() {
    return c >= '0' && c <= '9';
}

int isLetter() {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int checkFileEnd(char str[]) {
    return i >= strlen(str);
}

int isEndOfFile(char* str) {
    return getNext(str) == END_OF_STRING;
}

int isEndOfToken(char* str) {
    return getNext(str) == END_OF_TOKEN;
}

int scanner(char string[]) {
    from = i + 1;
    goto q0;
    
q0: if (isEndOfFile(string)) return END_OF_STRING;
    else if (c == 'i') goto q6;
    else if (c == 'b') goto q9;
    else if (c == 'p') goto q18;
    else if (c == 'd') goto q31;
    else if (c == 'v') goto q33;
    else if (c == 'f') goto q36;
    else if (c == 't') goto q41;
    else if (c == 'a') goto q45;
    else if (c == 'w') goto q48;
    else if (c == 'e') goto q53;
    else if (c == 'o') goto q63;
    else if (c == 'n') goto q65;
    else if (c == '/') goto q2;
    else if (c == '+') goto q72;
    else if (c == '-') goto q73;
    else if (c == '*') goto q74;
    else if (c == ':') goto q75;
    else if (c == '=') goto q78;
    else if (c == '<') goto q79;
    else if (c == '>') goto q80;
    else if (isNumber()) goto q81;
    else if (c == ';') goto q84;
    else if (c == '.') goto q85;
    else if (c == ',') goto q86;
    else if (c == ')') goto q87;
    else if (c == '(') goto q88;
    else if (c == '_' || isLetter()) goto q97;
    else if (c == ' ') return END_OF_TOKEN;
    else if (c == '\n') return IGNORE;
    else goto q100;
    
q1: if (isEndOfToken(string)) return COMMENT;

q2: if (isEndOfToken(string)) return DIV;
    else if (c == '*') goto q3;

q3: if (isEndOfFile(string)) return LEXICAL_ERROR;
    else if (c == '*') goto q4;
    else goto q3;

q4: if (isEndOfFile(string)) return LEXICAL_ERROR;
    else if (c == '/') goto q1;
    else goto q3;

q6: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'f') goto q17;
    else if (c == 'n') goto q7;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;

q7: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 't') goto q8;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q8: if (isEndOfToken(string)) return _INT; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q9: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'o') goto q10;
    else if (c == 'e') goto q13;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q10: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'o') goto q11;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;

q11: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
     else if (c == 'l') goto q12;
     else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q12: if (isEndOfToken(string)) return BOOL; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q13: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'g') goto q14;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q14: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'i') goto q15;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q15: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'n') goto q16;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q16: if (isEndOfToken(string)) return BEGIN; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q17: if (isEndOfToken(string)) return IF; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q18: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'r') goto q19;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q19: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'o') goto q20;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q20: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'g') goto q21;
    else if (c == 'c') goto q25;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q21: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'r') goto q22;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q22: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'a') goto q23;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q23: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'm') goto q24;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q24: if (isEndOfToken(string)) return PROGRAM; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q25: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'e') goto q26;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;

q26: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'd') goto q27;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;

q27: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'u') goto q28;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q28: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'r') goto q29;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q29: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'e') goto q30;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q30: if (isEndOfToken(string)) return PROCEDURE; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q31: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'o') goto q32;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q32: if (isEndOfToken(string)) return DO; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;

q33: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'a') goto q34;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q34: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'r') goto q35;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q35: if (isEndOfToken(string)) return VAR; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q36: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'a') goto q37;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q37: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'l') goto q38;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q38: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 's') goto q39;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q39: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'e') goto q40;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q40: if (isEndOfToken(string)) return _FALSE; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q41: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'h') goto q42;
    else if (c == 'r') goto q69;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q42: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'e') goto q43;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q43: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'n') goto q44;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q44: if (isEndOfToken(string)) return THEN; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q45: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'n') goto q46;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q46: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'd') goto q47;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q47: if (isEndOfToken(string)) return AND; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q48: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'r') goto q49;
    else if (c == 'h') goto q59;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q49: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'i') goto q50;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q50: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 't') goto q51;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q51: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'e') goto q52;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q52: if (isEndOfToken(string)) return WRITE; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q53: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'n') goto q54;
    else if (c == 'l') goto q56;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q54: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'd') goto q55;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q55: if (isEndOfToken(string)) return END; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q56: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 's') goto q57;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q57: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'e') goto q58;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q58: if (isEndOfToken(string)) return ELSE; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q59: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'i') goto q60;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q60: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'l') goto q61;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q61: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'e') goto q62;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q62: if (isEndOfToken(string)) return WHILE; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q63: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'r') goto q64;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q64: if (isEndOfToken(string)) return OR; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q65: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'o') goto q66;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q66: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 't') goto q67;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q67: if (isEndOfToken(string)) return NOT; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q69: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'u') goto q70;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q70: if (isEndOfFile(string)) return LEXICAL_ERROR; // Palavra reservada
    else if (c == 'e') goto q71;
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q71: if (isEndOfToken(string)) return _TRUE; // Palavra reservada
    else if (c == '_' || isLetter() || isNumber() || c == ' ') goto q97;
    
q72: if (isEndOfToken(string)) return PLUS;
    
q73: if (isEndOfToken(string)) return MINUS;
    
q74: if (isEndOfToken(string)) return TIMES;
    
q75: if (isEndOfToken(string)) return COLON;
    else if (c == '=') goto q77;
    
q76: if (isEndOfToken(string)) return EQUIVALENT;
    
q77: if (isEndOfToken(string)) return RECEIVES;

q78: if (isEndOfToken(string)) return EQUALS;
    
q79: if (isEndOfToken(string)) return LESS_THAN;
    else if (c == '=') goto q83;
    else if (c == '>') goto q76;
    
q80: if (isEndOfToken(string)) return GREATER_THAN;
    else if (c == '=') goto q82;
    
q81: if (isEndOfToken(string)) return NUMBER;
    else if (isNumber()) goto q81;
    
q82: if (isEndOfToken(string)) return GREATER_THAN_OR_EQUAL;
    
q83: if (isEndOfToken(string)) return LESS_THAN_OR_EQUAL;
    
q84: if (isEndOfToken(string)) return SEMICOLON;
    
q85: if (isEndOfToken(string)) return DOT;
    
q86: if (isEndOfToken(string)) return COMMA;
    
q87: if (isEndOfToken(string)) return CLOSE_BRACKTES;
    
q88: if (isEndOfToken(string)) return OPEN_BRACKETS;
    
q97: if (1) {
        if (c == ' ') { return IDENTIFIER; }
        int res2;
        res2 = getNext(string);
        if (res2 == END_OF_TOKEN || (c == ' ' && res2 == END_OF_STRING)) return IDENTIFIER; // Identificador
        else if (isLetter() || isNumber() || c == '_') goto q97;
    }
    
q100: if(isEndOfToken(string) || checkFileEnd(string)) return LEXICAL_ERROR;
    else goto q100;
    
    return LEXICAL_ERROR;
}

char* outputToken(int result, char string[]) { // obtem a string literal no código fonte (a.k.a. string) dado um resultado)
    char* strResult = NULL;
    int aux1;
    int j;

    switch (result) {
        case END_OF_STRING: strResult = "<END_OF_FILE>"; break;
        case END_OF_TOKEN: strResult = "<END_OF_TOKEN>"; break;
        case LEXICAL_ERROR: strResult = "<LEXICAL_LEXICAL_ERROR>"; break;
        case PROGRAM: strResult = "<PROGRAM>"; break;
        case BEGIN: strResult = "<BEGIN>"; break;
        case END: strResult = "<END>"; break;
        case PROCEDURE: strResult = "<PROCEDURE>"; break;
        case IF: strResult = "<IF>"; break;
        case THEN: strResult = "<THEN>"; break;
        case ELSE: strResult = "<ELSE>"; break;
        case WHILE: strResult = "<WHILE>"; break;
        case DO: strResult = "<DO>"; break;
        case AND: strResult = "<AND>"; break;
        case OR: strResult = "<OR>"; break;
        case NOT: strResult = "<NOT>"; break;
        case VAR: strResult = "<VAR>"; break;
        case _INT: strResult = "<INT>"; break;
        case BOOL: strResult = "<BOOL>"; break;
        case _TRUE: strResult = "<TRUE>"; break;
        case _FALSE: strResult = "<FALSE>"; break;
        case WRITE: strResult = "<WRITE>"; break;
        case IDENTIFIER:
            j = 0;
            strResult = malloc(i - from - 1);
            for (aux1 = from; aux1 < i; aux1 = aux1 + 1) {
                strResult[j] = string[aux1];
                j = j + 1;
            }
            break;
        case PLUS: strResult = "<PLUS>"; break;
        case MINUS: strResult = "<MINUS>"; break;
        case DIV: strResult = "<DIV>"; break;
        case TIMES: strResult = "<TIMES>"; break;
        case LESS_THAN: strResult = "<LESS_THAN>"; break;
        case GREATER_THAN: strResult = "<GREATER_THAN>"; break;
        case EQUALS: strResult = "<EQUALS>"; break;
        case LESS_THAN_OR_EQUAL: strResult = "<LESS_THAN_OR_EQUAL>"; break;
        case GREATER_THAN_OR_EQUAL: strResult = "<GREATER_THAN_OR_EQUAL>"; break;
        case RECEIVES: strResult = "<RECEIVES>"; break;
        case EQUIVALENT: strResult = "<EQUIVALENT>"; break;
        case OPEN_BRACKETS: strResult = "<OPEN_BRACKETS>"; break;
        case CLOSE_BRACKTES: strResult = "<CLOSE_BRACKTES>"; break;
        case COMMA: strResult = "<COMMA>"; break;
        case COLON: strResult = "<COLON>"; break;
        case DOT: strResult = "<DOT>"; break;
        case SEMICOLON: strResult = "<SEMICOLON>"; break;
        case COMMENT: strResult = "<COMMENT>"; break;
        case NUMBER:
            strResult = malloc(i - from + 8);
            strcpy(strResult, "<NUMBER, ");
            int h;
            h = 9;
            for (aux1 = from; aux1 < i; aux1 = aux1 + 1) {
                strResult[h] = string[aux1];
                h = h + 1;
            }
            strResult[h] = '>';
            break;
        default: strResult = "<LEXICAL_LEXICAL_ERROR>"; break;
    }
    
    return strResult;
}

// MARK: - ANALISADOR SINTÁTICO
int match(int word);
// MARK: - SINTAXE
int bloco(void);
// MARK: - DECLARAÇÕES
int parte_de_declaracao_variaveis(void);
int declaracao_variaveis(void);
int lista_identificadores(void);
int identificador(void);
int parte_de_declaracao_subrotinas(void);
int declaracao_procedimentos(void);
int parametros_formais(void);
int parametro_formal(void);
int parametro_formal_aux(void);
int tipo(void);
// MARK: - COMANDOS
int comando_composto(void);
int comando_aux(void);
int comando(void);
int atribuicao(void);
int chamada_procedimento(void);
int lista_parametro_chamada_procedimento(void);
int lista_parametro_chamada_procedimento_aux(void);
int parametro_chamada_procedimento(void);
int parametro_chamada_procedimento_tipo(void);
int comando_condicional(void);
int senao(void);
int comando_repetitivo(void);
int escreve(void);
// MARK: - EXPRESSÕES
int expressao(int tipo);
int expressao_aux(int tipo);
int expressao_simples(int tipo);
int expressao_simples_aux(int tipo);
int sinal(void);
int sinal_aux(void);
int termo(int tipo);
int termo_aux(int tipo);
int fator(int tipo);
int fator_aux(int tipo);
int relacao(void);
int booleano(void);
int variavel(void);
int fatores(void);

int match(int word) {
    if (lookahead == word) {
        lookahead = scanner(string);
        while (lookahead == IGNORE) { // ignora as quebras de linha
            lookahead = scanner(string);
        }
        return 1;
    }
    return 0;
}

// MARK :- SINTAXE
// Utilizou-se E como palavra vazia
// O `lookahead == IGNORE` foi utilizado para ignorar as quebras de linha (a.k.a. \n),

int programa() { // <programa> ::= program <identificador> ; <bloco> . END_OF_STRING
    if (lookahead == PROGRAM) {
        if (match(PROGRAM) && match(IDENTIFIER) && match(SEMICOLON) && bloco() && match(DOT)) return 1;
    }
    return 0;
}

int bloco() { // <bloco> ::= <parte de declarações de variáveis> <parte de declarações de sub-rotinas> <comando composto>
    if (parte_de_declaracao_variaveis() && parte_de_declaracao_subrotinas() && comando_composto()) return 1;
    return 0;
}

// MARK :- DECLARAÇÕES

int parte_de_declaracao_variaveis() { // <parte de declarações de variáveis> ::= var <declaração de variáveis> ; <declaração de variáveis> ; | E
    if (lookahead == VAR) {
        if (match(VAR) && declaracao_variaveis() && match(SEMICOLON) && parte_de_declaracao_variaveis()) return 1;
    }
    return 1;
}

int identificadores[100]; // define uma lista com os identificadores a serem incluídos na tabela de símbolos
int numIds; // define a quantidade de identificadores sendo colocados na lista
int declaracao_variaveis() { // <declaração de variáveis> ::= <lista de identificadores> : <tipo>
    numIds = -1; // inicializa como -1 o número de identificadores desta declaração de variáveis
    if (lista_identificadores() && match(COLON) && tipo()) return 1;
    return 0;
}
int lista_identificadores() { // <lista de identificadores> ::= IDENTIFIER , <identificador>
    if (lookahead == IDENTIFIER) {
        numIds = numIds + 1; // avança a quantidade de variáveis obtidas      
        identificadores[numIds] = insereSimbolo(outputToken(lookahead, string), VAR); // obtem a string literal da variável e insere-o na tabela de símbolos
        if (identificadores[numIds] == SEMANTIC_ERROR) return 0; // se houver erro, voltar falso
        if (match(IDENTIFIER) && identificador()) return 1;
    }
    return 0;
}

int identificador() { // <identificador> ::= , <lista de identificadores> | E
    if (lookahead == COMMA || lookahead == IGNORE) {
        if (match(COMMA) && lista_identificadores()) return 1;
    }
    return 1;
}

int parte_de_declaracao_subrotinas() { // <declaracao de subrotinas> ::=  <declaracao de procedimentos> | E
    if (declaracao_procedimentos()) return 1;
    return 1;
}

void limpaSimbolos() { // limpa o escopo atual
    Simbolo* aux;
    Simbolo* anterior;

    anterior = NULL;
    aux = tabela;
    while (aux != NULL) {
        if (aux->escopo == escopo) {
            anterior->prox = aux->prox;
            aux = anterior;
        }
        anterior = aux;
        aux = aux->prox;
    }
    escopo = escopo - 1;
}

int declaracao_procedimentos() { // <declaracao de procedimentos> ::= procedure IDENTIFIER <parametros formais> ; <bloco> ;
    if (lookahead == PROCEDURE || lookahead == IGNORE) {
        
        if (match(PROCEDURE)) {
            int indice;
            indice = insereSimbolo(outputToken(lookahead, string), PROCEDURE); // insere identificador do procedure na tabela de símbolos
            if (indice == SEMANTIC_ERROR) return 0;
            escopo = escopo + 1; // cria escopo novo
            if (match(IDENTIFIER) && parametros_formais() && match(SEMICOLON) && bloco() && match(SEMICOLON)) {
                limpaSimbolos(); // limpa escopo atual 
                return 1;
            }            
        }
        
    }
    
    return 0;
}

int parametros_formais() { // <parametros formais> ::= ( <parametro formal> <parametro formal aux> )
    if (lookahead == OPEN_BRACKETS || lookahead == IGNORE) {
        numIds = -1; // inicializa como -1 o número de identificadores para estes parâmetros formais
        if (match(OPEN_BRACKETS) && parametro_formal() && parametro_formal_aux() && match(CLOSE_BRACKTES)) return 1;
    }
    return 0;
}

int parametro_formal() { // <parametro formal> ::= var IDENTIFIER , <tipo>
    if (lookahead == VAR || lookahead == IGNORE) {        
        if (match(VAR)) { 
            numIds = numIds + 1; // avança a quantidade de variáveis obtidas
            identificadores[numIds] = insereSimbolo(outputToken(lookahead, string), VAR); // obtem a string literal da variável e insere-o na tabela de símbolos
            if (identificadores[numIds] == SEMANTIC_ERROR) return 0; // se houver erro, voltar falso
            if (match(IDENTIFIER) && match(COLON) && tipo()) return 1;
        }
    }
    return 0;
}

int parametro_formal_aux() { // <parametro formal aux> ::= , <parametro formal> <parametro formal aux> | E
    if (lookahead == COMMA || lookahead == IGNORE) {
        if (match(COMMA) && parametro_formal() && parametro_formal_aux()) return 1;
    }
    return 1;
}

void defineTipos(int tipo) { // itera sobre os identficadores obtidos no array identificadores e insere-os na lista de símbolos com o tipo em questão
    int x;
    Simbolo *aux;
    
    for (x = 0; x <= numIds; x = x + 1) {
        int indice;
        
        indice = identificadores[x];
        aux = tabela;
        while (aux != NULL) {
            if (aux->indice == indice) {
                aux->tipo = tipo;
                break;
            }
            aux = aux->prox;
        }
    }
}

int tipo() { // <tipo> ::= bool | int
    if (lookahead == _INT || lookahead == IGNORE) {
        defineTipos(_INT); // define os tipos obtidos na lista como INTEIROS
        if (match(_INT)) return 1;
    } else if (lookahead == BOOL || lookahead == IGNORE) {
        defineTipos(BOOL); // define os tipos obtidos na lista como BOOLEANOS
        if (match(BOOL)) return 1;
    }
    return 0;
}

int comando_composto() { // <comando composto> ::= begin <comando> <comando aux> end | E
    if (lookahead == BEGIN || lookahead == IGNORE) {
        if (match(BEGIN) && comando() && match(SEMICOLON) && comando_aux() && match(END)) return 1; 
    }
    return 0;
}

int comando_aux() { // <comando aux> ::= <comando> ; <comando aux> | E
    if (comando() && match(SEMICOLON) && comando_aux()) return 1;
    return 1;
}

// <comando> ::= <atribuição> | <chamada de procedimento> | <comando composto> | <comando condicional> | <comando repetitivo> | <escreve>
int comando() {
    if (atribuicao()) { return 1;
    } else if (chamada_procedimento()) { return 1;
    } else if (comando_composto()) { return 1;
    } else if (comando_condicional()) { return 1;
    } else if (comando_repetitivo()) { return 1;
    } else if (escreve()) { return 1; }
    return 0;
}

int tipoVariavel = 0; // Define o tipo da variável que está sendo atribuiída nesta função
int atribuicao() { // <atribuição> ::= IDENTIFIER := <expressão>
    tipoVariavel = 0;
    if (variavel() && match(RECEIVES) && expressao(tipoVariavel)) return 1;
    return 0;
}

int chamada_procedimento() { // <chamada procedimento> ::= IDENTIFIER ( <lista parâmetro chamada procedimento> )
    if (lookahead == IDENTIFIER || lookahead == IGNORE) {
        if (match(IDENTIFIER) && match(OPEN_BRACKETS) && lista_parametro_chamada_procedimento() && match(CLOSE_BRACKTES)) return 1;
    }
    return 0;
}

// <lista parametros chamada procedimento> ::= <parametro chamada procedimento> <parametro chamada procedimento aux> | E
int lista_parametro_chamada_procedimento() {
    if (parametro_chamada_procedimento() && lista_parametro_chamada_procedimento_aux()) return 1;
    return 1;
}

// <lista parametro chamada procedimento aux> ::= ; <parametro chamada procedimento> <lista parametro chamada procedimento aux> | E
int lista_parametro_chamada_procedimento_aux() {
    if (lookahead == SEMICOLON || lookahead == IGNORE) {
        if (match(SEMICOLON) && parametro_chamada_procedimento() && lista_parametro_chamada_procedimento_aux()) return 1;
    }
    return 1;
}

int parametro_chamada_procedimento() { // <parametro_chamada_procedimento> ::= ( <parametro chamada procedimento tipo> )
    if (lookahead == OPEN_BRACKETS || lookahead == IGNORE) {
        if (match(OPEN_BRACKETS) && parametro_chamada_procedimento_tipo() && match(CLOSE_BRACKTES)) return 1;
    }
    return 0;
}

int parametro_chamada_procedimento_tipo() { // <parametro chamada procedimento tipo> ::= IDENTIFIER | NUMBER | BOOL
    if (lookahead == IDENTIFIER || lookahead == IGNORE) {
        if (match(IDENTIFIER)) return 1;
    } else if (lookahead == NUMBER || lookahead == IGNORE) {
        if (match(NUMBER)) return 1;
    } else if (lookahead == BOOL || lookahead == IGNORE) {
        if (match(BOOL)) return 1;
    }
    return 0;
}

int comando_condicional() { // <comando condicional> ::= if ( <expressão> ) then <comando> <senão>
    if (lookahead == IF || lookahead == IGNORE) {
        if (match(IF) && match(OPEN_BRACKETS) && expressao(0) && match(CLOSE_BRACKTES) && match(THEN) && comando() && senao()) return 1;
    }
    return 0;
}

int senao() { // <senao> ::= else <comando> | E
    if (lookahead == ELSE || lookahead == IGNORE) {
        if (match(ELSE) && comando()) return 1;
    }
    return 1;
}

int comando_repetitivo() { // <comando repetitivo> ::= while ( <expressão> ) do <comando>
    if (lookahead == WHILE || lookahead == IGNORE) {
        if (match(WHILE) && match(OPEN_BRACKETS) && expressao(0) && match(CLOSE_BRACKTES) && match(DO) && comando()) return 1;
    }
    return 0;
}

int escreve() { // <escreve> ::= write ( IDENTIFIER )
    if (lookahead == WRITE || lookahead == IGNORE) {
        imprimeTabelaSimbolos();
        if (match(WRITE) && match(OPEN_BRACKETS) && match(IDENTIFIER) && match(CLOSE_BRACKTES)) return 1;
    }
    return 0;
}

int expressao(int tipo) { // <expressão> ::= <expressão simples> <expressao aux>
    if (expressao_simples(tipo) && expressao_aux(tipo)) return 1;
    return 0;
}

int expressao_aux(int tipo) { // <expressão aux> ::= <relação> <expressão simples> | E
    if (relacao() && expressao_simples(tipo)) return 1;
    return 1;
}

int relacao() { // <relação> ::= <> | = | < | <= | >= | >
    if (lookahead == EQUIVALENT || lookahead == IGNORE) {
        if (match(EQUIVALENT)) return 1;
    } else if (lookahead == EQUALS || lookahead == IGNORE) {
        if (match(EQUALS)) return 1;
    } else if (lookahead == LESS_THAN || lookahead == IGNORE) {
        if (match(LESS_THAN)) return 1;
    } else if (lookahead == LESS_THAN_OR_EQUAL || lookahead == IGNORE) {
        if (match(LESS_THAN_OR_EQUAL)) return 1;
    } else if (lookahead == GREATER_THAN_OR_EQUAL || lookahead == IGNORE) {
        if (match(GREATER_THAN_OR_EQUAL)) return 1;
    } else if (lookahead == GREATER_THAN || lookahead == IGNORE) {
        if (match(GREATER_THAN)) return 1;
    }
    return 0;
}

int expressao_simples(int tipo) { // <expressão simples> ::= <sinal> <termo> <termo aux>
    if (sinal() && termo(tipo) && termo_aux(tipo)) return 1;
    return 0;
}

int expressao_simples_aux(int tipo) { // <expressão simples aux> ::= <sinal> <termo> <termo aux> | E
    if (sinal_aux() && termo(tipo) && termo_aux(tipo)) return 1;
    return 1;
}

int sinal() { // <sinal> ::= + | - | E
    if (lookahead == PLUS || lookahead == IGNORE) {
        if (match(PLUS)) return 1;
    } else if (lookahead == MINUS || lookahead == IGNORE) {
        if (match(MINUS)) return 1;
    }
    return 1;
}

int sinal_aux() { // <sinal aux> ::= + | -
    if (lookahead == PLUS || lookahead == IGNORE) {
        if (match(PLUS)) return 1;
    } else if (lookahead == MINUS || lookahead == IGNORE) {
        if (match(MINUS)) return 1;
    }
    return 0;
}

int termo(int tipo) { // <termo> ::= <fator> <termo aux>
    if (fator(tipo) && fator_aux(tipo)) return 1;
    return 0;
}

int termo_aux(int tipo) { // <termo aux> ::= <sinal aux> <termo> | E
    if (sinal_aux() && termo(tipo)) return 1;
    return 1;
}

int fator_aux(int tipo) { // <fator aux> ::= <fatores> <fator> | E
    if (fatores() && fator(tipo)) return 1;
    return 1;
}

int fator(int tipo) { // <fator> ::= <variavel> | NUMBER | <bool> | <expressão simples>
    if (variavel()) {
        return 1;
    } else if (lookahead == NUMBER || lookahead == IGNORE) {
        if (tipo != _INT && tipo != 0) {
            hasSemanticError = 1;
            erroSemantico = SEMANTIC_ERROR_TYPE;
            return 0;
        }
        if (match(NUMBER)) return 1;
    } else if (booleano()) {
        if (tipo != BOOL && tipo != 0) {
            hasSemanticError = 1;
            erroSemantico = SEMANTIC_ERROR_TYPE;
            return 0;
        }
        return 1;
    } else if (expressao_simples_aux(tipo)) {
        return 1;
    }
    return 0;
}

int booleano() { // <booleano> ::= true | false
    if (lookahead == _TRUE || lookahead == IGNORE) {
        if (match(_TRUE)) return 1;
    } else if (lookahead == _FALSE || lookahead == IGNORE) {
        if (match(_FALSE)) return 1;
    }
    return 0;
}

int variavel() { // <variavel> ::= IDENTIFIER
    if (lookahead == IDENTIFIER || lookahead == IGNORE) {
        if (tipoVariavel != 0) { // caso o tipo já tiver sido definido, então estamos avaliando outra variável
            int tipo;
            char* pal;
            
            pal = outputToken(IDENTIFIER, string); // obtemos o string literal da variavel 
            tipo = obtemTipoSimbolo(pal); // buscamos na tabela de símbolos
            
            if (tipo == SEMANTIC_ERROR) return 0; // caso houver erro, voltar falso
            
            if (tipo != tipoVariavel) { // caso o tipo for diferente da variável sendo atribuída
                hasSemanticError = 1;
                erroSemantico = SEMANTIC_ERROR_TYPE; // houve um erro de tipo
                return 0; // retornamos falso
            }
        } else { // caso não tiver sido definido
            tipoVariavel = obtemTipoSimbolo(outputToken(IDENTIFIER, string)); // obtemos o tipo da variável sendo atribuída
            if (tipoVariavel == SEMANTIC_ERROR) return 0; // caso houver erro, voltar falso
        }
        if (match(IDENTIFIER)) return 1;
    }
    return 0;
}

int fatores() { // <fatores> ::= * | /
    if (lookahead == TIMES || lookahead == IGNORE) {
        if (match(TIMES)) return 1;
    } else if (lookahead == DIV || lookahead == IGNORE) {
        if (match(DIV)) return 1;
    }
    return 0;
}


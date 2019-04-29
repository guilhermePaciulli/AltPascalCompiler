//
//  main.c
//  ProjetoEtapa2
//
//  Created by Guilherme Paciulli on 02/04/19.
//  Copyright © 2019 Guilherme Paciulli. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Valores de controle
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

// MARK :- ENTRADA
char string[1000]; // String a ser avaliada pelo analisador
int i; // índice do token analisado

int scanner(char string[]); // função para retornar o próximo token válido (ou LEXICAL_ERROR caso haja erro)
int programa(int word, int *index); // estado inicial do analisador sintático recursivo descendente
int lookahead;

int analise_sintatica() { return programa(lookahead, 0); }
    
int main(int argc, const char * argv[]) {
    int resultado;
    FILE * file;
    char line[128];
    int lineIndex;
    
    file = fopen("/Users/ghpaciulli/Documents/seFormarEmQuatroAnos/compiladores/ProjetoEtapa2/ProjetoEtapa2/entrada.txt", "r"); // Endereço do arquivo a ser criado
    i = -1;
    
    lineIndex = 0;
    while(fgets(line, 128, file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        strcat(string, &line[0]);
    }
    printf("%s", string);
    
//    resultado = analise_sintatica();
    
//    printf("%d\n", resultado);
    fclose(file);
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
int from; // marca o ponto do último token para ser lido pelo output token
char* outputToken(int result, char string[]); // converte os códigos numéricos em string para saída

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
        int res2;
        res2 = getNext(string);
        if (res2 == END_OF_TOKEN || (c == ' ' && res2 == END_OF_STRING)) return IDENTIFIER; // Identificador
        else if (isLetter() || isNumber() || c == '_') goto q97;
    }
    
q100: if(isEndOfToken(string) || checkFileEnd(string)) return LEXICAL_ERROR;
    else goto q100;
    
    return LEXICAL_ERROR;
}

char* getIdentifier(int from, char string[], char tokenName[]) {
    char* strResult = malloc(i - from + strlen(tokenName) - 1);
    int j;
    int aux1;
    
    strcpy(strResult, "<ID, ");
    j = 5;
    for (aux1 = from; aux1 < i; aux1 = aux1 + 1) {
        strResult[j] = string[aux1];
        j = j + 1;
    }
    strResult[j] = '>';
    
    return strResult;
}

char* outputToken(int result, char string[]) {
    char* strResult = NULL;
    int aux1;
    switch (result) {
        case END_OF_STRING: break;
        case END_OF_TOKEN: break;
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
            strResult = malloc(i - from + 4);
            strcpy(strResult, "<ID, ");
            int j;
            j = 5;
            for (aux1 = from; aux1 < i; aux1 = aux1 + 1) {
                strResult[j] = string[aux1];
                j = j + 1;
            }
            strResult[j] = '>';
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
int match(int s, int word, int *index);
// MARK: - SINTAXE
int bloco(int word, int *index);
// MARK: - DECLARAÇÕES
int parte_de_declaracao_variaveis(int word, int *index);
int declaracao_variaveis(int word, int *index);
int lista_identificadores(int word, int *index);
int identificador(int word, int *index);
int parte_de_declaracao_subrotinas(int word, int *index);
int declaracao_procedimentos(int word, int *index);
int parametros_formais(int word, int *index);
int parametro_formal(int word, int *index);
int parametro_formal_aux(int word, int *index);
int tipo(int word, int *index);
// MARK: - COMANDS
int comando_composto(int word, int *index);

int match(int s, int word, int *index) {
    lookahead = scanner(string);
    if (s == word) return 1;
    return 0;
}

// MARK :- SINTAXE
// Utilizou-se E como palavra vazia

int programa(int word, int *index) { // <programa> ::= program <identificador> ; <bloco>.
    if (lookahead == PROGRAM) {
        if (match(PROGRAM, word, index) && match(IDENTIFIER, word, index) && match(SEMICOLON, word, index) && bloco(word, index) && match(DOT, word, index)) return 1;
    }
    return 0;
}

int bloco(int word, int *index) { // <bloco> ::= <parte de declarações de variáveis> <parte de declarações de sub-rotinas> <comando composto>
    if (parte_de_declaracao_variaveis(word, index) && parte_de_declaracao_subrotinas(word, index) && comando_composto(word, index)) return 1;
    return 0;
}

// MARK :- DECLARAÇÕES

int parte_de_declaracao_variaveis(int word, int *index) { // <parte de declarações de variáveis> ::= var <declaração de variáveis> ; <declaração de variáveis> ; | E
    if (lookahead == VAR) {
        if (match(VAR, word, index) && declaracao_variaveis(word, index) && match(SEMICOLON, word, index) && parte_de_declaracao_variaveis(word, index)) return 1;
    }
    return 1;
}

int declaracao_variaveis(int word, int *index) { // <declaração de variáveis> ::= <lista de identificadores> : <tipo>
    if (lista_identificadores(word, index) && match(COLON, word, index) && tipo(word, index)) return 1;
    return 0;
}

int lista_identificadores(int word, int *index) { // <lista de identificadores> ::= <identificador> , <identificador>
    if (lookahead == IDENTIFIER) {
        if (match(IDENTIFIER, word, index) && identificador(word, index)) return 1;
    }
    return 0;
}

int identificador(int word, int *index) {
    if (lookahead == COMMA) {
        if (match(COMMA, word, index) && lista_identificadores(word, index)) return 1;
    }
    return 1;
}

int parte_de_declaracao_subrotinas(int word, int *index) {
    if (declaracao_procedimentos(word, index)) return 1;
    return 1;
}

int declaracao_procedimentos(int word, int *index) {
    if (lookahead == PROCEDURE) {
        if (match(PROCEDURE, word, index) && match(IDENTIFIER, word, index)
            && parametros_formais(word, index) && match(SEMICOLON, word, index)
            && bloco(word, index) && match(SEMICOLON, word, index)) return 1;
    }
    
    return 0;
}

int parametros_formais(int word, int *index) {
    if (lookahead == OPEN_BRACKETS) {
        if (match(OPEN_BRACKETS, word, index) && parametro_formal(word, index) && parametro_formal_aux(word, index) && match(CLOSE_BRACKTES, word, index)) return 1;
    }
    return 0;
}

int parametro_formal(int word, int *index) {
    if (lookahead == VAR) {
        if (match(VAR, word, index) && match(IDENTIFIER, word, index) && match(COLON, word, index) && tipo(word, index)) return 1;
    }
    return 0;
}

int parametro_formal_aux(int word, int *index) {
    if (lookahead == COMMA) {
        if (match(COMMA, word, index) && parametro_formal(word, index) && parametro_formal_aux(word, index)) return 1;
    }
    return 1;
}

int tipo(int word, int *index) {
    if (lookahead == _INT) {
        if (match(_INT, word, index)) return 1;
    } else if (lookahead == BOOL) {
        if (match(BOOL, word, index)) return 1;
    }
    return 0;
}

int comando_composto(int word, int *index) {
    return 0;
}


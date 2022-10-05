#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define DIR "./img/"
typedef struct nosetor{
    long int inicio;
    long int fim;
    struct nosetor* prox;
    struct nosetor* ant;
}NoSetor;

typedef struct noarquivo{
    char nome[40];
    long int tam;
    NoSetor* setores;
    struct noarquivo* prox;
    struct noarquivo* ant;
}NoArquivo;

typedef struct{
    char nome[40];
    void* disco;
    NoSetor* livres;
    NoArquivo* arquivos;
    long int tamDisco;
    long int espacoLivre;
    long int qtdeArquivos;    
}Disco;


// PROTÓTIPOS DAS FUNÇÕES
long int descobreTamanho(FILE* arq);
void disco_lista(Disco* d);
Disco* disco_cria(char* nome, long int tamanho);
bool disco_grava(Disco* d, char* arquivo); // nome arquivo deve conter o caminho absoluto ou relativo do arquivo
bool disco_remove(Disco* d, char* nome); // somente o nome do arquivo sem o caminho
bool disco_recupera(Disco* d, char* nome, char* arquivoDestino); // nome arquivo deve conter o caminho absoluto ou relativo do arquivo
void imprimeSetoresLivres(Disco* d);
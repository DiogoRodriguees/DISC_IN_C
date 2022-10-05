#include "disco.h"

// PROTÓTIPOS DAS FUNÇÕES
bool agrupaNos(Disco *d)
{
    NoSetor *auxAtual = d->livres->prox; // Passa todas as posições de livres
    NoSetor *aux;                        // Passa todas as posições de livres

    // Passa pelos arquivos
    while (auxAtual != d->livres)
    {
        // recebendo o primeiro no livre
        aux = d->livres->prox;

        // Compara o fim do arquiovo atual com o inicio dos demais
        while (aux != d->livres)
        {
            // Verifica se sao iguais e junta os dois nós
            if (aux->inicio == auxAtual->fim)
            {

                // Atualizando o No atual
                auxAtual->fim = aux->fim;

                // Desencadeamento do no mais afastado
                aux->prox->ant = aux->ant;
                aux->ant->prox = aux->prox;
                free(aux);
            }
            aux = aux->prox;
        }

        auxAtual = auxAtual->prox;
    }

    return true;
}

Disco *disco_cria(char *nome, long int tamanho)
{
    Disco *discoNovo = (Disco *)malloc(sizeof(Disco));

    // disco com as posições preenchidas e livres
    discoNovo->disco = (void *)calloc(tamanho, sizeof(void));

    // Inserindo elementos que não são ponteiros
    discoNovo->qtdeArquivos = 0;
    discoNovo->qtdeArquivos = 0;
    discoNovo->tamDisco = tamanho;
    strcpy(discoNovo->nome, nome);

    // Criando sentinela dos espaços livres
    NoSetor *sentinelaSetorLivre = (NoSetor *)malloc(sizeof(NoSetor));
    sentinelaSetorLivre->prox = sentinelaSetorLivre;
    sentinelaSetorLivre->ant = sentinelaSetorLivre;

    NoSetor *arqLivres = (NoSetor *)malloc(sizeof(NoSetor));

    // Preenche o primeiro no de Arquivo Livres
    arqLivres->inicio = 0;
    arqLivres->fim = tamanho;
    arqLivres->ant = sentinelaSetorLivre;
    arqLivres->prox = sentinelaSetorLivre;

    // encadeando arqLivres e adicionando ao disco
    sentinelaSetorLivre->ant->prox = arqLivres;
    sentinelaSetorLivre->ant = arqLivres;

    discoNovo->livres = sentinelaSetorLivre;

    // Criando nó setinela Nulo dea rquivos
    NoArquivo *arquivos = (NoArquivo *)malloc(sizeof(NoArquivo));
    arquivos->prox = arquivos;
    arquivos->ant = arquivos;

    discoNovo->arquivos = arquivos;

    return discoNovo;
}

long int descobreTamanho(FILE *arq)
{
    rewind(arq);
    fseek(arq, 0, SEEK_END);
    int tamanho = ftell(arq);
    rewind(arq);
    return tamanho;
}

NoSetor *procuraEspacoLivre(long int tamNecessario, Disco *d)
{
    NoSetor *aux = d->livres->prox;
    NoSetor *livre = (NoSetor *)malloc(sizeof(NoSetor));

    if (tamNecessario >= (aux->fim - aux->inicio))
    {
        // Passa o No para o Setor Livre
        livre = aux;

        // Desencadeando No Livre
        aux->ant->prox = aux->prox;
        aux->prox->ant = aux->ant;
    }
    else
    {
        // Copia os dados necessarios para o NoSetor livre
        livre->inicio = aux->inicio;
        livre->fim = (aux->inicio + tamNecessario);
        livre->prox = livre;
        livre->ant = livre;

        // Atualiza o primeiro no livre
        aux->inicio = (aux->inicio + tamNecessario);
    }

    return livre;
}

bool disco_grava(Disco *d, char *arquivo) // nome arquivo deve conter o caminho absoluto ou relativo do arquivo
{
    FILE *arqOrigem;
    long int arqCopiado = 0;
    long int arqRestante; // Tamanho que falta copiar

    char caminho[200];
    caminho[0] = '\0';
    strcat(caminho, DIR);
    strcat(caminho, arquivo);

    // Abrindo arquivo para leitura
    arqOrigem = fopen(caminho, "rb");

    // Verificando se o arquivo esta vazio
    if (arqOrigem == NULL)
    {
        perror("Erro na abetura do arquivo");
        return false;
    }

    // Descobrindo o tamanaho do arquivo
    long int tamArquivoOrigem = descobreTamanho(arqOrigem);

    // Criando uma copia namemoria antes de passar para o disco
    void *buffer = (void *)calloc(tamArquivoOrigem, sizeof(void));

    if (buffer == NULL)
    {
        perror("Memoria insuficiente");
        return false;
    }

    // Copiando o arquivo no buffer
    fread(buffer, tamArquivoOrigem, 1, arqOrigem);

    long int espacoLivre;                                        // Variavel com o espaço disponivel em disco
    NoSetor *segmentoLivre = (NoSetor *)malloc(sizeof(NoSetor)); // No com o setor livre no disco
    arqRestante = tamArquivoOrigem;                              // Passando o tamanho do arquivo

    // Sentinela aux para os setores do arquivo que será adicionado
    NoSetor *sentinela = (NoSetor *)malloc(sizeof(NoSetor));
    sentinela->ant = sentinela;
    sentinela->prox = sentinela;

    // Novo Arquivo
    NoArquivo *arqAdd = (NoArquivo *)malloc(sizeof(NoArquivo));
    strcpy(arqAdd->nome, arquivo);

    // Colocando o novo arquivo na lista de arquivos
    arqAdd->prox = d->arquivos;
    arqAdd->ant = d->arquivos->ant;

    // Atualizando o fim da lista de arquivos
    d->arquivos->ant->prox = arqAdd;
    d->arquivos->ant = arqAdd;

    // Preenchendo as informações faltantes
    arqAdd->setores = sentinela;
    arqAdd->tam = tamArquivoOrigem;

    // Escrevendo o conteudo no disco
    while (arqCopiado < tamArquivoOrigem)
    {
        // Procura um No com espaçolivre
        segmentoLivre = procuraEspacoLivre(arqRestante, d);

        // Recebe o espaço livre para copia
        espacoLivre = (segmentoLivre->fim - segmentoLivre->inicio);

        // Copia os valores
        memcpy(d->disco + segmentoLivre->inicio, buffer + arqCopiado, espacoLivre);

        // Atualizando as variaveis depois da copia
        arqCopiado += espacoLivre;
        arqRestante -= espacoLivre;

        // Encadeando o novo segmento
        segmentoLivre->ant = sentinela->ant;
        segmentoLivre->prox = sentinela;

        // Atualizando o ultimo elemento dos setores
        sentinela->ant->prox = segmentoLivre;
        sentinela->ant = segmentoLivre;
    }

    // Fechando os Arquivos
    fclose(arqOrigem);
}

bool disco_remove(Disco *d, char *nome) // somente o nome do arquivo sem o caminho
{
    NoArquivo *aux = d->arquivos->prox; // Arquivo da posição atual
    NoSetor *livres = d->livres;        // Sentinela das posições livres

    // Indo até p arquivo com o nome recebido por parâmetro
    while (aux != d->arquivos)
    {
        if (strcmp(nome, aux->nome) == 0)
        {
            break;
        }
        aux = aux->prox;
    }

    // Tira o arquivo da lista
    aux->ant->prox = aux->prox;
    aux->prox->ant = aux->ant;

    // Colocando os segmentos na lista de livres
    aux->setores->prox->ant = livres->ant;
    aux->setores->ant->prox = livres;
    livres->ant->prox = aux->setores->prox;
    livres->ant = aux->setores->ant;

    agrupaNos(d);

    return true;
}

bool disco_recupera(Disco *d, char *nome, char *arquivoDestino) // nome arquivo deve conter o caminho absoluto ou relativo do arquivo
{
    FILE *destino;
    NoArquivo *arquivo = d->arquivos->prox; // Recebe o primeiro arquivo da lista

    // Indo até o arquivo que sera recuperado
    while (arquivo != d->arquivos)
    {
        if (strcmp(nome, arquivo->nome) == 0)
        {
            break;
        }
        arquivo = arquivo->prox;
    }

    // CAbrindo os arquivos destino e origem
    destino = fopen(arquivoDestino, "wb");

    // Sentinela dos setores do arquivo
    NoSetor *setor = arquivo->setores->prox;

    long int tam;

    // Percorrendo os setores para copia
    while (setor != arquivo->setores)
    {
        // Recebendo a quantidade de Bytes que serão copiados
        tam = (setor->fim - setor->inicio)+1;

        //  Copiando os Bytes
        fwrite(d->disco + (setor->inicio), tam, 1, destino);

        // Indo para o proximo setor
        setor = setor->prox;
    }

    // Fecha os arquivos
    fclose(destino);
    return true;
}

void imprimeSetoresLivres(Disco *d)
{
    printf("LIVRES: ");
    NoSetor *aux = d->livres->prox;
    while (aux != d->livres)
    {
        printf("[%ld,%ld] ", aux->inicio, aux->fim);
        aux = aux->prox;
    }
    printf("\n");
}

void disco_lista(Disco *d)
{
    printf("-----------------\n");
    imprimeSetoresLivres(d);
    printf("-----------------\n");
    printf("ARQUIVOS: \n");

    NoArquivo *arquivo = d->arquivos->prox;
    while (arquivo != d->arquivos)
    {
        printf("%-30s (%9ld) ", arquivo->nome, arquivo->tam);

        NoSetor *segmento = arquivo->setores->prox;
        while (segmento != arquivo->setores)
        {
            printf("[%ld,%ld] ", segmento->inicio, segmento->fim);
            segmento = segmento->prox;
        }
        printf("\n");
        arquivo = arquivo->prox;
    }
    printf("\n");
}

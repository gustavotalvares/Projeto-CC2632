#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int dia;
    int mes;
    int ano;
}t_data;

typedef struct{
    char categoria[25];
} t_categoria;

typedef struct{
    t_data data;
    char descricao[80];
    int categoria;
    float valor;
} t_transacao;

typedef struct{
    float saldo;
    int qnt_categorias;
    int qnt_transacoes;
    t_categoria *categorias;
    t_transacao *transacoes;
} t_arquivo;

void lerArquivo(t_arquivo *arquivo){
    // Abre o arquivo no mode leitura binária
    FILE *doc = fopen("arquivo.bin", "rb");

    // Lê saldo e quantidades de categorias e transações
    fread(&arquivo->saldo, sizeof(float), 1, doc);
    fread(&arquivo->qnt_categorias, sizeof(int), 1, doc);
    fread(&arquivo->qnt_transacoes, sizeof(int), 1, doc);

    // Aloca a memória considerada adequada para categorias e transações
    arquivo->categorias = calloc(sizeof(t_categoria), arquivo->qnt_categorias + 5);
    arquivo->transacoes = calloc(sizeof(t_transacao), arquivo->qnt_transacoes + 20);

    // Lê categorias e transações
    fread(arquivo->categorias, sizeof(t_categoria), arquivo->qnt_categorias, doc);
    fread(arquivo->transacoes, sizeof(t_transacao), arquivo->qnt_transacoes, doc);

    //Fecha o arquivo
    fclose(doc);
}

void salvarArquivo(t_arquivo *arquivo){
    // Abre o arquivo no mode escrita binária
    FILE *doc = fopen("arquivo.bin", "wb");

    // Salva o saldo e quantidades de categorias e transações
    fwrite(&arquivo->saldo, sizeof(float), 1, doc);
    fwrite(&arquivo->qnt_categorias, sizeof(int), 1, doc);
    fwrite(&arquivo->qnt_transacoes, sizeof(int), 1, doc);

    // Salva categorias e transações
    fwrite(arquivo->categorias, sizeof(t_categoria), arquivo->qnt_categorias, doc);
    fwrite(arquivo->transacoes, sizeof(t_transacao), arquivo->qnt_transacoes, doc);

    //Fecha o arquivo
    fclose(doc);
}


void criarTransacao(t_arquivo *arquivo){}
void editarTransacao(t_arquivo *arquivo){}
void apagarTransacao(t_arquivo *arquivo){}
void gerarRelatorio(t_arquivo *arquivo){}
void criarCategoria(t_arquivo *arquivo){}
void editarCategoria(t_arquivo *arquivo){}




int main(void){
    t_categoria categorias[] = {"Lazer", "Transporte", "Alimentacao"};
    t_data datas[] = {{20, 9, 2020}, {23, 10, 2020}, {25, 10, 2020}};


    t_transacao transacoes[] = {
        {datas[0], "Uma descricao qualquer", 1, 1548.57},
        {datas[1], "Outra descricao qualquer", 0, 127.12},
        {datas[2], "Mais uma descricao qualquer", 0, 53.30}
    };
    t_arquivo arquivo = {2500, 3, 3, categorias, transacoes};
    salvarArquivo(&arquivo);

    t_arquivo arquivo2;
    lerArquivo(&arquivo2);
    printf("saldo: %.2f\n", arquivo2.saldo);
    printf("qnt_categorias: %d\n", arquivo2.qnt_categorias);
    printf("qnt_transacoes: %d\n", arquivo2.qnt_transacoes);

    printf("Transacoes:\n");
    for(int i = 0; i < arquivo2.qnt_transacoes; i++){
        printf("Transacao %02d\n", i+1);
        //printf("Categoria: %s\n", arquivo2.categorias[arquivo2.transacoes[i].categoria].categoria);
        printf("Descricao: %s\n", arquivo2.transacoes[i].descricao);
        printf("Data: %02d/%02d/%02d\n",
               arquivo2.transacoes[i].data.dia,
               arquivo2.transacoes[i].data.mes,
               arquivo2.transacoes[i].data.ano);
        printf("Valor: %.2f\n", arquivo2.transacoes[i].valor);
        printf("\n");
    }

return 0;
}

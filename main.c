#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int dia;
    int mes;
    int ano;
} t_data;

typedef struct{
    char categoria[25];
} t_categoria;

typedef struct{
    int tipo; // 1 = crédito | -1 = débito
    t_data data;
    char descricao[100];
    int categoria;
    float valor;
} t_transacao;

typedef struct{
    float saldo;
    int qnt_categorias;
    int qnt_transacoes;
    t_categoria* categorias;
    t_transacao* transacoes;
} t_arquivo;

void lerArquivo(t_arquivo* arquivo){
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

void salvarArquivo(t_arquivo* arquivo){
    // Abre o arquivo no mode escrita binária
    FILE* doc = fopen("arquivo.bin", "wb");

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

t_data trataData(char* entrada){
    t_data data = {
        (((entrada[0]-48)*10) + (entrada[1]-48)),
        (((entrada[3]-48)*10) + (entrada[4]-48)),
        (((entrada[6]-48)*1000) + ((entrada[7]-48)*100) + ((entrada[8]-48)*10) + (entrada[9]-48))};

    if(data.dia > 31 ||
       data.mes > 12 ||
       data.ano < 1900 ||
       data.ano > 2100 ||
       entrada[2] != '/' ||
       entrada[5] != '/' ||
       entrada[10] != '\0'){
            data.dia = 0;
            data.mes = 0;
            data.ano = 0;}
    return data;
}

void tomaTipo(t_transacao* transacao){
    int entrada = -1;
    while(entrada != 1 && entrada != 2){
        printf("1. Credito\n2. Debito\n");
        printf("Tipo da transacao: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    if(entrada == 1){
        transacao->tipo = 1;
    }else if(entrada == 2){
        transacao->tipo = -1;
    }
    fflush(stdin);
}

void tomaData(t_transacao* transacao){
    t_data data = {0, 0, 0};
    char entrada[11];
    while(data.dia == 0){
        printf("Formato de entrada: DD/MM/AAAA\n");
        printf("Data: ");
        fflush(stdin);
        fgets(entrada, 11, stdin);
        data = trataData(entrada);
        printf("\n");
    }
    fflush(stdin);
    transacao->data = data;
}

void tomaDescricao(t_transacao* transacao){
    char entrada[101];
    printf("Maximo de 100 caracteres\n");
    printf("Descricao: ");
    fflush(stdin);
    fgets(entrada, 101, stdin);
    strcpy(transacao->descricao, entrada);
    fflush(stdin);
    printf("\n");
}

void tomaCategoria(t_transacao* transacao, t_arquivo* arquivo){
    char entrada[2];
    int categoria = -1;
    while(categoria < 0 || categoria >= arquivo->qnt_categorias){
        printf("Digite o numero da categoria\n");
        for(int i = 0; i < arquivo->qnt_categorias; i++){
            printf("%d. %s\n", i+1, arquivo->categorias[i]);
        }
        printf("Categoria: ");
        fflush(stdin);
        fgets(entrada, 2, stdin);
        printf("\n");
        categoria = entrada[0]-49;
    }
    fflush(stdin);
    transacao->categoria = categoria;
}

void tomaValor(t_transacao* transacao){
    float entrada = 0;
    printf("Valor: ");
    fflush(stdin);
    scanf("%f", &entrada);
    fflush(stdin);
    printf("\n");
    transacao->valor = entrada;
}

void apresentaTransacao(t_transacao* transacao, t_arquivo* arquivo){
    printf("Resumo da transacao:\n");
    printf("Data: %02d/%02d/%04d\n", transacao->data.dia, transacao->data.mes, transacao->data.ano);
    transacao->tipo == 1 ? printf("Operacao de credito\n") : printf("Operacao de debito\n");
    printf("Descricao: %s", transacao->descricao);
    printf("Categoria: %s\n", arquivo->categorias[transacao->categoria]);
    printf("Valor: R$%.2f\n", transacao->valor);
    printf("\n");
}

void incluiTransacao(t_transacao* transacao, t_arquivo* arquivo){
    transacao->valor *= transacao->tipo;
    arquivo->transacoes[arquivo->qnt_transacoes] = *transacao;
    arquivo->qnt_transacoes++;
    arquivo->saldo += transacao->valor;
    salvarArquivo(arquivo);
    printf("Transacao adicionada com sucesso!\n\n");
    fflush(stdin);
}

//Declaração da função confirmaTransação() para resolver o warning de função não declarada
void confirmaTransacao();

void editaTransacao(t_transacao* transacao, t_arquivo* arquivo){
    int entrada = 0;
    while(entrada < 1 || entrada > 6){
        printf("Selecione qual informacao voce deseja editar:\n");
        printf("1. Tipo (credito ou debito) \n");
        printf("2. Data \n");
        printf("3. Descricao \n");
        printf("4. Categoria \n");
        printf("5. Valor \n");
        printf("6. Voltar");
        printf("Indique a opcao desejada: ");
        scanf("%d", &entrada);
        printf("\n");
        switch(entrada){
        case 1:
            tomaTipo(transacao);
            break;
        case 2:
            tomaData(transacao);
            break;
        case 3:
            tomaDescricao(transacao);
            break;
        case 4:
            tomaCategoria(transacao, arquivo);
            break;
        case 5:
            tomaValor(transacao);
            break;
        case 6:
            break;
        default:
            break;
        }
    }
    apresentaTransacao(transacao, arquivo);
    confirmaTransacao(transacao, arquivo);
}


void confirmaTransacao(t_transacao* transacao, t_arquivo* arquivo){
    int entrada = 0;
    while(entrada < 1 || entrada > 3){
        printf("Confirmar transacao?\n");
        printf("1. Confirmar\n");
        printf("2. Editar\n");
        printf("3. cancelar\n");
        printf("Indique a opcao desejada: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    switch(entrada){
    case 1:
        incluiTransacao(transacao, arquivo);
        break;
    case 2:
        editaTransacao(transacao, arquivo);
        break;
    case 3:
    default:
        break;
    }
}

void criarTransacao(t_arquivo* arquivo){
    t_transacao transacao;
    tomaTipo(&transacao);
    tomaData(&transacao);
    tomaDescricao(&transacao);
    tomaCategoria(&transacao, arquivo);
    tomaValor(&transacao);
    apresentaTransacao(&transacao, arquivo);
    confirmaTransacao(&transacao, arquivo);
}

void trocaTransacoes(t_arquivo* arquivo, int j){
    t_transacao temp = arquivo->transacoes[j];
    arquivo->transacoes[j] = arquivo->transacoes[j+1];
    arquivo->transacoes[j+1] = temp;
}

int comparaDatasTransacoes(t_transacao t1, t_transacao t2){
    //t1 mais recente que t2    +1
    //t1 mesma data que t2      0
    //t1 mais antigo que t2     -1
    if(t1.data.ano > t2.data.ano){
        return 1;
    }else if(t1.data.ano < t2.data.ano){
        return -1;
    }else{
        if(t1.data.mes > t2.data.mes){
            return 1;
        }else if(t1.data.mes < t2.data.mes){
            return -1;
        }else{
            if(t1.data.dia > t2.data.dia){
                return 1;
            }else if(t1.data.dia < t2.data.dia){
                return -1;
            }else{
                return 0;
            }
        }
    }
}

void ordenaTransacoes(t_arquivo* arquivo){
    for(int i = 0; i < arquivo->qnt_transacoes; i++){
        for(int j = 0; j < arquivo->qnt_transacoes-1; j++){
            if(comparaDatasTransacoes(arquivo->transacoes[j], arquivo->transacoes[j+1]) == 1){
                trocaTransacoes(arquivo, j);
            }
        }
    }
    salvarArquivo(arquivo);
}

void atualizarTransacao(t_arquivo* arquivo){}
void apagarTransacao(t_arquivo* arquivo){}
void gerarRelatorio(t_arquivo* arquivo){}
void criarCategoria(t_arquivo* arquivo){}
void editarCategoria(t_arquivo* arquivo){}


int main(void){
    t_arquivo arquivo;
    lerArquivo(&arquivo);
    printf("Saldo: %.2f\n\n", arquivo.saldo);
    for(int i = 0; i < arquivo.qnt_transacoes; i++){
        printf("Data: %02d/%02d/%04d\n", arquivo.transacoes[i].data.dia, arquivo.transacoes[i].data.mes, arquivo.transacoes[i].data.ano);
        printf("Valor: %.2f\n\n", arquivo.transacoes[i].valor);
    }
    //ordenaTransacoes(&arquivo);
return 0;
}

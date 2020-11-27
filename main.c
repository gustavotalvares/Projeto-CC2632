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
    arquivo->categorias = calloc(sizeof(t_categoria), arquivo->qnt_categorias + 2);
    arquivo->transacoes = calloc(sizeof(t_transacao), arquivo->qnt_transacoes + 2);

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

void imprimeTransacao(t_transacao transacao, int id, t_arquivo* arquivo){
    if(id != 0){
        printf("ID:           %05d\n", id);
    }
    transacao.tipo == 1 ? printf("Tipo:         credito\n") : printf("Tipo:         debito\n");
    printf("Data:         %02d/%02d/%02d\n", transacao.data.dia, transacao.data.mes, transacao.data.ano);
    printf("Descricao:    %s", transacao.descricao);
    printf("Categoria:    %s\n", arquivo->categorias[transacao.categoria]);
    printf("Valor:        %.2f\n", transacao.valor);
    printf("\n");
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

void tomaData(t_data* data){
    t_data data_aux = {0, 0, 0};
    char entrada[11];
    while(data_aux.dia == 0){
        printf("Formato de entrada: DD/MM/AAAA\n");
        printf("Data: ");
        fflush(stdin);
        fgets(entrada, 11, stdin);
        data_aux = trataData(entrada);
        printf("\n");
    }
    fflush(stdin);
    *data = data_aux;
}

void tomaDescricao(t_transacao* transacao){
    char entrada[101] = " ";
    printf("Maximo de 100 caracteres\n");
    printf("Descricao: ");
    fflush(stdin);
    fgets(entrada, 101, stdin);
    strcpy(transacao->descricao, entrada);
    fflush(stdin);
    printf("\n");
}

void imprimeCategorias(t_arquivo* arquivo){
    for(int i = 0; i < arquivo->qnt_categorias; i++){
        printf("%d. %s\n", i+1, arquivo->categorias[i]);
    }
}

void tomaCategoria(t_transacao* transacao, t_arquivo* arquivo){
    char entrada[2];
    int categoria = -1;
    while(categoria < 0 || categoria >= arquivo->qnt_categorias){
        printf("Digite o numero da categoria\n");
        imprimeCategorias(arquivo);
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

void incluiTransacao(t_transacao* transacao, t_arquivo* arquivo){
    //Insere a transacao no arquivo
    transacao->valor *= transacao->tipo;
    arquivo->transacoes[arquivo->qnt_transacoes] = *transacao;
    arquivo->qnt_transacoes++;
    arquivo->saldo += transacao->valor;
    //Realoca memória para o vetor transacoes
    arquivo->transacoes = realloc(arquivo->transacoes, (arquivo->qnt_transacoes + 2) * sizeof(t_transacao));
    printf("Transacao adicionada com sucesso!\n\n");
    fflush(stdin);
}

//Declaração da função confirmaTransação() para resolver o warning de função não declarada
void confirmaTransacao();
void ordenaTransacoes();

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
            tomaData(&transacao->data);
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
    imprimeTransacao(*transacao, 0, arquivo);
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
    tomaData(&transacao.data);
    tomaDescricao(&transacao);
    tomaCategoria(&transacao, arquivo);
    tomaValor(&transacao);
    imprimeTransacao(transacao, 0, arquivo);
    confirmaTransacao(&transacao, arquivo);
    ordenaTransacoes(arquivo);
    salvarArquivo(arquivo);
}

void trocaTransacoes(t_arquivo* arquivo, int j){
    t_transacao temp = arquivo->transacoes[j];
    arquivo->transacoes[j] = arquivo->transacoes[j+1];
    arquivo->transacoes[j+1] = temp;
}

int comparaDatas(t_data data1, t_data data2){
    //data1 mais recente que data2    +1
    //data1 mesma data que data2      0
    //data1 mais antigo que data2     -1
    if(data1.ano > data2.ano){
        return 1;
    }else if(data1.ano < data2.ano){
        return -1;
    }else{
        if(data1.mes > data2.mes){
            return 1;
        }else if(data1.mes < data2.mes){
            return -1;
        }else{
            if(data1.dia > data2.dia){
                return 1;
            }else if(data1.dia < data2.dia){
                return -1;
            }else{
                return 0;
            }
        }
    }
}

void ordenaTransacoes(t_arquivo* arquivo){
    int continuar = 1;
    while(continuar == 1){
        continuar = 0;
        for(int j = arquivo->qnt_transacoes-1; j > 0 ; j--){
            if(comparaDatas(arquivo->transacoes[j-1].data, arquivo->transacoes[j].data) == 1){
                trocaTransacoes(arquivo, j-1);
                continuar = 1;
            }
        }
    }
    salvarArquivo(arquivo);
}

void imprimeTransacoes(t_transacao* transacoes, int qnt_transacoes, t_arquivo* arquivo){
    for(int i = 0; i < qnt_transacoes; i++){
        imprimeTransacao(transacoes[i], i+1, arquivo);
    }
}

void tomaFiltroDatas(t_data* data_inicial, t_data* data_final){
    printf("Indique a data incial do periodo desejado:\n");
    tomaData(data_inicial);
    printf("Indique a data final do periodo desejado:\n");
    tomaData(data_final);
}

int verificaData(t_data* data_inicial, t_data* data_final, t_data* data){
    if(comparaDatas(*data, *data_inicial) >= 0 && comparaDatas(*data, *data_final) <= 0){
        return 1;
    }else{
        return 0;
    }
}

int filtraTransacoes(t_transacao** transacoes, int** id_transacoes, int categoria, t_data data_i, t_data data_f, int tipo, t_arquivo* arquivo){
    int qnt_transacoes = 0;
    if(categoria != -1 && tipo != 0){
        for(int i = 0; i < arquivo->qnt_transacoes; i++){
            if(verificaData(&data_i, &data_f, &arquivo->transacoes[i].data) &&
            arquivo->transacoes[i].categoria == categoria &&
            arquivo->transacoes[i].tipo == tipo){
                qnt_transacoes++;
            }
        }
        *transacoes = calloc(sizeof(t_transacao), qnt_transacoes);
        *id_transacoes = calloc(sizeof(int), qnt_transacoes);
        int j = 0;
        for(int i = 0; i < arquivo->qnt_transacoes; i++){
            if(verificaData(&data_i, &data_f, &arquivo->transacoes[i].data) &&
            arquivo->transacoes[i].categoria == categoria &&
            arquivo->transacoes[i].tipo == tipo){
                (*transacoes)[j] = arquivo->transacoes[i];
                (*id_transacoes)[j] = i+1;
                j++;
            }
        }
    }else if(categoria != -1){
        for(int i = 0; i < arquivo->qnt_transacoes; i++){
            if(verificaData(&data_i, &data_f, &arquivo->transacoes[i].data) &&
            arquivo->transacoes[i].categoria == categoria){
                qnt_transacoes++;
            }
        }
        *transacoes = calloc(sizeof(t_transacao), qnt_transacoes);
        *id_transacoes = calloc(sizeof(int), qnt_transacoes);
        int j = 0;
        for(int i = 0; i < arquivo->qnt_transacoes; i++){
            if(verificaData(&data_i, &data_f, &arquivo->transacoes[i].data) &&
            arquivo->transacoes[i].categoria == categoria){
                (*transacoes)[j] = arquivo->transacoes[i];
                (*id_transacoes)[j] = i+1;
                j++;
            }
        }
    }else if(tipo != 0){
        for(int i = 0; i < arquivo->qnt_transacoes; i++){
            if(verificaData(&data_i, &data_f, &arquivo->transacoes[i].data) &&
            arquivo->transacoes[i].tipo == tipo){
                qnt_transacoes++;
            }
        }
        *transacoes = calloc(sizeof(t_transacao), qnt_transacoes);
        *id_transacoes = calloc(sizeof(int), qnt_transacoes);
        int j = 0;
        for(int i = 0; i < arquivo->qnt_transacoes; i++){
            if(verificaData(&data_i, &data_f, &arquivo->transacoes[i].data) &&
            arquivo->transacoes[i].tipo == tipo){
                (*transacoes)[j] = arquivo->transacoes[i];
                (*id_transacoes)[j] = i+1;
                j++;
            }
        }
    }else{
        for(int i = 0; i < arquivo->qnt_transacoes; i++){
            if(verificaData(&data_i, &data_f, &arquivo->transacoes[i].data)){
                qnt_transacoes++;
                printf("Entrou uma\n");
            }
        }
        *transacoes = calloc(sizeof(t_transacao), qnt_transacoes);
        *id_transacoes = calloc(sizeof(int), qnt_transacoes);
        int j = 0;
        for(int i = 0; i < arquivo->qnt_transacoes; i++){
            if(verificaData(&data_i, &data_f, &arquivo->transacoes[i].data)){
                (*transacoes)[j] = arquivo->transacoes[i];
                (*id_transacoes)[j] = i+1;
                j++;
            }
        }
    }
    return qnt_transacoes;
}

void vizualizarTransacoes(t_arquivo* arquivo){
    // define datas iniciais e finais
    t_data data_i = {1, 1, 0};
    t_data data_f = {31, 12, 3000};
    int entrada = 0;
    while(entrada < 1 || entrada > 2){
        printf("Voce deseja definir um filtro de data?\n");
        printf("1. Sim\n");
        printf("2. Nao\n");
        printf("Insira a opcao desejada: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    if(entrada == 1){
        tomaFiltroDatas(&data_i, &data_f);
    }

    // define filtro de categoria
    int categoria = -1;
    entrada = -1;
    while(entrada < 0 || entrada > arquivo->qnt_categorias){
        printf("Voce gostaria de selecionar uma categoria especifica?\n");
        printf("0. Nao selecionar categoria\n");
        imprimeCategorias(arquivo);
        printf("Insira a opcao desejada: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    categoria = entrada-1;

    // define filtro de tipo
    int tipo = 0;
    entrada = -1;
    while(entrada < 0 || entrada > 2){
        printf("Voce gostaria de selecionar um tipo especifico de transacao?\n");
        printf("0. Nao selecionar tipo\n");
        printf("1. Credito\n");
        printf("2. Debito\n");
        printf("Insira a opcao desejada: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    if(entrada == 2){
        tipo = -1;
    }else{
        tipo = entrada;
    }

    printf("Filtros criados\n");
    printf("categoria: %d\n", categoria);
    printf("Tipo: %d\n", tipo);

    //Cria os ponteiros transacoes e id_transacoes
    t_transacao* transacoes;
    int* id_transacoes;

    printf("Ponteiros criados\n");
    //Chama a função filtraTransacoes()
    int qnt_transacoes = filtraTransacoes(&transacoes, &id_transacoes, categoria, data_i, data_f, tipo, arquivo);
    printf("%d transacoes filtradas\n", qnt_transacoes);

    //Imprime as transacoes
    for(int i = 0; i < qnt_transacoes; i++){
        imprimeTransacao(transacoes[i], id_transacoes[i], arquivo);
    }
    printf("%d transacoes impressas\n", qnt_transacoes);
}

void atualizarTransacao(t_arquivo* arquivo){
    //Define a transacao a ser editada
    int entrada = -1;
    while(entrada < 0){
        printf("Editar Transacao\n");
        printf("Entre com o ID da transacao: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    int id_transacao = entrada - 1;

    t_transacao transacao = arquivo->transacoes[id_transacao];
    imprimeTransacao(transacao, entrada, arquivo);

    while(entrada < 0 || entrada > 5){
        printf("Selecione qual informacao voce deseja editar:\n");
        printf("1. Tipo (credito ou debito) \n");
        printf("2. Data \n");
        printf("3. Descricao \n");
        printf("4. Categoria \n");
        printf("5. Valor \n");
        printf("0. Cancelar e voltar ao menu inicial\n");
        printf("Indique a opcao desejada: ");
        scanf("%d", &entrada);
        printf("\n");
    }
    switch(entrada){
    case 1:
        tomaTipo(&transacao);
        break;
    case 2:
        tomaData(&transacao.data);
        break;
    case 3:
        tomaDescricao(&transacao);
        break;
    case 4:
        tomaCategoria(&transacao, arquivo);
        break;
    case 5:
        tomaValor(&transacao);
        break;
    case 0:
        break;
    default:
        break;
    }
    if(transacao.tipo != arquivo->transacoes[id_transacao].tipo){
        transacao.valor *= -1;
    }
    if(transacao.valor != arquivo->transacoes[id_transacao].valor){
        arquivo->saldo += transacao.valor - arquivo->transacoes[id_transacao].valor;
    }

    if(comparaDatas(transacao.data, arquivo->transacoes[id_transacao].data) != 0){
        arquivo->transacoes[id_transacao] = transacao;
        ordenaTransacoes(arquivo);
    }else{
        arquivo->transacoes[id_transacao] = transacao;
    }

    if(entrada != 0){
        salvarArquivo(arquivo);
        printf("Transacao alterada com sucesso!\n\n");
    }
}
void apagarTransacao(t_arquivo* arquivo){}
//gerarRelatorio pode ser chamado dentro do visualizarTransacoes()
void gerarRelatorio(t_arquivo* arquivo){}
void criarCategoria(t_arquivo* arquivo){}
void editarCategoria(t_arquivo* arquivo){}

void resetar(t_arquivo* arquivo){
    t_categoria categorias[] = {"Trabalho", "Transporte", "Alimentacao", "Lazer", "Outros"};
    arquivo->categorias = categorias;
    arquivo->qnt_categorias = 5;
    arquivo->qnt_transacoes = 0;
    arquivo->saldo = 0;
    salvarArquivo(arquivo);
}

int main(void){
    t_arquivo arquivo;
    lerArquivo(&arquivo);
    while(1){
        printf("Saldo: %.2f\n\n", arquivo.saldo);
        imprimeTransacoes(arquivo.transacoes, arquivo.qnt_transacoes, &arquivo);
        atualizarTransacao(&arquivo);
    }
    //vizualizarTransacoes(&arquivo);
    //resetar(&arquivo);
return 0;
}

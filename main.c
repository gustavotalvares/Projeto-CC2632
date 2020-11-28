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
    printf("Descricao:    %s\n", transacao.descricao);
    printf("Categoria:    %s\n", arquivo->categorias[transacao.categoria]);
    printf("Valor:        R$%.2f\n", transacao.valor * transacao.tipo);
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
void removeEnter();
void tomaDescricao(t_transacao* transacao){
    char entrada[101] = " ";
    printf("Maximo de 100 caracteres\n");
    printf("Descricao: ");
    fflush(stdin);
    fgets(entrada, 101, stdin);
    strcpy(transacao->descricao, entrada);
    removeEnter(transacao->descricao, 101);
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
    transacao->valor = entrada*transacao->tipo;
}

void incluiTransacao(t_transacao* transacao, t_arquivo* arquivo){
    //Insere a transacao no arquivo
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

    //Cria os ponteiros transacoes e id_transacoes
    t_transacao* transacoes;
    int* id_transacoes;

    //Chama a função filtraTransacoes()
    int qnt_transacoes = filtraTransacoes(&transacoes, &id_transacoes, categoria, data_i, data_f, tipo, arquivo);

    //Imprime as transacoes
    for(int i = 0; i < qnt_transacoes; i++){
        imprimeTransacao(transacoes[i], id_transacoes[i], arquivo);
    }
}

void atualizarTransacao(t_arquivo* arquivo){
    //Define a transacao a ser editada
    int entrada = -1;
    while(entrada < 0 || entrada > arquivo->qnt_transacoes){
        printf("Editar Transacao\n");
        printf("Entre com o ID da transacao: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    int id_transacao = entrada - 1;

    t_transacao transacao = arquivo->transacoes[id_transacao];
    imprimeTransacao(transacao, entrada, arquivo);

    entrada = -1;
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
    if(transacao.valor != arquivo->transacoes[id_transacao].valor){
        arquivo->saldo += transacao.valor - arquivo->transacoes[id_transacao].valor;
    }
    if(transacao.tipo != arquivo->transacoes[id_transacao].tipo){
        transacao.valor *= -1;
        arquivo->saldo += 2*transacao.valor;
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
void apagarTransacao(t_arquivo* arquivo){
    int entrada = 0;
    while(entrada < 1 || entrada > arquivo->qnt_transacoes){
        printf("Apagar transacao\n");
        printf("Entre com o ID da transacao: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    int id_transacao = entrada - 1;
    imprimeTransacao(arquivo->transacoes[id_transacao], entrada, arquivo);
    entrada = 0;
    while(entrada < 1 || entrada >2){
        printf("Essa acao é irreversivel, voce tem certeza que deseja\n");
        printf("remover a transacao %05d dos registros?\n", id_transacao+1);
        printf("1. Sim, remover transacao\n");
        printf("2. Nao, cancelar e voltar ao menu inicial\n");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    fflush(stdin);
    if(entrada == 1){
        arquivo->transacoes[id_transacao].data.ano = 5000;
        ordenaTransacoes(arquivo);
        arquivo->saldo -= arquivo->transacoes[id_transacao].valor;
        arquivo->qnt_transacoes--;
        salvarArquivo(arquivo);
        printf("Transacao removida com sucesso!\n");
        printf("\n");
    }

}

void confirmaCategoria(t_categoria* categoria, t_arquivo* arquivo){
    int entrada = 0;
    while(entrada < 1 || entrada > 2){
        printf("Confirmar criacao da categoria?\n");
        printf("1. Confirmar\n");
        printf("2. Cancelar\n");
        printf("\n");
        printf("Insira a opcao desejada: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
        if(entrada == 1){
            arquivo->categorias = realloc(arquivo->categorias, sizeof(arquivo->qnt_categorias + 2) * sizeof(t_categoria));
            arquivo->categorias[arquivo->qnt_categorias]  = *categoria;
            arquivo->qnt_categorias++;
            salvarArquivo(arquivo);
            printf("Categoria criada com sucesso!");
            printf("\n");
        }if(entrada == 2){
            printf("Criacao de categoria cancelada.");
            printf("\n");
        }
    }


}

void removeEnter(char* string, int n){
    for(int i = 0; i < n; i++){
        if(string[i] == '\n'){
            string[i] = '\0';
        }
    }
}

void tomaNomeCategoria(t_categoria* categoria, t_arquivo* arquivo){
        printf("Obs.: max. 25 caracteres\n");
        printf("Nome da categoria: ");
        fgets(categoria->categoria, 25, stdin);
        removeEnter(categoria->categoria, 25);
        printf("\n");
        confirmaCategoria(categoria, arquivo);
}

void criarCategoria(t_arquivo* arquivo){
    t_categoria categoria;
    char entrada[25];
    printf("Atencao, evite criar muitas categorias\n");
    printf("pois nao sera possivel excluir qualquer\n");
    printf("categoria ser apos criada.\n");
    printf("\n");
    while(entrada[0] != '1' && entrada[0] != '2'){
        printf("1. Continuar\n");
        printf("2. Cancelar e voltar ao menu inicial\n");
        printf("Insira a opcao desejada: ");
        fflush(stdin);
        fgets(entrada, 2, stdin);
    }
    fflush(stdin);
    printf("\n");
    if(entrada[0] == '1'){
        tomaNomeCategoria(&categoria, arquivo);
    }
}

void editarCategoria(t_arquivo* arquivo){
    int entrada = 0;
    printf("Editar categoria\n");
    while(entrada < 1 || entrada > arquivo->qnt_categorias){
        printf("Qual categoria voce deseja editar?\n");
        imprimeCategorias(arquivo);
        printf("Insira a opcao desejada: ");
        fflush(stdin);
        scanf("%d", &entrada);
        printf("\n");
    }
    fflush(stdin);
    char novoNome[25];
    printf("Insira o novo nome da categoria: ");
    fgets(novoNome, 25, stdin);
    removeEnter(novoNome, 25);
    strcpy(arquivo->categorias[entrada-1].categoria, novoNome);
    printf("\n");
    printf("Categoria alterada com sucesso!\n");
    salvarArquivo(arquivo);
    printf("\n");
}

void insereCC(FILE* relatorio){
    FILE* css = fopen("CSStemplate.txt", "r");
    char c;
    while((c = fgetc(css)) != EOF){
        fputc(c, relatorio);
    }
}

void geraHeader(FILE* relatorio){
    fprintf(relatorio, "<head>\n");
    fprintf(relatorio, "<meta charset=\"UTF-8\">\n");
    fprintf(relatorio, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n")
    fprintf(relatorio, "<title>Relatório</title>\n");
    insereCSS(relatorio);
    fprintf(relatorio, "</head>\n");
}

void geraRelatorio(t_transacao* transacoes, int n, t_arquivo* arquivo){
    FILE* relatorio = fopen("relatorio.txt", "w");
        fprintf(relatorio, "!DOCTYPE html\n");
        fprintf(relatorio, "<html>\n");
        geraHeader(relatorio);
        fprintf(relatorio, "<body>\n");
        geraListaTransacoes(relatorio, transacoes, n, arquivo);
        fprintf("</body>\n");
        fprintf("</html>\n");
        fclose(relatorio);
}

void sair(t_arquivo* arquivo){
    free(arquivo->categorias);
    free(arquivo->transacoes);
    exit(0);
}

void resetar(t_arquivo* arquivo){
    t_categoria categorias[] = {"Trabalho", "Transporte", "Alimentacao", "Lazer", "Outros"};
    arquivo->categorias = categorias;
    arquivo->qnt_categorias = 5;
    arquivo->qnt_transacoes = 0;
    arquivo->saldo = 0;
    salvarArquivo(arquivo);
    free(arquivo->categorias);
    free(arquivo->transacoes);
    lerArquivo(arquivo);
}

int main(void){
    t_arquivo arquivo;
    lerArquivo(&arquivo);
    while(1){
        printf("########################################\n");
        printf("##                MENU                ##\n");
        printf("########################################\n");
        printf("\n");
        printf("Saldo: R$%.2f\n", arquivo.saldo);
        printf("\n");
        printf("1. Adicionar transacao\n");
        printf("2. Visualizar transacoes\n");
        printf("3. Editar transacao\n");
        printf("4. Apagar transacao\n");
        printf("5. Criar categoria\n");
        printf("6. Editar categoria\n");
        printf("9. Resetar programa\n");
        printf("0. Sair\n");
        printf("\n");
        printf("Entre com a operacao desejada: ");
        fflush(stdin);
        int escolha;
        scanf("%d", &escolha);
        fflush(stdin);
        printf("\n");
        switch(escolha){
        case 1:
            printf("########################################\n");
            printf("##        ADICIONAR TRANSACAO         ##\n");
            printf("########################################\n");
            printf("\n");
            criarTransacao(&arquivo);
            break;
        case 2:
            printf("########################################\n");
            printf("##       VISUALIZAR TRANSACOES        ##\n");
            printf("########################################\n");
            printf("\n");
            vizualizarTransacoes(&arquivo);
            break;
        case 3:
            printf("########################################\n");
            printf("##          EDITAR TRANSACAO          ##\n");
            printf("########################################\n");
            printf("\n");
            atualizarTransacao(&arquivo);
            break;
        case 4:
            printf("########################################\n");
            printf("##          APAGAR TRANSACAO          ##\n");
            printf("########################################\n");
            printf("\n");
            apagarTransacao(&arquivo);
            break;
        case 5:
            printf("########################################\n");
            printf("##          CRIAR CATEGORIA           ##\n");
            printf("########################################\n");
            printf("\n");
            criarCategoria(&arquivo);
            break;
        case 6:
            printf("########################################\n");
            printf("##          EDITAR CATEGORIA          ##\n");
            printf("########################################\n");
            printf("\n");
            editarCategoria(&arquivo);
            break;
        case 9:
            resetar(&arquivo);
            break;
        case 0:
            sair(&arquivo);
            break;
        default:
            printf("Operacao invalida, tente novamente\n");
            printf("\n");
            break;
        }

    }
    //vizualizarTransacoes(&arquivo);
    //resetar(&arquivo);
return 0;
}

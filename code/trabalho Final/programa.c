#include <stdio.h>
#include <stdlib.h>
#include "queue.c"
#include <stdbool.h>

#define N_MAXIMO_DE_INSTRUCOES 30
#define N_MAXIMO_DE_PROCESSOS 30
#define MAX_MEMORIA 300

int disk;
int memoria[MAX_MEMORIA];
int apontadorDaUltimaAlocacao = 0;

struct pcb {
    int id;
    int pc;
    short estado;
};

struct pcb *novoPcb(int id, int pc, short estado) {
    struct pcb *pcb = malloc(sizeof(struct pcb));
    pcb->id = id;
    pcb->pc = pc;
    pcb->estado = estado; //-1 - n introduzido, 0 -NEW, 1 - WAIT, 2 - RUN, 3 - BLOCK, 4 - EXIT
    return pcb;
}

struct processo {
    int instante;
    int quantum;
    int tempo_que_precisa_de_ficar_em_block;
    int codigo[N_MAXIMO_DE_INSTRUCOES * 3];
    int maxPc;
    int posicaoInicial;
    int posicaoFinal;
    struct pcb *pcb;
};

struct processo *novoProcesso(int instante, int codigo[N_MAXIMO_DE_INSTRUCOES * 3], struct pcb *pcb, int size) {
    struct processo *processo = malloc(sizeof(struct processo));
    int i;
    processo->instante = instante;
    for (i = 0; i < size; i++) {
        processo->codigo[i] = codigo[i];
    }
    processo->posicaoInicial = 0;
    processo->posicaoFinal = 0;
    processo->quantum = 0;
    processo->tempo_que_precisa_de_ficar_em_block = 3;
    processo->pcb = pcb;
    processo->maxPc = size / 3;
    return processo;
}

void limparExit(int *processo_em_exit, int *n_processos_corridos, struct processo *processos[]) {

    for (int x = processos[*processo_em_exit]->posicaoInicial; x <= processos[*processo_em_exit]->posicaoFinal; x++) {
        memoria[x] = -1;
    }
    if (*processo_em_exit != -1) {
        processos[*processo_em_exit]->pcb->estado = -2;
        //printf("id: %d mudei no instante %d para lixo\n", processos[processo_em_exit]->pcb->id, timer);
        (*processo_em_exit) = -1;
        (*n_processos_corridos)++;
    }

}

void percorrerBlock(queue *block, queue *wait, struct processo *processos[], int first, int last) {
    int inst, variavel, prox;
    for (int i = first; i <= last; i++) {
        processos[block->Q[i]]->tempo_que_precisa_de_ficar_em_block--;
        if (processos[block->Q[i]]->tempo_que_precisa_de_ficar_em_block == 0) {
            //                        printf("id: %d mudei no instante %d para wait de block\n", processos[block->Q[i]]->pcb->id,
            //                        timer);
            inst = memoria[processos[i]->posicaoInicial + ((processos[i]->pcb->pc * 3) - 3) + 10];
            if (inst == 8) {
                variavel = memoria[processos[i]->posicaoInicial + ((processos[i]->pcb->pc * 3) - 2) + 10];
                disk = memoria[variavel];
            } else {
                variavel = memoria[processos[i]->posicaoInicial + ((processos[i]->pcb->pc * 3) - 2) + 10];
                memoria[variavel] = disk;
            }
            prox = dequeue(block);
            if (processos[prox]->pcb->estado != -2) {
                processos[block->Q[i]]->pcb->estado = 1;
                enqueue(prox, wait);
            }
        }
    }
    //TODO configurar posicoa das variaveis para ler/escrever no disco
}

void blockParaWait(queue *block, queue *wait, struct processo *processos[]) {

    if (!isEmpty(block)) {
        if (block->first <= block->last) {
            percorrerBlock(block, wait, processos, block->first, block->last);
        } else {
            percorrerBlock(block, wait, processos, block->first, block->size);
            percorrerBlock(block, wait, processos, 0, block->last);
        }
    }
}

void runParaBlock(int *processo_em_run, queue *block, struct processo *processos[]) {

    processos[*processo_em_run]->pcb->pc++;
//                printf("id: %d mudei no instante %d para block\n", processos[processo_em_run]->pcb->id, timer);
    processos[*processo_em_run]->pcb->estado = 3;
    processos[*processo_em_run]->tempo_que_precisa_de_ficar_em_block = 3;
    enqueue(*processo_em_run, block);
    (*processo_em_run) = -1;

}

void runParaExit(int *processo_em_run, int *processo_em_exit, queue *block, struct processo *processos[]) {

    processos[*processo_em_run]->pcb->pc++;
//                printf("id: %d mudei no instante %d para exit\n", processos[processo_em_run]->pcb->id, timer);
    processos[*processo_em_run]->pcb->estado = 4;
    (*processo_em_exit) = (*processo_em_run);
    enqueue(*processo_em_run, block);
    (*processo_em_run) = -1;
}

void runParaWait(int *processo_em_run, queue *wait, struct processo *processos[]) {

    processos[*processo_em_run]->pcb->estado = 1;
    enqueue(*processo_em_run, wait);
//                printf("id: %d mudei no instante %d para wait\n", processos[processo_em_run]->pcb->id, timer);
    (*processo_em_run) = -1;

}

int obterPosicao(struct processo *processo) { //-1 se nao cabe ,senao, posicao de memoria inicial se cabe
    int espacoNessesario = (processo->maxPc * 3) + 10;
    int inicio;
    int fim;
    int x;
    for (x = apontadorDaUltimaAlocacao; x < MAX_MEMORIA; x++) {
        if (memoria[x] == -1) {
            inicio = x;
        }
        while (x < MAX_MEMORIA && memoria[x] == -1) {
            x++;
        }

        fim = x;
        if (fim - inicio >= espacoNessesario) {
            return inicio;
        }
        x = fim;
    }

    for (x = 0; x < MAX_MEMORIA; x++) {
        if (memoria[x] == -1) {
            inicio = x;
        }
        while (x < MAX_MEMORIA && memoria[x] == -1) {
            x++;
        }

        fim = x;
        if (fim - inicio >= espacoNessesario) {
            return inicio;
        }
        x = fim;
    }

    return -1;
}

void copiarParaMemoria(struct processo *processo, int posicao) {
    processo->posicaoInicial = posicao;
    for (int x = posicao; x < posicao + 10; x++) {
        memoria[x] = 0;
    }
    posicao += 10;
    for (int x = 0; processo->codigo[x] != -1; x++) {
        memoria[posicao] = processo->codigo[x];
        posicao++;
    }
    processo->posicaoFinal = posicao - 1;
    apontadorDaUltimaAlocacao = posicao;
}

void newParaWait(int p_id, queue *wait, struct processo *processos[]) {

    int n_p = 0;
    while ((!isFull(wait)) && (n_p < p_id)) {
        if (processos[n_p]->pcb->estado == 0) {
            int posicao = obterPosicao(processos[n_p]);
            if (posicao != -1) {
                processos[n_p]->pcb->estado = 1;
                enqueue(n_p, wait);
                copiarParaMemoria(processos[n_p], posicao);
//                  printf("id: %d mudei no instante %d para wait\n", processos[n_p]->pcb->id, timer);
            }
        }
        n_p++;
    }
}

void waitParaRun(int *processo_em_run, queue *wait, struct processo *processos[]) {

    if (((*processo_em_run) == -1) && (!isEmpty(wait))) {

        (*processo_em_run) = dequeue(wait);
        processos[*processo_em_run]->pcb->estado = 2; // processo passa para run
        processos[*processo_em_run]->quantum = 4;
//            printf("id: %d mudei no instante %d para run\n", processo_em_run, timer);
    }
}

void receberParaNew(int timer, int p_id, struct processo *processos[]) {

    for (int n_p = 0; n_p < p_id; n_p++) {

        if (processos[n_p]->instante == timer) {
            processos[n_p]->pcb->estado = 0;
//                printf("id: %d mudei no instante %d para new\n", processos[n_p]->pcb->id, timer);
        }
    }
}

void printEstados(int timer, int p_id, struct processo *processos[], int *print, int *fork) {
    printf("T: %3d | ", timer);

//print de todos os processos introduzidos
    for (int n_p = 0; n_p < p_id; n_p++) {
        switch (processos[n_p]->pcb->estado) {
            case -2:
                printf("      | ");
                break;
            case 0:
                printf("NEW   | ");
                break;
            case 1:
                printf("WAIT  | ");
                break;
            case 2:
                printf("RUN   | ");
                break;
            case 3:
                printf("BLOCK | ");
                break;
            case 4:
                printf("EXIT  | ");
                break;
            default:
                break;
        }

    }
    if (*print != -1) {
        printf("  %d", *print);
        *print = -1;
    }
    if (*fork == 1) {
        printf(" falha no fork");
        *fork = 0;
    }
    printf("\n");
}

void debugPrint(int p_id, struct processo *processos[]) {

    printf("\nPROCESSOS\n");
    struct processo *aux;

    for (int i = 0; i < p_id; i++) {
        aux = processos[i];
        printf("%d estado: %d | instante = %d", aux->pcb->id, aux->pcb->estado, aux->instante);
        printf("\n");
    }

    free(aux);
}

int main(void) {

    int instante, p_id = 0, controlo, n_processos_corridos = 0, timer = 0, processo_em_run = -1,
            processo_em_exit = -1, algoParaImprimir = -1, falhaFork = 0;
    struct processo *processos[N_MAXIMO_DE_PROCESSOS] = {0};
    char test;

    //inicializa a memoria a -1 (posicao vazia)
    for (int i = 0; i < MAX_MEMORIA; i++) {
        memoria[i] = -1;
    }

    // le toda a informacao do ficheiro de teste
    while (scanf(" %d", &instante) != EOF) {
        int line[N_MAXIMO_DE_INSTRUCOES * 3] = {0};

        controlo = 0;
        struct pcb *nPCB = novoPcb(p_id, 0, -1);            // novo pcb para cada programa

        while (scanf("%c", &test) == 1) {                   // le a linha toda ate ao \n "codigo"

            if (test == '\n') {
                break;
            }

            //-----------------instrucao--------variavel 1-----------variavel 2
            scanf(" %d %d %d", &line[controlo], &line[controlo + 1], &line[controlo + 2]);
            controlo += 3;

        }
        line[controlo++] = -1;

        processos[p_id] = novoProcesso(instante, line, nPCB, controlo);
        p_id++;
    }

    queue *wait = new_queue();  // fila do wait
    queue *block = new_queue();  // fila do block

    while (1) {

        /*
            IF YOU DON'T WANT THE BLANK LINE MOVE THE IF AFTER THE limparExit() FUNTION.
        */
        //printf("\ttime: %d\n", timer);
        if (n_processos_corridos == p_id)
            break;

        // exit
        limparExit(&processo_em_exit, &n_processos_corridos, processos);

        // block -> wait
        // nao tem restrico para espço
        // block passa para wait direto
        blockParaWait(block, wait, processos);

        // run -> block se for necessario ou run -> exit
        // run -> wait verificar se atingiu o quantum Q=4 (max) se Q=0 passa para wait
        if (processo_em_run != -1) {

            //vai a memoria apanhar oque esta nas posicoes
            int pInicial = processos[processo_em_run]->posicaoInicial;
            int pc = processos[processo_em_run]->pcb->pc * 3;
            int inst = memoria[pInicial + pc + 10],
                    arg1 = memoria[pInicial + pc + 11],
                    arg2 = memoria[pInicial + pc + 12];

            printf("%d %d %d\n", inst, arg1, arg2);

            switch (inst) {
                case 0:                                                 // x1=x2
                    memoria[pInicial + arg1 - 1] = memoria[pInicial + arg2 - 1];
                    break;
                case 1:                                                 // x=n
                    memoria[pInicial + arg1 - 1] = arg2;
                    break;
                case 2:                                                 // x=x+1
                    memoria[pInicial + arg1 - 1]++;
                    break;
                case 3:                                                 // x=x-1
                    memoria[pInicial + arg1 - 1]--;
                    break;
                case 4:                                                 // pc-=N
                    if ((processos[processo_em_run]->pcb->pc -= arg1) >= 0)
                        processos[processo_em_run]->pcb->pc -= arg1;
                    else {
                        printf("Erro no processo %d na instrucao de BACK_N ", processo_em_run);
                        runParaExit(&processo_em_run, &processo_em_exit, block, processos);
                    }
                    break;
                case 5:                                                 // pc+=N
                    if (arg1 > 1) {
                        if ((processos[processo_em_run]->pcb->pc += arg1) <= processos[processo_em_run]->maxPc)
                            processos[processo_em_run]->pcb->pc += arg1;
                        else {
                            printf("Erro no processo %d na instrucao de FORW_N ", processo_em_run);
                            runParaExit(&processo_em_run, &processo_em_exit, block, processos);
                        }
                    }
                    break;
                case 6:                                                 // if x=0, pc+=N else pc++
                    if (arg1 == 0) {
                        if ((processos[processo_em_run]->pcb->pc += arg2) <= processos[processo_em_run]->maxPc)
                            processos[processo_em_run]->pcb->pc += arg2;
                        else {
                            printf("Erro no processo %d na instrucao de FORW_N ", processo_em_run);
                            runParaExit(&processo_em_run, &processo_em_exit, block, processos);
                        }
                    }
                    break;
                case 7:                                                 // fork
                    if (!isFull(wait)) { // TODO verificar se tem memoria sufuciente apos a sua implementacao

                    } else
                        falhaFork = 1;
                    break;
                case 8:                                                 // guardar no disco
                    runParaBlock(&processo_em_run, block, processos);
                    break;
                case 9:                                                 // ler do disco
                    runParaBlock(&processo_em_run, block, processos);
                    break;
                case 10:                                                // imprimir variavel
                    algoParaImprimir = memoria[pInicial + arg1];
                    break;
                default:
                    runParaExit(&processo_em_run, &processo_em_exit, block, processos);
                    break;
            }

            if (processo_em_run != -1)
                processos[processo_em_run]->pcb->pc++;

            processos[processo_em_run]->quantum--;

            if (processos[processo_em_run]->quantum == 0) {

                runParaWait(&processo_em_run, wait, processos);
            }
        }

        // new -> wait
        newParaWait(p_id, wait, processos);

        // wait -> run
        waitParaRun(&processo_em_run, wait, processos);

        // receber o programa no seu instante passando para new
        receberParaNew(timer, p_id, processos);

        //print do instante e do estado de cada processo que já foi introduzido
        printEstados(timer, p_id, processos, &algoParaImprimir, &falhaFork);

        timer++;
        //          SHOW LE MEMORY
        for (int i = 0; i < MAX_MEMORIA; i++) {
            printf("%d", memoria[i]);
        }

    }

    //print de todos os processos introduzidos, descomentar para ver.
    //debugPrint(p_id, processos);

    return 0;
}
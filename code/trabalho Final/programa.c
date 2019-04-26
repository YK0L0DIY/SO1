#include <stdio.h>
#include <stdlib.h>
#include "queue.c"

#define N_MAXIMO_DE_INSTRUCOES 30
#define N_MAXIMO_DE_PROCESSOS 30

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
    int codigo[N_MAXIMO_DE_INSTRUCOES * 3];
    int quantum;
    int tempo_que_precisa_de_ficar_em_block;
    struct pcb *pcb;
};

struct processo *novoProcesso(int instante, int codigo[N_MAXIMO_DE_INSTRUCOES * 3], struct pcb *pcb, int size) {
    struct processo *processo = malloc(sizeof(struct processo));
    int i;
    processo->instante = instante;
    for (i = 0; i < size; i++) {
        processo->codigo[i] = codigo[i];
    }
    processo->quantum = 0;
    processo->tempo_que_precisa_de_ficar_em_block = 3;
    processo->pcb = pcb;
    return processo;
}

int main(void) {
    int instante, p_id = 0, controlo;
    struct processo *processos[N_MAXIMO_DE_PROCESSOS] = {0};
    char test;

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

    int n_processos_corridos = 0, timer = 0, processo_em_run = -1, processo_em_exit = -1;

    while (1) {

//        printf("\ttime: %d\n", timer);
        if (n_processos_corridos == p_id)
            break;

        int n_p;


        // exit
        if (processo_em_exit != -1) {
            processos[processo_em_exit]->pcb->estado = -2;
//            printf("id: %d mudei no instante %d para lixo\n", processos[processo_em_exit]->pcb->id, timer);
            processo_em_exit = -1;
            n_processos_corridos++;
        }

        // block -> wait
        // nao tem restrico para espço
        // block passa para wait direto
        if (!isEmpty(block)) {
            if (block->first <= block->last) {
                for (int i = block->first; i <= block->last; i++) {
                    processos[block->Q[i]]->tempo_que_precisa_de_ficar_em_block--;
                    if (processos[block->Q[i]]->tempo_que_precisa_de_ficar_em_block == 0) {
//                        printf("id: %d mudei no instante %d para wait de block\n", processos[block->Q[i]]->pcb->id,
//                        timer);
                        processos[block->Q[i]]->pcb->estado = 1;
                        enqueue(dequeue(block), wait);
                    }
                }
            } else {
                for (int i = block->first; i <= block->size; i++) {
                    processos[block->Q[i]]->tempo_que_precisa_de_ficar_em_block--;
                    if (processos[block->Q[i]]->tempo_que_precisa_de_ficar_em_block == 0) {
//                        printf("id: %d mudei no instante %d para wait de block\n", processos[block->Q[i]]->pcb->id,
//                        timer);
                        processos[block->Q[i]]->pcb->estado = 1;
                        enqueue(dequeue(block), wait);
                    }
                }
                for (int i = 0; i <= block->last; i++) {
                    processos[block->Q[i]]->tempo_que_precisa_de_ficar_em_block--;
                    if (processos[block->Q[i]]->tempo_que_precisa_de_ficar_em_block == 0) {
//                        printf("id: %d mudei no instante %d para wait de block\n", processos[block->Q[i]]->pcb->id,
//                        timer);
                        processos[block->Q[i]]->pcb->estado = 1;
                        enqueue(dequeue(block), wait);
                    }
                }
            }
        }


        // run -> block se for necessario ou run -> exit
        // run -> wait verificar se atingiu o quantum Q=4 (max) se Q=0 passa para wait
        if (processo_em_run != -1) {
            if ((processos[processo_em_run]->codigo[processos[processo_em_run]->pcb->pc * 3] == 8) ||
                (processos[processo_em_run]->codigo[processos[processo_em_run]->pcb->pc * 3] == 9)) {
                processos[processo_em_run]->pcb->pc++;
//                printf("id: %d mudei no instante %d para block\n", processos[processo_em_run]->pcb->id, timer);
                processos[processo_em_run]->pcb->estado = 3;
                processos[processo_em_run]->tempo_que_precisa_de_ficar_em_block = 3;
                enqueue(processo_em_run, block);
                processo_em_run = -1;

            } else if (processos[processo_em_run]->codigo[processos[processo_em_run]->pcb->pc * 3] == 11) {
                processos[processo_em_run]->pcb->pc++;
//                printf("id: %d mudei no instante %d para exit\n", processos[processo_em_run]->pcb->id, timer);
                processos[processo_em_run]->pcb->estado = 4;
                processo_em_exit = processo_em_run;
                enqueue(processo_em_run, block);
                processo_em_run = -1;
            }

            if (processo_em_run != -1)
                processos[processo_em_run]->pcb->pc++;

            processos[processo_em_run]->quantum--;

            if (processos[processo_em_run]->quantum == 0) {
                processos[processo_em_run]->pcb->estado = 1;
                enqueue(processo_em_run, wait);
//                printf("id: %d mudei no instante %d para wait\n", processos[processo_em_run]->pcb->id, timer);
                processo_em_run = -1;
            }
        }

        // new -> wait
        // posteriormente verificar se tem memoria para o fazer
        n_p = 0;
        while ((!isFull(wait)) && (n_p < p_id)) {
            if (processos[n_p]->pcb->estado == 0) {
                processos[n_p]->pcb->estado = 1;
                enqueue(n_p, wait);
//                printf("id: %d mudei no instante %d para wait\n", processos[n_p]->pcb->id, timer);
            }
            n_p++;
        }

        // wait -> run
        if ((processo_em_run == -1) && (!isEmpty(wait))) {
            processo_em_run = dequeue(wait);
            processos[processo_em_run]->pcb->estado = 2; // processo passa para run
            processos[processo_em_run]->quantum = 4;
//            printf("id: %d mudei no instante %d para run\n", processo_em_run, timer);
        }

        // receber o programa no seu instante passando para new
        // posteriormente ver como organizar os arrays devido aos forks
        for (n_p = 0; n_p < p_id; n_p++) {
            if (processos[n_p]->instante == timer) {
                processos[n_p]->pcb->estado = 0;
//                printf("id: %d mudei no instante %d para new\n", processos[n_p]->pcb->id, timer);
            }
        }

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
        printf("\n");
        timer++;

    }

    //print de todos os processos introduzidos
    int i = 0;
    struct processo *aux;
    while (i < p_id) {
        aux = processos[i];
        printf("%d estado: %d | instante = %d", aux->pcb->id, aux->pcb->estado, aux->instante);
        i++;
        printf("\n");
    }
    free(aux);
}
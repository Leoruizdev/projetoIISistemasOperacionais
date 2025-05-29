#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "estruturas.h"

void simular_acessos(Simulador *sim, int pid, int num_acessos);
void exibir_estatisticas(Simulador *sim);

int main() {
    srand(time(NULL)); // Inicializa semente para números aleatórios

    int alg;
    printf("Escolha o algoritmo (0=FIFO, 1=LRU, 2=CLOCK, 3=RANDOM): ");
    scanf("%d", &alg);

    // Inicializa simulador: 4KB página, 16KB memória (4 frames)
    Simulador *sim = inicializar_simulador(4096, 16384, alg);

    // Cria processos
    criar_processo(sim, 16384, 1); // Processo 1, 16KB = 4 páginas
    criar_processo(sim, 24576, 2); // Processo 2, 24KB = 6 páginas

    printf("\n=== Estado inicial da memória ===\n");
    exibir_memoria_fisica(sim);

    // Simula acessos aos processos
    simular_acessos(sim, 1, 10); // 10 acessos ao processo 1
    simular_acessos(sim, 2, 15); // 15 acessos ao processo 2

    printf("\n=== Estado final da memória ===\n");
    exibir_memoria_fisica(sim);

    printf("\n=== Estatísticas da simulação ===\n");
    exibir_estatisticas(sim);

    // Libera memória alocada
    // (Você precisaria implementar uma função liberar_simulador())

    liberar_simulador(sim);

    return 0;
}

void simular_acessos(Simulador *sim, int pid, int num_acessos) {
    Processo *proc = NULL;
    for (int i = 0; i < sim->num_processos; i++) {
        if (sim->processos[i].pid == pid) {
            proc = &sim->processos[i];
            break;
        }
    }

    if (!proc) {
        printf("Processo %d não encontrado!\n", pid);
        return;
    }

    printf("\nSimulando %d acessos para processo %d...\n", num_acessos, pid);

    for (int i = 0; i < num_acessos; i++) {
        // Gera um endereço virtual aleatório dentro do espaço do processo
        int endereco_virtual = rand() % proc->tamanho;
        int endereco_fisico = traduzir_endereco(sim, pid, endereco_virtual);

        printf("Acesso %d: Virtual: 0x%X -> Físico: 0x%X\n",
               i+1, endereco_virtual, endereco_fisico);

        sim->tempo_atual++; // Incrementa o tempo da simulação
    }
}

void exibir_estatisticas(Simulador *sim) {
    printf("Total de acessos: %d\n", sim->total_acessos);
    printf("Page faults: %d\n", sim->page_faults);
    printf("Taxa de page faults: %.2f%%\n",
           (float)sim->page_faults / sim->total_acessos * 100);

    // Mostra estatísticas por processo
    for (int i = 0; i < sim->num_processos; i++) {
        printf("\nProcesso %d:\n", sim->processos[i].pid);
        printf("  Páginas carregadas: ");

        int carregadas = 0;
        for (int j = 0; j < sim->processos[i].num_paginas; j++) {
            if (sim->processos[i].tabela_paginas[j].presente) {
                printf("%d ", j);
                carregadas++;
            }
        }
        printf("\n  Total páginas presentes: %d/%d\n",
               carregadas, sim->processos[i].num_paginas);
    }
}

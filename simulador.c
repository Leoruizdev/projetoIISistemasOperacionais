#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "estruturas.h"

// Função auxiliar para encontrar um processo pelo PID
Processo* encontrar_processo(Simulador *sim, int pid) {
    for (int i = 0; i < sim->num_processos; i++) {
        if (sim->processos[i].pid == pid) {
            return &sim->processos[i];
        }
    }
    return NULL;
}

// Função para registrar um acesso à página
void registrar_acesso(Simulador *sim, int pid, int pagina, int tipo_acesso) {
    Processo *proc = encontrar_processo(sim, pid);
    if (!proc) return;

    if (pagina < 0 || pagina >= proc->num_paginas) return;

    proc->tabela_paginas[pagina].referenciada = 1;
    if (tipo_acesso == 1) { // Escrita
        proc->tabela_paginas[pagina].modificada = 1;
    }
}

// Função FIFO para substituição de página
int substituir_pagina_fifo(Simulador *sim) {
    int frame_mais_antigo = 0;
    int tempo_mais_antigo = INT_MAX;

    for (int i = 0; i < sim->memoria.num_frames; i++) {
        if (sim->memoria.tempo_carga[i] < tempo_mais_antigo) {
            tempo_mais_antigo = sim->memoria.tempo_carga[i];
            frame_mais_antigo = i;
        }
    }

    // Remove a página atual do frame
    int conteudo = sim->memoria.frames[frame_mais_antigo];
    int pid = conteudo >> 16;
    int pagina = conteudo & 0xFFFF;

    Processo *proc = encontrar_processo(sim, pid);
    if (proc) {
        proc->tabela_paginas[pagina].presente = 0;
        proc->tabela_paginas[pagina].frame = -1;
    }

    return frame_mais_antigo;
}

// Função LRU para substituição de página
int substituir_pagina_lru(Simulador *sim) {
    int frame_lru = 0;
    int tempo_mais_antigo = INT_MAX;

    for (int i = 0; i < sim->memoria.num_frames; i++) {
        int conteudo = sim->memoria.frames[i];
        int pid = conteudo >> 16;
        int pagina = conteudo & 0xFFFF;

        Processo *proc = encontrar_processo(sim, pid);
        if (proc && proc->tabela_paginas[pagina].ultimo_acesso < tempo_mais_antigo) {
            tempo_mais_antigo = proc->tabela_paginas[pagina].ultimo_acesso;
            frame_lru = i;
        }
    }

    // Remove a página atual do frame
    int conteudo = sim->memoria.frames[frame_lru];
    int pid = conteudo >> 16;
    int pagina = conteudo & 0xFFFF;

    Processo *proc = encontrar_processo(sim, pid);
    if (proc) {
        proc->tabela_paginas[pagina].presente = 0;
        proc->tabela_paginas[pagina].frame = -1;
    }

    return frame_lru;
}

// Função para tradução de endereço
int traduzir_endereco(Simulador *sim, int pid, int endereco_virtual) {
    sim->total_acessos++;

    int pagina, deslocamento;
    extrair_pagina_deslocamento(sim, endereco_virtual, &pagina, &deslocamento);

    Processo *proc = encontrar_processo(sim, pid);
    if (!proc) return -1; // Processo não encontrado

    // Verifica se a página é válida
    if (pagina < 0 || pagina >= proc->num_paginas) {
        return -1; // Página inválida
    }

    if (proc->tabela_paginas[pagina].presente) {
        registrar_acesso(sim, pid, pagina, 0); // Tipo de acesso: leitura
        proc->tabela_paginas[pagina].ultimo_acesso = sim->tempo_atual;
        return proc->tabela_paginas[pagina].frame * sim->tamanho_pagina + deslocamento;
    }

    // Page fault
    sim->page_faults++;

    int frame;
    switch (sim->algoritmo) {
        case 0:
            frame = substituir_pagina_fifo(sim);
            break;
        case 1:
            frame = substituir_pagina_lru(sim);
            break;
        default:
            printf("Algoritmo inválido. Usando FIFO como padrão.\n");
            frame = substituir_pagina_fifo(sim);
    }

    // Atualiza a memória física
    sim->memoria.frames[frame] = (pid << 16) | pagina;
    sim->memoria.tempo_carga[frame] = sim->tempo_atual;

    // Atualiza a tabela de páginas do processo
    proc->tabela_paginas[pagina].presente = 1;
    proc->tabela_paginas[pagina].frame = frame;
    proc->tabela_paginas[pagina].tempo_carga = sim->tempo_atual;
    proc->tabela_paginas[pagina].ultimo_acesso = sim->tempo_atual;
    proc->tabela_paginas[pagina].referenciada = 1; // Página foi referenciada ao ser carregada

    registrar_acesso(sim, pid, pagina, 1); // Tipo de acesso: escrita

    return frame * sim->tamanho_pagina + deslocamento;
}


// Função para extrair página e deslocamento
void extrair_pagina_deslocamento(Simulador *sim, int endereco_virtual, int *pagina, int *deslocamento) {
    *pagina = endereco_virtual / sim->tamanho_pagina;
    *deslocamento = endereco_virtual % sim->tamanho_pagina;
}

int verificar_pagina_presente(Simulador *sim, int pid, int pagina) {
    Processo *proc = encontrar_processo(sim, pid);
    if (!proc || pagina < 0 || pagina >= proc->num_paginas) return 0;
    return proc->tabela_paginas[pagina].presente;
}

int carregar_pagina(Simulador *sim, int pid, int pagina) {
    int frame = -1;

    // Verifica se há frame livre
    for (int i = 0; i < sim->memoria.num_frames; i++) {
        if (sim->memoria.frames[i] == -1) {
            frame = i;
            break;
        }
    }

    // Se não houver, aplica substituição
    if (frame == -1) {
        switch (sim->algoritmo) {
            case 0: frame = substituir_pagina_fifo(sim); break;
            case 1: frame = substituir_pagina_lru(sim); break;
            case 2: frame = substituir_pagina_clock(sim); break;
            case 3: frame = substituir_pagina_random(sim); break;
            default: frame = substituir_pagina_fifo(sim); break;
        }
    }

    // Atualiza estruturas
    sim->memoria.frames[frame] = (pid << 16) | pagina;
    sim->memoria.tempo_carga[frame] = sim->tempo_atual;

    Processo *proc = encontrar_processo(sim, pid);
    proc->tabela_paginas[pagina].presente = 1;
    proc->tabela_paginas[pagina].frame = frame;
    proc->tabela_paginas[pagina].tempo_carga = sim->tempo_atual;
    proc->tabela_paginas[pagina].ultimo_acesso = sim->tempo_atual;
    proc->tabela_paginas[pagina].referenciada = 1;

    return frame;
}


// Função para inicializar o simulador (já implementada)
Simulador *inicializar_simulador(int tamanho_pagina, int tamanho_memoria_fisica, int algoritmo) {
    Simulador *sim = malloc(sizeof(Simulador));
    sim->tempo_atual = 0;
    sim->tamanho_pagina = tamanho_pagina;
    sim->tamanho_memoria_fisica = tamanho_memoria_fisica;
    sim->num_processos = 0;
    sim->total_acessos = 0;
    sim->page_faults = 0;
    sim->algoritmo = algoritmo;

    int num_frames = tamanho_memoria_fisica / tamanho_pagina;
    sim->memoria.num_frames = num_frames;
    sim->memoria.frames = malloc(sizeof(int) * num_frames);
    sim->memoria.tempo_carga = malloc(sizeof(int) * num_frames);

    for(int i = 0; i < num_frames; i++){
        sim->memoria.frames[i] = -1; // frame vazio
        sim->memoria.tempo_carga[i] = -1;
    }

    sim->processos = NULL;
    return sim;
}

// Função para criar um processo (já implementada)
Processo *criar_processo(Simulador *sim, int tamanho_processo, int pid) {
    sim->num_processos++;
    sim->processos = realloc(sim->processos, sizeof(Processo) * sim->num_processos);

    Processo *proc = &sim->processos[sim->num_processos - 1];
    proc->pid = pid;
    proc->tamanho = tamanho_processo;
    proc->num_paginas = tamanho_processo / sim->tamanho_pagina;
    proc->tabela_paginas = malloc(sizeof(Pagina) * proc->num_paginas);

    for(int i = 0; i < proc->num_paginas; i++){
        proc->tabela_paginas[i].presente = 0;
        proc->tabela_paginas[i].frame = -1;
        proc->tabela_paginas[i].modificada = 0;
        proc->tabela_paginas[i].referenciada = 0;
        proc->tabela_paginas[i].tempo_carga = -1;
        proc->tabela_paginas[i].ultimo_acesso = -1;
    }

    return proc;
}

void executar_simulacao(Simulador *sim, int algoritmo) {
    for (int i = 0; i < sim->num_processos; i++) {
        simular_acessos(sim, sim->processos[i].pid, 10);
    }
    exibir_memoria_fisica(sim);
    exibir_estatisticas(sim);
}

int acessar_memoria(Simulador *sim, int pid, int endereco_virtual) {
    int pagina, deslocamento;
    extrair_pagina_deslocamento(sim, endereco_virtual, &pagina, &deslocamento);

    if (!verificar_pagina_presente(sim, pid, pagina)) {
        carregar_pagina(sim, pid, pagina);
        sim->page_faults++;
    }

    registrar_acesso(sim, pid, pagina, 0);
    Processo *proc = encontrar_processo(sim, pid);
    proc->tabela_paginas[pagina].ultimo_acesso = sim->tempo_atual;

    return proc->tabela_paginas[pagina].frame * sim->tamanho_pagina + deslocamento;
}


// Função para exibir a memória física
void exibir_memoria_fisica(Simulador *sim) {
    printf("Estado da Memória Física:\n");
    for (int i = 0; i < sim->memoria.num_frames; i++) {
        if (sim->memoria.frames[i] == -1)
            printf("Frame %d: [ LIVRE ]\n", i);
        else {
            int pid = sim->memoria.frames[i] >> 16;
            int pagina = sim->memoria.frames[i] & 0xFFFF;
            printf("Frame %d: Processo %d - Página %d\n", i, pid, pagina);
        }
    }
}

void liberar_simulador(Simulador *sim) {
    for (int i = 0; i < sim->num_processos; i++) {
        free(sim->processos[i].tabela_paginas);
    }
    free(sim->processos);
    free(sim->memoria.frames);
    free(sim->memoria.tempo_carga);
    free(sim);
}


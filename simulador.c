#include <stdio.h>
#include <stdlib.h>
#include "estruturas.h"

Simulador *inicializar_simulador(int tamanho_pagina, int tamanho_memoria_fisica, int algoritmo){
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

Processo *criar_processo(Simulador *sim, int tamanho_processo, int pid){
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

void extrair_pagina_deslocamento ( Simulador *sim, int endereco_virtual, int *pagina ,int *deslocamento){
    *pagina = endereco_virtual / sim->tamanho_pagina;
    *deslocamento = endereco_virtual & sim->tamanho_pagina;
}

int traduzir_endereco(Simulador *sim, int pid, int endereco_virtual){
    sim->total_acessos++;

    int pagina, deslocamento;

    extrair_pagina_deslocamento(sim, endereco_virtual, &pagina, &deslocamento);

    Processo *proc = NULL;
    for (int i = 0; i < sim->num_processos; i++){
        if(sim->num_processos[i].pid == pid){
            proc = &sim->processos;
            break;
        }
    }

    if(!proc) return -1; // processo n encontrado.

    if(proc->tabela_paginas[pagina].presente){
        registrar_acesso(sim, pid, pagina, 0); // tipo_acesso = leitura
        proc->tabela_paginas[pagina].ultimo_acesso = sim->tempo_atual;
        return proc->tabela_paginas[pagina].frame * sim->tamanho_pagina + deslocamento;
    }

    //page fault
    sim->page_faults++;
    
    int frame;
    switch (sim->algoritmo)
    {
    case 0:
        frame = substituir_pagina_fifo(sim);
        break;
    case 1: 
        frame = substituir_pagina_lru(sim);
        break;
    default:
        printf("Opção Inválida. Usaremos como padrão o FIFO");
        frame = substituir_pagina_fifo(sim);
    }

    sim->memoria.frames[frame] = (pid << 16) | pagina;
    sim->memoria.tempo_carga[frame] = sim->tempo_atual;

    // Atualizar a tabela de páginas do processo
    proc->tabela_paginas[pagina].presente = 1;
    proc->tabela_paginas[pagina].frame = frame;
    proc->tabela_paginas[pagina].tempo_carga = sim->tempo_atual;
    proc->tabela_paginas[pagina].ultimo_acesso = sim->tempo_atual;

    registrar_acesso(sim, pid, pagina, 1); // tipo_acesso = escrita (por substituição)

    return frame * sim->tamanho_pagina + deslocamento;
    
}

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

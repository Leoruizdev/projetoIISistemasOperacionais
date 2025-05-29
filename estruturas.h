#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

//Estruturas passadas pelo Professor
typedef struct {
    int presente; // 1 se a página está na memória, 0 caso contrário
    int frame; // Número de frame onde a página está alocada (-1 se não alocada)
    int modificada; // 1 se a página foi modificada, 0 caso contrário
    int referenciada; // 1 se a página foi referenciada recentemente, 0 caso contrário
    int tempo_carga; // Instante em que a página foi carregada na memória
    int ultimo_acesso; // Instante do último acesso à página 
} Pagina;

typedef struct {
    int pid; // Identificador do processo
    int tamanho; // Tamanho do processo em bytes
    int num_paginas; // Número de páginas do processo
    Pagina *tabela_paginas; // Tabela de páginas do processo
} Processo;

typedef struct {
    int num_frames; // Número total de frames na memória física
    int *frames;    // Array de frames (cada elemento contém o pid e a página )
                    // Ex: frames [i] = (pid << 16) | num_pagina
    int *tempo_carga; // Tempo em que cada frame foi carregado (para FIFO)
} MemoriaFisica;

typedef struct {
    int tempo_atual; // Contador de tempo da simulação
    int tamanho_pagina; // Tamanho da página em bytes
    int tamanho_memoria_fisica; // Tamanho da memória física em bytes
    int num_processos; // Número de processos na simulação
    Processo *processos; // Array de processos
    MemoriaFisica memoria; // Memória física

// Estatísticas
    int total_acessos;    // Total de acessos à memória
    int page_faults;      // Total de page faults ocorridos

    // Algoritmo de substituição atual
    int algoritmo;        // 0=FIFO , 1=LRU , 2= CLOCK , 3= RANDOM , 4= CUSTOM
} Simulador;

// Inicialização
Simulador *inicializar_simulador(int tamanho_pagina, int tamanho_memoria_fisica, int algoritmo); // já feito
Processo *criar_processo(Simulador *sim, int tamanho_processo, int pid); // já feito


//Tradução de Endereços
// Traduz um endereço virtual para físico
// Retorna o endereço físico ou -1 em caso de page fault
int traduzir_endereco(Simulador *sim, int pid, int endereco_virtual); // feita

// Extrai o número da página e o deslocamento de um endereço virtual
void extrair_pagina_deslocamento ( Simulador *sim, int endereco_virtual, int *pagina ,int *deslocamento); // feita

//Manipulação de Memória
// Verifica se uma página está presente na memória física
int verificar_pagina_presente (Simulador *sim, int pid, int pagina); // 3 - Leonardo
// Carrega uma página na memória física
// Retorna o número do frame onde a página foi carregada
int carregar_pagina ( Simulador *sim, int pid, int pagina); // 4 - Leonardo 
// Implementa o algoritmo de substituição de páginas FIFO // 5 - Leonardo 
int substituir_pagina_fifo (Simulador *sim); 
// Implementa o algoritmo de substituição de páginas LRU 
int substituir_pagina_lru (Simulador *sim);
// Implementa o algoritmo de substituição de páginas CLOCK    // algum desses 6 - Leonardo
int substituir_pagina_clock (Simulador *sim);
// Implementa o algoritmo de substituição de páginas RANDOM
int substituir_pagina_random (Simulador *sim);

//Estatísticas e Visualização
// Exibe o estado atual da memória física
void exibir_memoria_fisica (Simulador *sim); // já feito
// Exibe estatísticas da simulação
void exibir_estatisticas (Simulador *sim); // 7 - Jully
// Registra um acesso à memória
void registrar_acesso (Simulador *sim, int pid, int pagina, int tipo_acesso); // 8 - Jully

//Simulação
// Executa a simulação com uma sequência de acessos à memória
void executar_simulacao (Simulador *sim ,int algoritmo); // 9 - Jully
// Simula um acesso à memória
int acessar_memoria (Simulador *sim, int pid, int endereco_virtual); // 10 - Jully

#endif
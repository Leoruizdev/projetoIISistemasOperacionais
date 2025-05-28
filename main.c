#include <stdio.h>
#include "estruturas.h"

int main(){

  int alg;
  printf("Escolha o algoritmo (0=FIFO, 1=LRU, 2=CLOCK, 3=RANDOM): ");
  scanf("%d", &alg);

  Simulador *sim = inicializar_simulador(4096, 16384, alg); // 4 KB página, 16 KB memória (4 frames)

  criar_processo(sim, 16384, 1); // processo 1, 16 KB = 4 páginas
  criar_processo(sim, 16384, 2); // processo 2, 16 KB = 4 páginas

  exibir_memoria_fisica(sim);
  return 0;
}

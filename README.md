# 🧠 Simulador de Paginação de Memória

Este projeto simula o gerenciamento de memória por paginação, com suporte a múltiplos processos e algoritmos de substituição de páginas. O simulador foi desenvolvido em C como parte do projeto da disciplina de Sistemas Operacionais.

## 🔧 Funcionalidades

- Gerenciamento de memória física com páginas e frames
- Criação de processos com suas tabelas de páginas
- Tradução de endereços virtuais para físicos
- Simulação de acessos à memória com geração de page faults
- Implementação de 4 algoritmos de substituição de páginas:
  - FIFO
  - LRU
  - CLOCK
  - RANDOM
- Estatísticas detalhadas da simulação

## 📁 Estrutura do Projeto

```
projetoIISistemasOperacionais/
├── main.c                 # Função principal: inicia simulação e gera acessos
├── simulador.c            # Lógica do simulador: substituição, tradução, registros
├── estruturas.h           # Definição das estruturas e cabeçalhos
├── Makefile               # Script de build para compilar facilmente
└── README.md              # Este arquivo
```

## 🚀 Compilação e Execução

### Usando Make:

```bash
make
./simulador
```

### Ou compilando manualmente:

```bash
gcc main.c simulador.c -o simulador
./simulador
```

## 📌 Como funciona

1. O usuário escolhe o algoritmo de substituição de páginas (0 a 3).
2. Dois processos são criados com tamanhos predefinidos.
3. Cada processo realiza acessos aleatórios à memória.
4. O sistema gerencia as páginas e substitui quando necessário.
5. Ao final, o programa exibe o estado da memória e estatísticas da simulação.

## 📊 Exemplo de Saída

```txt
Escolha o algoritmo (0=FIFO, 1=LRU, 2=CLOCK, 3=RANDOM): 1

=== Estado inicial da memória ===
Frame 0: [ LIVRE ]
Frame 1: [ LIVRE ]
Frame 2: [ LIVRE ]
Frame 3: [ LIVRE ]

Simulando 10 acessos para processo 1...
Acesso 1: Virtual: 0x0 -> Físico: 0x0
...

=== Estado final da memória ===
Frame 0: Processo 1 - Página 1
Frame 1: Processo 2 - Página 2
...

=== Estatísticas da simulação ===
Total de acessos: 25
Page faults: 18
Taxa de page faults: 72.00%
```

## 👨‍💻 Desenvolvido por

- **JULLY MANUELE DIAS LIMA - 10420556** 
- **KARINA DA CUNHA ALVES - 10420180** 
- **LEONARDO SILVA MORENO RUIZ - 10420477** 
- **MARCELO LUIS SIMONE LUCAS - 10332213**

  Universidade Presbiteriana Mackenzie  
  Curso: Ciência da Computação  
  Projeto de Sistemas Operacionais
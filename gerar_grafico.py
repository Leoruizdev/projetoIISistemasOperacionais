import pandas as pd
import matplotlib.pyplot as plt

# Lê o CSV
df = pd.read_csv("resultados.csv", header=None, names=["Algoritmo", "Acessos", "PageFaults"])

# Mapeia códigos de algoritmo para nomes
nomes = {0: "FIFO", 1: "LRU", 2: "CLOCK", 3: "RANDOM"}
df["Nome"] = df["Algoritmo"].map(nomes)

# Cria gráfico de barras
plt.figure(figsize=(8,5))
plt.bar(df["Nome"], df["PageFaults"], width=0.5)
plt.title("Comparação de Page Faults por Algoritmo")
plt.xlabel("Algoritmo")
plt.ylabel("Page Faults")
plt.grid(axis='y')
plt.tight_layout()
plt.savefig("comparacao_algoritmos.png")
plt.show()

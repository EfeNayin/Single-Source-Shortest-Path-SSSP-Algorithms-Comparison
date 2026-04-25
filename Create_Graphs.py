import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

plt.style.use('seaborn-v0_8-darkgrid')
colors = {'Dijkstra': "#0D83B5", 'Bellman-Ford': "#C80F0F"}

df = pd.read_csv('benchmark_results.csv')
datasets = sorted(df['Dataset'].unique())
algorithms = sorted(df['Algorithm'].unique())
x = np.arange(len(datasets))
width = 0.35

#log scale
fig, ax = plt.subplots(figsize=(10, 6))
for i, algo in enumerate(algorithms):
    algo_data = df[df['Algorithm'] == algo]
    runtimes = [algo_data[algo_data['Dataset'] == d]['Avg_Runtime_ms'].values[0] for d in datasets]
    bars = ax.bar(x + i*width, runtimes, width, label=algo, color=colors[algo])

ax.set_yscale('log') 
ax.set_xlabel('Dataset', fontsize=12, fontweight='bold')
ax.set_ylabel('Average Runtime (ms) - Log Scale', fontsize=12, fontweight='bold')
ax.set_title('SSSP Algorithm Comparison: Runtime by Dataset', fontsize=14, fontweight='bold')
ax.set_xticks(x + width/2)
ax.set_xticklabels(datasets)
ax.legend()
ax.grid(axis='y', alpha=0.3)
plt.tight_layout()
plt.savefig('runtime_comparison.png', dpi=300, bbox_inches='tight')
plt.close()
print("✓ runtime_comparison.png")

# Scalability
fig, ax = plt.subplots(figsize=(10, 6))
for algo in algorithms:
    algo_data = df[df['Algorithm'] == algo].sort_values('Nodes')
    ax.plot(algo_data['Nodes'], algo_data['Avg_Runtime_ms'],
           marker='o', linewidth=2, markersize=8, label=algo, color=colors[algo])

ax.set_xlabel('Number of Nodes', fontsize=12, fontweight='bold')
ax.set_ylabel('Average Runtime (ms)', fontsize=12, fontweight='bold')
ax.set_title('SSSP Algorithm: Runtime Scalability', fontsize=14, fontweight='bold')
ax.set_xscale('log')
ax.set_yscale('log')
ax.legend()
ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig('scalability_comparison.png', dpi=300, bbox_inches='tight')
plt.close()
print("scalability_comparison.png")

# Memory 
fig, ax = plt.subplots(figsize=(10, 6))
for i, algo in enumerate(algorithms):
    algo_data = df[df['Algorithm'] == algo]
    memory = [algo_data[algo_data['Dataset'] == d]['Memory_KB'].values[0] for d in datasets]
    ax.bar(x + i*width, memory, width, label=algo, color=colors[algo])

ax.set_xlabel('Dataset', fontsize=12, fontweight='bold')
ax.set_ylabel('Memory Usage (KB)', fontsize=12, fontweight='bold')
ax.set_title('SSSP Algorithm Comparison: Memory Usage', fontsize=14, fontweight='bold')
ax.set_xticks(x + width/2)
ax.set_xticklabels(datasets)
ax.legend()
ax.grid(axis='y', alpha=0.3)
plt.tight_layout()
plt.savefig('memory_comparison.png', dpi=300, bbox_inches='tight')
plt.close()
print("memory_comparison.png")

# Speed up 
fig, ax = plt.subplots(figsize=(10, 6))
dijkstra_data = df[df['Algorithm'] == 'Dijkstra'].set_index('Dataset')
bellman_ford_data = df[df['Algorithm'] == 'Bellman-Ford'].set_index('Dataset')
speedup = bellman_ford_data.loc[datasets, 'Avg_Runtime_ms'].values / dijkstra_data.loc[datasets, 'Avg_Runtime_ms'].values

bars = ax.bar(datasets, speedup, color='#F18F01', alpha=0.9, edgecolor='black', linewidth=1.5)
ax.axhline(y=1, color='red', linestyle='--', linewidth=2, label='Parity (1x)')

for bar in bars:
    height = bar.get_height()
    ax.text(bar.get_x() + bar.get_width()/2., height,
           f'{height:,.0f}x', ha='center', va='bottom', fontweight='bold', fontsize=10)

ax.set_xlabel('Dataset', fontsize=12, fontweight='bold')
ax.set_ylabel('Speedup Factor', fontsize=12, fontweight='bold')
ax.set_title('Dijkstra Speedup over Bellman-Ford', fontsize=14, fontweight='bold')
ax.legend()
ax.grid(axis='y', alpha=0.3)
plt.tight_layout()
plt.savefig('speedup_comparison.png', dpi=300, bbox_inches='tight')
plt.close()
print("speedup_comparison.png")

print("\nBenchmark Summary:")
print(df.to_string(index=False))
print("\n All graphs created")
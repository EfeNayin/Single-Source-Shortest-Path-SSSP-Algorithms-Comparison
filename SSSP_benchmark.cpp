#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <limits>
#include <chrono>
#include <iomanip>
#include <string>
#include <cstring>
#include <algorithm>
#include <functional>
#include <windows.h>
#include <psapi.h>

using namespace std;

const long long INF = numeric_limits<long long>::max();
const int NUM_DIJKSTRA_SOURCES = 10;
const int NUM_BF_SOURCES = 1;


struct Graph {
    int num_nodes;
    long long num_edges;
    vector<vector<pair<int, long long>>> adj; 
    
    Graph(int n) : num_nodes(n), num_edges(0), adj(n) {}
    
    void add_edge(int u, int v, long long w) {
        adj[u].push_back({v, w});
        num_edges++;
    }
};

struct BenchmarkResult {
    string dataset_name;
    string algorithm;
    int num_nodes;
    long long num_edges;
    double avg_runtime_ms;
    double total_runtime_ms;
    long long memory_used_kb;
    int num_sources_tested;
};

// Dijkstra's Algorithm
long long dijkstra(const Graph& graph, int source, vector<long long>& dist) {
    dist.assign(graph.num_nodes, INF);
    dist[source] = 0;
    
    priority_queue<pair<long long, int>, 
                   vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;
    pq.push({0, source});
    
    long long operations = 0;
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        operations++;
        
        if (d > dist[u]) continue;
        
        for (auto [v, w] : graph.adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    
    return operations;
}

// Bellman_Ford Algorithm
long long bellman_ford(const Graph& graph, int source, vector<long long>& dist) {
    dist.assign(graph.num_nodes, INF);
    dist[source] = 0;
    
    long long operations = 0;
    
    for (int i = 0; i < graph.num_nodes - 1; i++) {
        for (int u = 0; u < graph.num_nodes; u++) {
            if (dist[u] == INF) continue;
            
            for (auto [v, w] : graph.adj[u]) {
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    operations++;
                }
            }
        }
    }
    
    return operations;
}

bool read_graph(const string& filename, Graph& graph) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return false;
    }
    
    string line;
    int max_node = 0;
    long long edge_count = 0;
    
    while (getline(file, line)) {
        if (line.empty() || line[0] == 'c') continue; 
        
        if (line[0] == 'p') {
            int nodes, edges;
            sscanf(line.c_str(), "p sp %d %d", &nodes, &edges);
            graph = Graph(nodes);
            continue;
        }
        
        if (line[0] == 'a') {
            int u, v;
            long long w;
            sscanf(line.c_str(), "a %d %d %lld", &u, &v, &w);
            
            u--; v--;
            
            graph.add_edge(u, v, w);
            max_node = max(max_node, max(u, v));
            edge_count++;
            
            if (edge_count % 1000000 == 0) {
                cout << "  Loaded " << edge_count << " edges..." << endl;
            }
        }
    }
    
    file.close();
    return true;
}

long long get_memory_usage() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024;
    }
    return 0;
}

BenchmarkResult benchmark_algorithm(const string& dataset_name,
                                   const Graph& graph,
                                   const string& algorithm_name,
                                   int num_sources,
                                   function<long long(const Graph&, int, vector<long long>&)> algo) {
    BenchmarkResult result;
    result.dataset_name = dataset_name;
    result.algorithm = algorithm_name;
    result.num_nodes = graph.num_nodes;
    result.num_edges = graph.num_edges;
    result.num_sources_tested = min(num_sources, graph.num_nodes);
    
    vector<double> runtimes;
    vector<long long> dist;
    
    long long mem_before = get_memory_usage();
    
    for (int i = 0; i < result.num_sources_tested; i++) {
        int source = (i * graph.num_nodes) / result.num_sources_tested;
        
        auto start = chrono::high_resolution_clock::now();
        algo(graph, source, dist);
        auto end = chrono::high_resolution_clock::now();
        
        double runtime_ms = chrono::duration<double, milli>(end - start).count();
        runtimes.push_back(runtime_ms);
        
        cout << "    Source " << (i+1) << "/" << result.num_sources_tested 
             << ": " << fixed << setprecision(2) << runtime_ms << " ms" << endl;
    }
    
    long long mem_after = get_memory_usage();
    
    result.total_runtime_ms = 0;
    for (double t : runtimes) result.total_runtime_ms += t;
    result.avg_runtime_ms = result.total_runtime_ms / result.num_sources_tested;
    result.memory_used_kb = max(0LL, mem_after - mem_before);
    
    return result;
}


int main(int argc, char* argv[]) {
    cout << "SSSP Algorithm Benchmark" << endl;

    vector<pair<string, string>> datasets = {
        {"NY", "Distance Graph/USA-road-d.NY.gr"},
        {"COL", "Distance Graph/USA-road-d.COL.gr"}
    };
    
    vector<BenchmarkResult> all_results;
    
    for (auto [name, filename] : datasets) {
        cout << "Loading dataset: " << name << " (" << filename << ")..." << endl;
        
        Graph graph(0);
        if (!read_graph(filename, graph)) {
            cerr << "Failed to load " << filename << endl;
            continue;
        }
        
        cout << "  Nodes: " << graph.num_nodes << endl;
        cout << "  Edges: " << graph.num_edges << "\n" << endl;
        
        cout << "Running Dijkstra on " << name << "..." << endl;
        auto dij_result = benchmark_algorithm(
            name, graph, "Dijkstra", NUM_DIJKSTRA_SOURCES,
            [](const Graph& g, int s, vector<long long>& d) { return dijkstra(g, s, d); }
        );
        all_results.push_back(dij_result);
        cout << endl;
        
        cout << "Running Bellman-Ford on " << name << "..." << endl;
        auto bf_result = benchmark_algorithm(
            name, graph, "Bellman-Ford", NUM_BF_SOURCES,
            [](const Graph& g, int s, vector<long long>& d) { return bellman_ford(g, s, d); }
        );
        all_results.push_back(bf_result);
        cout << endl;
    }

    cout << "Writing results to benchmark_results.csv..." << endl;
    ofstream csv_file("benchmark_results.csv");
    csv_file << "Dataset,Algorithm,Nodes,Edges,Avg_Runtime_ms,Total_Runtime_ms,Memory_KB,Sources_Tested\n";
    
    for (auto& result : all_results) {
        csv_file << result.dataset_name << ","
                 << result.algorithm << ","
                 << result.num_nodes << ","
                 << result.num_edges << ","
                 << fixed << setprecision(3) << result.avg_runtime_ms << ","
                 << result.total_runtime_ms << ","
                 << result.memory_used_kb << ","
                 << result.num_sources_tested << "\n";
    }
    csv_file.close();
   
    cout << "\n Final" << endl;
    cout << left << setw(15) << "Dataset" 
         << setw(15) << "Algorithm" 
         << setw(15) << "Average Time (ms)" 
         << setw(15) << "Memory (KB)" << endl;
    cout << string(60, '-') << endl;
    
    for (auto& result : all_results) {
        cout << left << setw(15) << result.dataset_name
             << setw(15) << result.algorithm
             << setw(15) << fixed << setprecision(3) << result.avg_runtime_ms
             << setw(15) << result.memory_used_kb << endl;
    }
    
    return 0;
}

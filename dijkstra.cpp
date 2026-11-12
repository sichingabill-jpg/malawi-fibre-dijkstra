// Dijkstra's shortest path on the Malawi National Fibre Backbone graph.
// Build:  g++ -std=c++17 -O2 -o dijkstra dijkstra.cpp
// Run:    ./dijkstra                 (interactive)
//         ./dijkstra A N             (Lilongwe -> Nsanje)
//         ./dijkstra Lilongwe Nsanje -v   (-v prints every step)
#include <algorithm>
#include <climits>
#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
using namespace std;

const vector<string> TOWN = {
    "Lilongwe", "Blantyre", "Mzuzu", "Zomba", "Kasungu", "Mangochi", "Salima",
    "Karonga", "Dedza", "Nkhotakota", "Liwonde", "Balaka", "Mchinji", "Nsanje",
    "Mulanje", "Thyolo", "Chikwawa", "Ntcheu", "Dwangwa", "Rumphi", "Mzimba"};  // A..U

struct Edge { char u, v; int km; };
const vector<Edge> EDGES = {
    {'B','D',60},  {'B','O',80},  {'B','Q',40},  {'D','K',50},  {'K','F',70},
    {'F','G',90},  {'G','A',100}, {'A','M',110}, {'A','I',90},  {'I','R',50},  // I-R: 50 per table (drawing says 30)
    {'R','L',60},  {'L','K',40},  {'A','E',130}, {'E','U',140}, {'U','C',50},
    {'C','T',60},  {'T','H',120}, {'G','J',100}, {'J','S',40},  {'S','C',150},
    {'Q','N',80},  {'P','B',30}};

const int INF = INT_MAX;
using Adj = vector<vector<pair<int,int>>>;

string label(int i) { return string(1, char('A' + i)) + " (" + TOWN[i] + ")"; }

// Accepts a letter (A-U) or a town name, any case. Returns -1 if unknown.
int parseNode(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    if (s.size() == 1 && s[0] >= 'a' && s[0] < 'a' + (int)TOWN.size()) return s[0] - 'a';
    for (size_t i = 0; i < TOWN.size(); i++) {
        string t = TOWN[i];
        transform(t.begin(), t.end(), t.begin(), ::tolower);
        if (t == s) return (int)i;
    }
    return -1;
}

void dijkstra(const Adj& g, int src, vector<int>& dist, vector<int>& prev, bool verbose) {
    int n = g.size();
    dist.assign(n, INF);
    prev.assign(n, -1);
    vector<bool> done(n, false);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;  // (distance, node)
    dist[src] = 0;
    pq.push({0, src});
    int step = 0;
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (done[u]) continue;           // stale queue entry
        done[u] = true;
        if (verbose) cout << "Step " << ++step << ": settle " << label(u) << " at " << d << " km\n";
        for (auto [v, w] : g[u]) {
            if (!done[v] && d + w < dist[v]) {
                dist[v] = d + w;
                prev[v] = u;
                pq.push({dist[v], v});
                if (verbose) cout << "    relax " << char('A' + u) << "->" << char('A' + v)
                                  << ": dist[" << char('A' + v) << "] = " << dist[v] << "\n";
            }
        }
    }
}

void report(const Adj& g, int s, int t, bool verbose) {
    vector<int> dist, prev;
    dijkstra(g, s, dist, prev, verbose);
    if (dist[t] == INF) { cout << "No route from " << label(s) << " to " << label(t) << "\n"; return; }
    vector<int> path;
    for (int x = t; x != -1; x = prev[x]) path.push_back(x);
    reverse(path.begin(), path.end());
    cout << "Shortest route " << label(s) << " -> " << label(t) << ": " << dist[t] << " km\nPath: ";
    for (size_t i = 0; i < path.size(); i++) cout << (i ? " -> " : "") << char('A' + path[i]) << " " << TOWN[path[i]];
    cout << "\n";
    if (verbose) {
        cout << "\nFinal distances from " << label(s) << ":\n";
        for (size_t i = 0; i < dist.size(); i++)
            cout << "  " << left << setw(18) << label(i) << dist[i] << " km\n";
    }
}

int main(int argc, char* argv[]) {
    Adj g(TOWN.size());
    for (auto& e : EDGES) {
        int a = e.u - 'A', b = e.v - 'A';
        g[a].push_back({b, e.km});
        g[b].push_back({a, e.km});       // undirected
    }
    bool verbose = false;
    vector<string> args;
    for (int i = 1; i < argc; i++) {
        string a = argv[i];
        if (a == "-v") verbose = true; else args.push_back(a);
    }
    if (args.size() == 1) { cerr << "Usage: ./dijkstra SOURCE DESTINATION [-v]   (or no arguments for interactive mode)\n"; return 1; }
    if (args.size() >= 2) {
        int s = parseNode(args[0]), t = parseNode(args[1]);
        if (s < 0 || t < 0) { cerr << "Error: unknown node '" << (s < 0 ? args[0] : args[1]) << "'\n"; return 1; }
        report(g, s, t, verbose);
        return 0;
    }
    cout << "Malawi fibre backbone - Dijkstra. Nodes A-U or town names. Type q to quit.\n";
    string a, b;
    while (true) {
        cout << "\nSource: ";      if (!(cin >> a) || a == "q") break;
        cout << "Destination: ";  if (!(cin >> b) || b == "q") break;
        int s = parseNode(a), t = parseNode(b);
        if (s < 0 || t < 0) { cout << "Unknown node '" << (s < 0 ? a : b) << "'. Use A-U or a town name.\n"; continue; }
        report(g, s, t, verbose);
    }
    return 0;
}

#include<bits/stdc++.h>
using namespace std;

#define int long long
int INF = LLONG_MAX;
double INFD = DBL_MAX;
using pii = pair<int, int>;
using pdi = pair<double, int>;



signed main(){
    string filename = "./output_meio/";
    string s;
    cout << "digite griddy para greedy e cmia para cmia-h\n";
    cin >> s;

    for(char &a: s){
        a = tolower(a);
    }

    if(s == "griddy"){
        filename += "greedy-seeds.txt";
    }
    else if(s == "cmia"){
        filename += "cmia-h-seeds.txt";
    }
    else{
        cout << "errou os nomes tente denovo\n";
        return 0;
    }

    ifstream input;
    input.open(filename);
    
    vector<int> seeds;
    int a;
    while(input >> a){
        seeds.push_back(a);
    }
    input.close();

    ifstream data("./datasets/NetHEPT.txt");

    int n, m, k, l;
    data >> n >> m >> k >> l;

    vector<vector<pair<int,double>>> out_adj(n);
    for (int i = 0; i < m; i++) {
        int x, y; double w;
        data >> x >> y >> w;          // aresta x->y com prob w (ja 0-indexado)
        out_adj[x].push_back({y, w});
    }

    vector<int> negs;                 // adversarios (campanha negativa)
    for (int i = 0; i < l; i++) { int x; data >> x; negs.push_back(x); }
    data.close();

    int R = 10000;                //carlos
    mt19937_64 rng(42);
    uniform_real_distribution<double> unif(0.0, 1.0);

    vector<vector<int>> live(n);
    vector<int> dN(n), dP(n), fila(n);

    auto bfs = [&](vector<int>& src, vector<int>& d) {   // BFS multi-fonte no grafo live
        for (int i = 0; i < n; i++) d[i] = INF;
        int head = 0, tail = 0;
        for (int x : src)
         {
            if (x >= 0 && x < n && d[x] == INF) //
            { 
            d[x] = 0; fila[tail++] = x; 
            }
        }
        while (head < tail) {
            int u = fila[head++];
            for (int v : live[u]) if (d[v] == INF) { d[v] = d[u] + 1; fila[tail++] = v; }
        }
    };

    double base = 0, massa = 0, salvos = 0;
    for (int sim = 0; sim < R; sim++) {
        for (int u = 0; u < n; u++) live[u].clear();
        for (int u = 0; u < n; u++)
            for (auto& e : out_adj[u])
                if (unif(rng) < e.second) live[u].push_back(e.first);

        bfs(negs, dN);                // campanha negativa
        bfs(seeds, dP);               // campanha positiva 

        for (int w = 0; w < n; w++) {
            if (dN[w] == INF) continue;     // negativa nem chegaria a w
            base += 1;                       // w seria infectado sem positivas
            if (dN[w] < dP[w]) massa += 1; // negativa chega antes -> ativado mesmo com bons
            else salvos += 1;                // positiva chegou junto/antes -> saved
        }
    }

    cout << fixed << setprecision(3);
    cout << "alcance negativo geral: " << base / R << " nos\n";
    cout << "ainda ativados com blockers:          " << massa / R;
    cout << "salvados pela heuristica:               " << salvos / R ;
    if (base > 0) cout << "   (" << 100.0 * salvos / base << "% bloqueado)";
    cout << "\n";

    return 0;
}





    
#include <bits/stdc++.h>
using namespace std;
#define int long long
int INF = LLONG_MAX;
double INFD = DBL_MAX;
using pii = pair<int, int>;
using pdi = pair<double, int>;

/* 
   no coicm tem probabilidade p_{v,w} por aresta

   o greedy eh hill climbing: em cada uma das k rodadas, pra cada
   candidato v ele estima por monte carlo o ganho marginal de adicionar v ao AL
   atual, e fica com o melhor. garante (1 - 1/e) do otimo (coicm eh submodular).
   */

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count());
uniform_real_distribution<double> unif(0.0, 1.0);

bool flip(double p) {//gambling
    return unif(rng) < p;
}

set<int> random_inimigso(int n, int sa_size) {
    vector<int> todos(n);
    iota(todos.begin(), todos.end(), 0);    
    shuffle(todos.begin(), todos.end(), rng);     // embaralha com o rng das moedas
    set<int> S;
    for (int i = 0; i < sa_size && i < n; i++) S.insert(todos[i]);
    return S;
}

/* 
   InfLimit(n, G, Sa,S, v)

   roda simulacao (um mundo, moedas ja jogadas) e devolve o
   ganho de v: quantos nos v consegue salvar que o conjunto S sozinh salvaria.

   roda uma simulacao de mundo possivel e retorna o ganho de v, ou seja o delta 
   de n lembro o nome 

   */
int InfLimit(int n, vector<pair<int, double>> out_adj[], set<int> &Sa, set<int> &S, int v) {

    // 1) gambling com as moedas para ver as arestas que estao on  
    vector<vector<int>> live(n);//store the live arestars
    for (int u = 0; u < n; u++)
        for (auto &e : out_adj[u])
            if (flip(e.second))            // e.second = p_{u, e.first} da aresta
                live[u].push_back(e.first);

    auto bfs = [&](const vector<int>& sources) {
        vector<int> d(n, INF);
        queue<int> q;
        for (int s : sources) { d[s] = 0; q.push(s); }
        while (!q.empty()) {
            int u = q.front(); 
            q.pop();
            for (int w : live[u])
                if (d[w] == INF) { 
                    d[w] = d[u] + 1; q.push(w);
                 }
        }
        return d;
    };

    // 2) ESPALHA C A PARTIR DE Sa.
    vector<int> Csrc(Sa.begin(), Sa.end());
    vector<int> dC = bfs(Csrc);

    // 3) tempos de chegada de L (MESMO grafo live):
    //    dS = melhor (menor) tempo a partir do conjunto S ja escolhido (multi-fonte)
    //    dV = tempo a partir do candidato v
    vector<int> Ssrc(S.begin(), S.end());
    vector<int> dS = S.empty() ? vector<int>(n, INF) : bfs(Ssrc);
    vector<int> dV = bfs({v});

    // 4) marginal gain: conta w (que C infectaria, dC < INF) que v salva e S nao.
    //    w salvo por um conjunto sse (menor dist de L ate w) <= dC[w].
    int ganho = 0;
    for (int w = 0; w < n; w++) {
        if (dC[w] == INF) continue;        // C nem chega -> nao tem o que salvar

        bool S_salva = (dS[w] != INF) && (dS[w] <= dC[w]);
        bool v_salva = (dV[w] != INF) && (dV[w] <= dC[w]);

        if (v_salva && !S_salva) ganho++;  // v salva alguem que S sozinho nao salvava
    }
    return ganho;
}

/* 
   GREEDY (Figura 2 do paper)

   1: dado (G, AC=Sa, r, k)
   2: AL <- vazio,  R <- #sim
   3: para i = 1..k:
   4:   pra cada v em N - AL:
   5:     sv <- 0
   6:     para j = 1..R:
   7:       sv += InfLimit(G, Sa, r, AL, v)
   8:     sv <- sv / R
   9:   AL <- AL U { argmax_v sv }
   10: devolve AL
   */
set<int> greedy(int n, vector<pair<int, double>> out_adj[], set<int> &Sa,  int k, int R) {
    set<int> AL;                                       // linha 2: AL <- vazio

    for (int i = 0; i < k; i++) {                      // linha 3: i = 1..k
        double melhor_sv = -1;                         // pra guardar o argmax
        int melhor_v = -1;

        for (int v = 0; v < n; v++) {                  // linha 4: v em N - AL
            if (AL.count(v)) continue;

            double sv = 0;                             // linha 5
            for (int j = 0; j < R; j++)                // linha 6: j = 1..R
                sv += InfLimit(n, out_adj, Sa, AL, v); // linha 7: soma o ganho marginal
            sv /= R;                                   // linha 8: media -> #salvos esperado

            if (sv > melhor_sv) { melhor_sv = sv; melhor_v = v; }
        }

        if (melhor_v == -1) break;                     // nao sobrou nada para o algoritmo
        AL.insert(melhor_v);                           // linha 9: AL <- AL U {argmax}
    }

    return AL;                                          // linha 10
}

signed main () {

    int n, m;
    cin >> n >> m;

    vector<pair<int, double>> out_adj[n], in_adj[n];

    for (int i = 0; i < m; i++) {
        int a, b; double p;
        cin >> a >> b >> p;        // aresta que sai de a e entra em b, prob p
        a--, b--;
        out_adj[a].push_back({b, p});
        in_adj[b].push_back({a, p});
    }

    set<int> Sa;                                       // adversarios (AC). caso simples: so o na
    int sa_size; 
    cin >> sa_size;
    for (int i = 0; i < sa_size; i++) { 
        int a; cin >> a; Sa.insert(--a);
     }

    int r, k, R;                                       // delay, orcamento, carlos
    cin >> r >> k >> R;

    set<int> AL = greedy(n, out_adj, Sa,k, R);     // roda o greedy -> seeds de L

    // saida: as seeds escolhidas pra L, de volta em 1-indexado
    for (int v : AL) cout << v + 1 << " ";
    cout << "\n";

    return 0;
}
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
set<int> griddy(int n, vector<pair<int, double>> out_adj[], set<int> &Sa,  int k, int R) {
    set<int> AL;                                       // linha 2: AL <- vazio

    for (int i = 0; i < k; i++) {                      // linha 3: i = 1..k
        double melhor_sv = -1;                         // pra guardar o argmax
        int melhor_v = -1;

        for (int v = 0; v < n; v++) {                  // linha 4: v em N - AL
            if (AL.count(v) || Sa.count(v)) continue; //pov ele poderia usar o adversario como seed

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

/*
   CELF - uma ruma de gente et al
   "Cost-effective Outbreak Detection in Networks", KDD 2007.
    vamos usar da submodularidade do problema: se v eh o argmax de delta(v) = sv(v | AL), entao
    delta(v) >= delta(w) para todo w em N - AL.

   1: dado (G, AC=Sa, k, R)
   2: AL <- vazio
   3: pra cada v em N - Sa:                  // 1a rodada: calcula todo mundo UMA vez
   4:   delta[v] <- sv(v | AL)               // sv = media de R chamadas de InfLimit (= linhas 5-8 do greedy)
   5:   calculado_em[v] <- 0                 // |AL| de quando delta[v] foi calculado
   6:   poe {delta[v], v} na fila Q          // Q = max-heap por delta
   7: para i = 1..k:
   8:   repete:
   9:     v <- topo de Q
   10:     se calculado_em[v] == |AL|:        // delta[v] ainda esta "fresco"
   11:       AL <- AL U {v}; tira v de Q; sai // eh o argmax de fato
   12:     senao:                             // desatualizado: recalcula SO o topo
   13:       delta[v] <- sv(v | AL); calculado_em[v] <- |AL|; reordena v em Q
   14: devolve AL

   pdi = pair<double,int> (a priority_queue ordena por .first, max-heap).
   */
set<int> griddy_celf(int n, vector<pair<int, double>> out_adj[], set<int> &Sa, int k, int R) {
    set<int> AL;                                       // linha 2: AL <- vazio

    // sv(v | AL): ganho marginal medio de v sobre o AL atual 
    auto sv_de = [&](int v) {
        double sv = 0;
        for (int j = 0; j < R; j++){
             sv += InfLimit(n, out_adj, Sa, AL, v);
             cout << "itr: " << j << " sv: " << sv << "\n";
        }
        return sv / R;
    };

    priority_queue<pdi> pq;                            // Q: 
    vector<int> calculado_em(n, -1);                   // |AL| de quando delta[v] foi calculado

    for (int v = 0; v < n; v++) {                      // linha 3: pra cada v em N - Sa
        if (Sa.count(v)) continue;                     // nao usa adversario como seed
        pq.push({sv_de(v), v});                        // linhas 4 e 6: delta[v] e poe na fila
        calculado_em[v] = 0;                           // linha 5: calculado com |AL| = 0
    }

    for (int i = 0; i < k; i++) {                      // linha 7: i = 1..k
        while (!pq.empty()) {                          // linha 8: repete
            pdi topo = pq.top();                       // linha 9: v <- topo de Q
            int v = topo.second;

            if (calculado_em[v] == (int)AL.size()) {   // linha 10: delta[v]
                pq.pop();
                AL.insert(v);                          // linha 11: AL <- AL U {argmax}
                break;
            }

            pq.pop();                                  // linhas 12-13: = recalcula SO o topo
            topo.first = sv_de(v);
            calculado_em[v] = AL.size();
            pq.push(topo);
        }

        if ((int)AL.size() != i + 1) break;            // fila esvaziou: nao sobrou candidato
    }

    return AL;                                          // linha 14: devolve AL
}

signed main () {

    auto old = chrono::steady_clock::now();

    //   n = vertices, m = arestas, k = orcamento de seeds positivas, l = num. de seeds negativas
    int n, m, k, l;
    cin >> n >> m >> k >> l;

    vector<pair<int, double>> out_adj[n];

    for (int i = 0; i < m; i++) {
        int a, b; double p;
        cin >> a >> b >> p;        // aresta que sai de a e entra em b, prob p 
        out_adj[a].push_back({b, p});
    }

    //mysery enemies
    set<int> Sa;
    for (int i = 0; i < l; i++) {
        int a; cin >> a; Sa.insert(a);   
    }

    int R = 100; // numero de simulacoes de monte carlo para estimar o ganho marginal

    set<int> AL = griddy_celf(n, out_adj, Sa, k, R);   // CELF agora 

    auto duracao = chrono::steady_clock::now() - old;

    for (int v : AL) cout << v << " ";
    cout << "\n";

    ofstream output;

    output.open("output_meio/greedy-seeds.txt");

    for (int v : AL) output << v << " ";

    output.close();

    output.open("output_meio/greedy-time-nanoseconds.txt");

    output << chrono::duration_cast<chrono::nanoseconds>(duracao).count() << "\n";

    output.close();

    return 0;
}
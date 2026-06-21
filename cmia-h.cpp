#include <bits/stdc++.h>
using namespace std;
#define int long long
int INF = LLONG_MAX;
double INFD = DBL_MAX;
using pii = pair<int, int>;
using pdi = pair<double, int>;
int vertices;

bool montandonegs = true;

double theta, thetas[5] = {0.0025, 0.005, 0.01, 0.02, 0.05};

set<int> negative_seeds;
set<int> final_seeds, neg_s;

struct MIA{
    vector<double> distance;
    vector<int> pai;
    vector<bool>alcanca;
};

struct Mioa : public MIA {

};

struct Miia : public MIA {
    vector<double> activation_probabilities;
};

struct PIS{
    vector<bool> ta;
    set<int> who;
};

struct Piis : public PIS {

};

struct Pios : public PIS{

};


MIA build_mia(vector<pair<int, double>> out_adj[], int vertex, MIA &mioa) { // dijkstra kinda

    // sempre que eu alcançar alguém, eu testo se posso adicionar ele em NegS (nao ja ta, nao ta nas seeds iniciais)
    mioa.distance.assign(vertices, INFD);
    mioa.pai.assign(vertices, -1);
    mioa.alcanca.assign(vertices, 0); // no que que isso ajuda kinda

    mioa.distance[vertex] = 0;
    priority_queue<pdi,vector<pdi>, greater<pdi>> q;
    q.push({0, vertex});

    while(!q.empty()) {
        int v = q.top().second;
        double d_v = q.top().first;
        q.pop();

        if (d_v != mioa.distance[v]) continue;

        // note to self: o out adj ta com node, distance; aqui preciso acessar inverso pra a fila de prioridade ordenar direitinho
        for (int i = 0; i < out_adj[v].size(); i++) {

            int to = out_adj[v][i].first; // checar isso aqqqqq
            double len = -1*log(out_adj[v][i].second);


            // se nao foi alcancado e nao faz parte das negative seeds
            /*if (neg_s.count(out_adj[v][i].second) == 0 && negative_seeds.count(out_adj[v][i].second) == 0) {
                neg_s.insert(out_adj[v][i].second);
            }*/

            // seta todos os que sao alcancaveis e tem uma prob menor ou igual a theta
            if (mioa.alcanca[out_adj[v][i].first] == 0 && min(mioa.distance[v] == INFD ? mioa.distance[v] : mioa.distance[v] + len, mioa.distance[out_adj[v][i].second]) <= -1*log(theta)) {
                mioa.alcanca[out_adj[v][i].first] = true;

                if (negative_seeds.count(out_adj[v][i].first) == 0 && montandonegs) { // isso era pra tirar a redundancia mas meio que fodase se uma outra negative atingir isso aq ele tenta add de novo
                    neg_s.insert(out_adj[v][i].first);
                }
            }
            

            if (mioa.distance[v] + len < mioa.distance[to]) {
                mioa.distance[to] = mioa.distance[v] + len; // soma de logn = multiplicacao!!!
                mioa.pai[to] = v;
                q.push({mioa.distance[to], to});
            }

        }

    }

    return mioa;

}

/*int build_D(vector<pair<int, double>> in_adj[], int vertex, MIA miia, set<int> &D, bool bols[], int &dnmax, int L = 0, bool waaait4me = true) {

    if (waaait4me && final_seeds.count(vertex) == 1) {
        dnmax = vertex;
        waaait4me = false;
    }

    if (L != 0 && negative_seeds.count(vertex) == 1) { // so addamos à lista de distancias se for negativo
        D.insert(L);
    }


    for (auto e : in_adj[vertex]) {

        if (bols[e.first] == 1) continue;

        build_D(in_adj, e.first, miia, D, bols);
    }

}*/


void build_D(vector<pair<int, double>> in_adj[], int vertex, MIA miia, set<int> &D, bool bols[], int &dnmax, queue<pii> q ={}, int L = 0, bool waaait4me = true) {

    q.push({vertex, (long long)0});
    bols[vertex] = 1;

    while (q.size()) {

        auto a = q.front();
        q.pop();

        for (auto viz : in_adj[a.first]) {

            if (bols[viz.first] == 1) continue;
            bols[viz.first] = 1;

            if (a.second+1 != 0 && negative_seeds.count(viz.first) == 1) { // so addamos à lista de distancias se for negativo
                D.insert(a.second+1);
            }

            if (waaait4me && final_seeds.count(viz.first) == 1) {
                dnmax = a.second; // menor distancia ate positivo - 1
                waaait4me = false;
            }

            q.push({viz.first, a.second+1});
        }

    }

}


// POR ENQUANTO LENGTH AINDA NAO IMPLEMENTA NADA!!!!!!!!!!!!!
// tenho que ver isso ai


// isso aq eh uma dp para calcular as activation probabilities
double ap(int v, vector<pair<int, double>> in_adj[], Miia &miiau, int length, set<int> conj = negative_seeds) {
    //miiau.activation_probabilities[length].assign(vertices, 0);
    

    if (negative_seeds.count(v) == 1) { // se eh seed negativa, 1.0
        miiau.activation_probabilities[v] = 1;
    }
    else if (miiau.alcanca[v] == 0) { // se nao eh atingivel
        miiau.activation_probabilities[v] = 0;
    }
    else {
        double produtorio = 1;
        for (int w = 0; w < in_adj[v].size(); w++) {
            produtorio *= (1 - ap(in_adj[v][w].first, in_adj, miiau, length, conj) * in_adj[v][w].second); // isso nao vai sempre zerar porque eh bottom up; a partir do momento que um zera, faz sentido o resto das possibilidades que tentam zerar (mais distante)
        }
        miiau.activation_probabilities[v] = 1 - produtorio;
    }
    return miiau.activation_probabilities[v];
}


void build_piis(vector<pair<int, double>> in_adj[], int vertex, int dmax, PIS piis, bool bols[] = {0},queue<pii> q ={}) {

    q.push({vertex, (long long)0});
    bols[vertex] = 1;

    while (q.size() && q.front().second != dmax) { // so visita os que tem dist menor que dmax

        auto a = q.front();
        q.pop();
        if (a.second == dmax) break;

        for (auto viz : in_adj[a.first]) {

            if (bols[viz.first] == 1) continue;
            bols[viz.first] = 1;

            piis.ta[viz.first] = true;
            piis.who.insert(viz.first);

            q.push({viz.first, a.second+1});
        }

    }

}


signed main () {

    // primeiro vou receber as infos do grafo como se fosse uma questao de maratona

    int n, m, k;

    cin >> n >> m >> k >> theta;
    vertices = n;

    vector<pair<int, double>> out_adj[n], log_out_adj[n];
    vector<pair<int,double>> in_adj[n], log_in_adj[n];

    double DecInf[n] = {0};
    int dc[n] = {INF}; // might have to change this tho...... no pseudocodigo ele considera
                        // o estado como sendo vertice, conjunto atual das seeds positivas Sp
                        // talvez bitset seja a melhor opção?

    for (int i = 0; i < m; i++) {
        int a, b;
        double weight;
        cin >> a >> b >> weight; // edge que vai de a a b (sai de a(out), entra em b(in))
        a--,b--;

        // adding the edge to both a's out-adjacency list and b's in-adjacency list
        in_adj[b].push_back({a, weight});
        out_adj[a].push_back({b, weight});

    }

    // recebe as negative seeds

    for (int i = 0; i < k; i++) {
        int a;
        cin >> a;
        negative_seeds.insert(--a);

    }


    map<int, MIA> out_mias_arvores;

    // construindo todas as MIOAs e colocando elas num set
    for (auto a = negative_seeds.begin(); a != negative_seeds.end(); a++){
        Mioa mioa;
        build_mia(out_adj, *(a), mioa);
        out_mias_arvores[*a] = mioa;
        // NegS eh constuido dentro da funcao MIOA
    }



    montandonegs = false;
    Piis meus_piis[n];

    for (auto a = neg_s.begin(); a != neg_s.end(); a++) {
        // contruir MIIA
        // construir o set das distancias

        bool bols[n] = {0};

        set<int> D; // distinct distances to our vertex in question

        Miia miia;

        build_mia(in_adj, (*a), miia);
        miia.alcanca[(*a)] = true; // tem que alcancar a si mesmo pra nao dar bo no algoritmo de ap

        int dnmax = INF;

        build_D(in_adj, (*a), miia, D, bols, dnmax);

        // como D se refere a distancias entre dois vertices estabelecidas por uma BFS,
        // a distancia maxima eh menor ou igual a N-1

        // logo, teremos N-1 ou menos aps

        // DNMAX = MAIOR DISTANCIA MENOR QUE UMA POSITIVE SEED; COMECA COMO INF

        
        
        for (auto itr = D.begin(); itr != D.end(); itr++) {

            miia.activation_probabilities.assign(n, 0); // qtd de tamamhos

            // porque que eu preciso construir isso pra todo mia? nao vai dar as mesmas probs nao...?
            ap((*a), in_adj, miia, (*itr));

        }


        build_piis(in_adj, (*a), dnmax, meus_piis[*a]);

        // eu uso um array de presenca ou um set? array eh sempre theta n mas o set pode ser bem menos se alguns nao forem alcancaveis

        for (auto itr = meus_piis[*a].who.begin(); itr != meus_piis[*a].who.end(); itr++) {
            //DecInf[*itr] += (miia.activation_probabilities[*a] /*- miia.activation_probabilities*/)
            // agora eu busco a distancia minima e recalculo
        }

        // aqui eu faço uma bfs para encontrar todas as distâncias dos nós negativos para o u

        /*for (int i = 0; i < algo; i++) {
            ap((*a), in_adj, miia, i);

        }*/
    }





    return 0;
}
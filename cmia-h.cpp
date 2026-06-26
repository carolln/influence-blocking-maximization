#include <bits/stdc++.h>
#include <chrono>
#include <fstream>
using namespace std;
#define int long long
int INF = LLONG_MAX;
double INFD = DBL_MAX;
using pii = pair<int, int>;
using pdi = pair<double, int>;
int vertices;
vector<double> DecInf;

bool montandonegs = true;

float theta = 0.002;
vector<float>thetas = {0.0025, 0.005, 0.01, 0.02, 0.05};

//veroegocioladonopositivo ou negativo somethibg lije that



set<int> negative_seeds;
set<int> final_seeds, neg_s;

struct MIA{
    vector<double> distance;
    vector<int> pai;
    vector<bool>alcanca;
    vector<int> weightless_distance;
    vector<int> sorted_negative_seeds;
};

struct Mioa : public MIA {

};

struct Miia : public MIA {
    vector<double> activation_probabilities;
};

struct PIS{
    vector<bool> ta;
    set<int> who;
    vector<int> distances;
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
        //if (v != vertex && negative_seeds.count(v) == 1) continue; // se encontrar uma seed negativa, nao precisa mais ir pra trás

        // note to self: o out adj ta com node, distance; aqui preciso acessar inverso pra a fila de prioridade ordenar direitinho
        for (int i = 0; i < out_adj[v].size(); i++) {

            int to = out_adj[v][i].first; // checar isso aqqqqq
            double len = -1*log(out_adj[v][i].second);

            // se nao foi alcancado e nao faz parte das negative seeds
            /*if (neg_s.count(out_adj[v][i].second) == 0 && negative_seeds.count(out_adj[v][i].second) == 0) {
                neg_s.insert(out_adj[v][i].second);
            }*/

            // seta todos os que sao alcancaveis e tem uma prob menor ou igual a theta
            if (mioa.alcanca[out_adj[v][i].first] == 0 && min(mioa.distance[v] + len, mioa.distance[to]) <= -1*log(theta)) {
                mioa.alcanca[out_adj[v][i].first] = true;

                if (montandonegs && negative_seeds.count(out_adj[v][i].first) == 0) { // isso era pra tirar a redundancia mas meio que fodase se uma outra negative atingir isso aq ele tenta add de novo
                    neg_s.insert(out_adj[v][i].first);
                }
            }
            

            if (mioa.distance[v] + len < mioa.distance[to]) {
                mioa.distance[to] = mioa.distance[v] + len; // soma de logn = multiplicacao!!!
                mioa.pai[to] = v;
                if (negative_seeds.count(to) == 0 && mioa.alcanca[to]) { // so seguimos para avaliar os vizinhos se nao for negativo!!
                    q.push({mioa.distance[to], to});
                }
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


void build_D(vector<pair<int, double>> in_adj[], int vertex, MIA &miia, set<int> &D,
     vector<bool> &bols, int &dnmax, queue<pii> q ={}, int L = 0, bool waaait4me = true) {

    q.push({vertex, (long long)0});
    bols[vertex] = 1;
    miia.weightless_distance.assign(vertices, -1);
    miia.sorted_negative_seeds.reserve(vertices);
    miia.weightless_distance[vertex] = 0;

    while (q.size()) {

        auto a = q.front();
        q.pop();

        for (auto viz : in_adj[a.first]) {

            if (bols[viz.first] == 1) continue;
            bols[viz.first] = 1;

            // CHECAR ISSO AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
            if (miia.alcanca[viz.first] && negative_seeds.count(viz.first) == 1) {
                miia.sorted_negative_seeds.push_back(a.second+1);
            }

            miia.weightless_distance[viz.first] = a.second+1; // construindo distancia sem pesos para usarmos mais pra frente

            if (a.second+1 != 0 && negative_seeds.count(viz.first) == 1) { // so addamos à lista de distancias se for negativo
                D.insert(a.second+1);
            }

            if (waaait4me && final_seeds.count(viz.first) == 1) { // procurando uma seed positiva
                dnmax = a.second; // menor distancia ate positivo - 1
                waaait4me = false;
            }

            q.push({viz.first, a.second+1});
        }

    }

    miia.weightless_distance;
    sort(miia.sorted_negative_seeds.begin(), miia.sorted_negative_seeds.end());

}


// POR ENQUANTO LENGTH AINDA NAO IMPLEMENTA NADA!!!!!!!!!!!!!
// tenho que ver isso ai


// isso aq eh uma dp para calcular as activation probabilities
double ap(int v, vector<pair<int, double>> in_adj[], Miia &miiau, int length, set<int> conj = negative_seeds) {
    //miiau.activation_probabilities[length].assign(vertices, 0);
    

    if (negative_seeds.count(v) == 1) { // se eh seed negativa, 1.0
        miiau.activation_probabilities[v] = 1;
    }
    else if (miiau.alcanca[v] == 0 || miiau.weightless_distance[v] > length) { // se nao eh atingivel / distancia maior que limite colocado
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

int find_max_decinf(vector<double> decinf) {

    double maximo = -1;
    int where = -1;// recomputo piis aqui

    for (int i = 0; i < vertices; i++) {
        if (decinf[i] > maximo && negative_seeds.count(i) == 0 && final_seeds.count(i) == 0) {
            maximo = decinf[i];
            where = i;
        }
    }

    return where;

}

// dmax aqui eh a menor distancia positica ou a maior negativa possivel?
void build_piis(vector<pair<int, double>> in_adj[], int vertex, int dmax, PIS &piis, vector<bool> bols = {0},queue<pii> q ={}) {

    q.push({vertex, (long long)0});
    //piis.distances.reserve(vertices);
    piis.distances.assign(vertices,-1);
    piis.distances[vertex] = 0;
    bols.assign(vertices,false);
    bols[vertex] = 1;
    piis.ta.assign(vertices,0);
    piis.who.clear();

    while (q.size() && q.front().second < dmax) { // so visita os que tem dist menor que dmax

        auto a = q.front();
        q.pop();
        if (a.second >= dmax) break;

        for (auto viz : in_adj[a.first]) {

            if (bols[viz.first] == 1) continue;
            bols[viz.first] = 1;
            if (negative_seeds.count(viz.first) == 1) continue; // VER SE ISSO FAZ SENTIDO
            // cause like..... a gente nao vai ter acesso a uma seed nao inicializada se tiver uma
            // negativa no meio do caminho

            piis.distances[viz.first] = a.second+1;

            piis.ta[viz.first] = true;
            piis.who.insert(viz.first);

            q.push({viz.first, a.second+1});
        }

    }

}




signed main () {

    // primeiro vou receber as infos do grafo como se fosse uma questao de maratona

    auto old = chrono::steady_clock::now();

    int n, m, k, l;

    cin >> n >> m >> k >> l;
    vertices = n;

    if (n < k + l) {
        cout << "pfv coloque entr.assignadas que facam sentido; nao da pra ter mais seeds do que vertices\n";
    }

    vector<pair<int, double>> out_adj[n], log_out_adj[n];
    vector<pair<int,double>> in_adj[n], log_in_adj[n];

    vector<vector<double>> apn(n, vector<double>(n, 0)); // cada no tem ate n probabilidades (distancias that can reach n)

    
    DecInf.assign(n, 0);
    vector<int> dc(n, INF); // might have to change this tho...... no pseudocodigo ele considera
                        // o estado como sendo vertice, conjunto atual das seeds positivas Sp
                        // talvez bitset seja a melhor opção?

    for (int i = 0; i < n; i++) { // a inicializacao de cima nao estava funcionando for some reason
        dc[i] = INF;
    }

    for (int i = 0; i < m; i++) {
        int a, b;
        double weight;
        cin >> a >> b >> weight; // edge que vai de a a b (sai de a(out), entra em b(in))
        //a--,b--; o dataset ja ta 0 indexado

        // adding the edge to both a's out-adjacency list and b's in-adjacency list
        in_adj[b].push_back({a, weight});
        out_adj[a].push_back({b, weight});

    }

    // recebe as negative seeds

    for (int i = 0; i < l; i++) {
        int a;
        cin >> a;
        negative_seeds.insert(a);

    }


    map<int, MIA> out_mias_arvores;

    // construindo todas as MIOAs e colocando elas num set
    for (auto a = negative_seeds.begin(); a != negative_seeds.end(); a++){
        Mioa mioa;
        build_mia(out_adj, *(a), mioa);
        out_mias_arvores[*a] = mioa;
        // NegS eh constuido dentro da funcao MIOA
    }


    int dnmax;



    montandonegs = false;
    Piis meus_piis[n];
    map<int, MIA> in_miias_arvores;

    for (auto a = neg_s.begin(); a != neg_s.end(); a++) {
        // contruir MIIA
        // construir o set das distancias

        vector<bool> bols(n, false);

        set<int> D; // distinct distances to our vertex in question

        Miia miia;

        build_mia(in_adj, (*a), miia);
        miia.alcanca[(*a)] = true; // tem que alcancar a si mesmo pra nao dar bo no algoritmo de ap

        
        int dnmaxx = INF;

        build_D(in_adj, (*a), miia, D, bols, dnmaxx);

        // como D se refere a distancias entre dois vertices estabelecidas por uma BFS,
        // a distancia maxima eh menor ou igual a N-1

        // logo, teremos N-1 ou menos aps

        // DNMAX = MAIOR DISTANCIA MENOR QUE UMA POSITIVE SEED; COMECA COMO INF

        // quando a distância de um positivo é 1, a probabilidade de ativacao negativa de um nó é 0!
        apn[(*a)][0] = 0;
        
        for (auto itr = D.begin(); itr != D.end(); itr++) {

            miia.activation_probabilities.assign(n, 0); // qtd de tamamhos

            // porque que eu preciso construir isso pra todo mia? nao vai dar as mesmas probs nao...?
            apn[(*a)][*itr] = ap((*a), in_adj, miia, (*itr));
            // isso aq ta tipo theta(n*m) pra cada iteração. checar se eu não posso otimizar isso aq de alguma forma

        }


        build_piis(in_adj, (*a), dnmaxx, meus_piis[*a]);

        // eu uso um array de presenca ou um set? array eh sempre theta n mas o set pode ser bem menos se alguns nao forem alcancaveis

        in_miias_arvores[*a] = miia;

        //dnmax = dnmaxx;
        dnmax = *(--D.end()); // maior distancia menor que uma positiva (ainda nao temos positivas)
        

        // posso guardar as distancias em comparacao a u no meu piis pra eu nao precisar fazer bfs desnecessarias yk
        for (auto itr = meus_piis[*a].who.begin(); itr != meus_piis[*a].who.end(); itr++) {


            // caso adicionassemos o itr no nosso conjunto de seeds positivas, o dnmax
            // seria o minimo entre o dnmax atual (maior seed negativa atingida, calculada com o D)
            // e a maior distancia menor que a distancia entre a e o nó que estamos avaliando agora

            int outro;

            auto pera = upper_bound(in_miias_arvores[*a].sorted_negative_seeds.begin(), in_miias_arvores[*a].sorted_negative_seeds.end(), meus_piis[*a].distances[*itr]-1);

            // se o primeiro já for maior, fodase vai ser infinito
            if (in_miias_arvores[*a].sorted_negative_seeds[0] > meus_piis[*a].distances[*itr]-1) {
                outro = 0;
            }
            else { // EXISTE ALGUM!!!!!
                outro = *(pera-1);
            }

            DecInf[*itr] += apn[*a][dnmax] - apn[*a][min(meus_piis[*a].distances[*itr], outro)];
            
    
        }

        // aqui eu faço uma bfs para encontrar todas as distâncias dos nós negativos para o u

        /*for (int i = 0; i < algo; i++) {
            ap((*a), in_adj, miia, i);

        }*/
    }

    for (int i = 0; i < k; i++) {

        // encontrar o de max decinf
        // atualizar tudo ao redor dele antes de tirar
        // adicionar ao conjunto positivo

        int escolhido = find_max_decinf(DecInf);

        // o dnmax depende do no escolhido que odioooooooo

        Pios pios;

        build_piis(out_adj, escolhido, dnmax, pios); // no pios nao importa a distancia

        for (auto itr = pios.who.begin() ; itr != pios.who.end(); itr++) {

            
            for (auto itr2 = meus_piis[*itr].who.begin(); itr2 != meus_piis[*itr].who.end(); itr2++) {

                // posso passar a guardar todas as seeds negativas atingidas num vetor da miia ordenada por distancia
                // ai faco uma busca binaria lowerbound com base na dc[vertice]
                // talvez precise de um minimo em algum lugar

                // essa gambiarra aqui de baixo é porque o upperbound retorna um iterador para o primeiro lugar onde o valor pode ser inserido
                
                int outro;

                auto pera = upper_bound(in_miias_arvores[*itr].sorted_negative_seeds.begin(), in_miias_arvores[*itr].sorted_negative_seeds.end(), dc[*itr]-1);

                // se o primeiro já for maior, fodase vai ser infinito
                if (in_miias_arvores[*itr].sorted_negative_seeds[0] > dc[*itr]-1) {
                    outro = 0;
                }
                else { // EXISTE ALGUM!!!!!
                    outro = *(pera-1);
                }


                int outro2;

                auto pera2 = upper_bound(in_miias_arvores[*itr].sorted_negative_seeds.begin(), in_miias_arvores[*itr].sorted_negative_seeds.end(), min(in_miias_arvores[*itr].weightless_distance[*itr2]-1, dc[*itr]-1));

                // se o primeiro já for maior, fodase vai ser infinito
                if (in_miias_arvores[*itr].sorted_negative_seeds[0] > min(in_miias_arvores[*itr].weightless_distance[*itr2]-1, dc[*itr]-1)) {
                    outro2 = 0;
                }

                else { // EXISTE ALGUM!!!!!
                    outro2 = *(pera2-1);
                }


                DecInf[*itr2] -= apn[*itr][outro] - apn[*itr][outro2];

                
            }

            // como eu já computei as distancias de cada nó para o nosso u (e também as probs de todas as distâncias atingíveis),
            // e 
    
        }

        final_seeds.insert(escolhido);

        for (auto itr = pios.who.begin() ; itr != pios.who.end(); itr++) {

            if (*itr == escolhido) continue;

            // updatar a menor distancia de seed positiva

            dc[*itr] = min(dc[*itr], in_miias_arvores[*itr].weightless_distance[escolhido]);


            // encontrando a maior seed negativa que eh menor que dc[*itr]
            int outroo;

            auto pera = upper_bound(in_miias_arvores[*itr].sorted_negative_seeds.begin(), in_miias_arvores[*itr].sorted_negative_seeds.end(), dc[*itr]-1);

            // se o primeiro já for maior, fodase vai ser infinito
            if (in_miias_arvores[*itr].sorted_negative_seeds[0] > dc[*itr]-1) {
                outroo = 0;
            }
            else { // EXISTE ALGUM!!!!!
                outroo = *(pera-1);
            }

            build_piis(in_adj, *itr, outroo, meus_piis[*itr]);


            for (auto itr2 = meus_piis[*itr].who.begin(); itr2 != meus_piis[*itr].who.end(); itr2++) {

                int outro;

                auto pera = upper_bound(in_miias_arvores[*itr].sorted_negative_seeds.begin(), in_miias_arvores[*itr].sorted_negative_seeds.end(), dc[*itr]-1);

                // se o primeiro já for maior, fodase vai ser infinito
                if (in_miias_arvores[*itr].sorted_negative_seeds[0] > dc[*itr]-1) {
                    outro = 0;
                }
                else { // EXISTE ALGUM!!!!!
                    outro = *(pera-1);
                }


                int outro2;

                auto pera2 = upper_bound(in_miias_arvores[*itr].sorted_negative_seeds.begin(), in_miias_arvores[*itr].sorted_negative_seeds.end(), min(in_miias_arvores[*itr].weightless_distance[*itr2]-1, dc[*itr]-1));

                // se o primeiro já for maior, fodase vai ser infinito
                if (in_miias_arvores[*itr].sorted_negative_seeds[0] > min(in_miias_arvores[*itr].weightless_distance[*itr2]-1, dc[*itr]-1)) {
                    outro2 = 0;
                }

                else { // EXISTE ALGUM!!!!!
                    outro2 = *(pera2-1);
                }

                DecInf[*itr2] += apn[*itr][outro] - apn[*itr][outro2];
            }

            // como eu já computei as distancias de cada nó para o nosso u (e também as probs de todas as distâncias atingíveis),
            // e 
    
        }


    }


    auto duracao = chrono::steady_clock::now() - old;


    ofstream output;

    output.open("output_meio/cmia-h-seeds.txt");



    for (auto a = final_seeds.begin(); a != final_seeds.end(); a++) {
        output << (*a) << " ";
    }
    
    cout << "\n";

    output.close();

    output.open("output_meio/cmia-h-time-nandatasets/NetHEPT.txtoseconds.txt");

    output << chrono::duration_cast<chrono::nanoseconds>(duracao).count() << "\n";

    output.close();

    return 0;
}
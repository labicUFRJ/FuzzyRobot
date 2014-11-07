typedef struct no{
	int id; /* Vértice */
	int peso;
	struct no *prox;
	double *acao;
} No;

typedef struct grafo {
	int nvertices;      /* Número de nós */
	No** lista;
} Grafo;

void cria_grafo(Grafo* g, int nvertices);
void destroi(Grafo *g);
void adiciona_aresta(Grafo *g, int id1, int id2, int peso,double tamPasso, double giroDir, double giroEsq);
int pesoAresta(Grafo *g, int id1, int id2);
void remove_aresta(Grafo *g, int id1, int id2);
void imprime_grafo(Grafo *g);
void initSingleSource(Grafo *g, int raiz);
void relaxaAresta(int u, int v, int pesoAresta);
void dijkstra(Grafo *g, int raiz,HEAP h);
void reconstroiCaminho(Grafo *g, int inicio, int fim);
void imprimeCaminho(Grafo *g);
void imprimePais(Grafo *g);
void imprimeDistancias(Grafo *g);
void initCaminho(Grafo *g);
void AcaoVertice(Grafo *g,int inicioTrecho, int fimTrecho);
void localizaAcao(Grafo *g, int verticeVisto);

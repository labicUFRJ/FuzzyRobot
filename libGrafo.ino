void cria_grafo(Grafo* g, int nvertices)  { 
	g->lista = (No**)calloc(nvertices, sizeof(No*));
	g->nvertices = nvertices;
}

void destroi(Grafo *g) {
	int i;
	for (i = 0; i < g->nvertices; i++) {
		No* l = g->lista[i]; 
		while (l) {
			No *r = l;
			l = l->prox;
			free(r);
		}
	}
	free(g->lista);
}

void adiciona_aresta(Grafo *g, int id1, int id2, int peso,double tamPasso, double giroDir, double giroEsq) {
	No **ap_l = &g->lista[id1];
	while (*ap_l != NULL && (*ap_l)->id < id2)
		ap_l = &(*ap_l)->prox;
	if (*ap_l == NULL || (*ap_l)->id != id2) {
		No* n = (No*)malloc(sizeof(No));
		n->acao = (double*)calloc(3, sizeof(double));
		n->id = id2;
		n->prox = *ap_l;
		n->peso = peso;
		n->acao[0] = tamPasso;
		n->acao[1] = giroDir;
		n->acao[2] = giroEsq;
		*ap_l = n;
	}
}

int pesoAresta(Grafo *g, int id1, int id2) {
	No *l = g->lista[id1];
	while (l != NULL && l->id < id2)
		l = l->prox;
	if (l != NULL && l->id == id2)
		return l->peso;
	else{
	  Serial.print('não existe aresta ');
          Serial.print(id1);
          Serial.print("->");
          Serial.print(id2);	
          Serial.println();
//        printf("não existe aresta %d->%d\n",id1, id2 );
		return 0;
	}
}

void remove_aresta(Grafo *g, int id1, int id2) {
	No **ap_l = &g->lista[id1];
	while (*ap_l != NULL && (*ap_l)->id < id2)
		ap_l = &(*ap_l)->prox;
	if (*ap_l != NULL && (*ap_l)->id == id2) {
		No* r = *ap_l;
		*ap_l = (*ap_l)->prox;
		free(r);
	}
}

void imprime_grafo(Grafo *g) {
	int i;
	for (i = 0; i < g->nvertices; ++i)
	{
		No **l = &g->lista[i];
		Serial.print("vertice ");
                Serial.print(i);
                Serial.print(", adjacentes: ");
//                printf("vertice %d, adjacentes: ", i);
		while ( *l != NULL){
//			printf("%d ", (*l)->id );
                        Serial.print((*l)->id);
			l = &(*l)->prox;
		}
		Serial.println();
	}
}

void initSingleSource(Grafo *g, int raiz){
	/*para cada vertice v £ V[G]*/
	int i;
	for (i = 0; i < g->nvertices; ++i){
		distNo[i] = INFINITO;
		noPai[i] = -1;
	}
	distNo[raiz] = 0;
}

void relaxaAresta(int u, int v, int pesoAresta){
	//printf("distNo[%d] = %d\n distNo[%d] = %d\n pesoAresta = %d\n",v, distNo[v],u, distNo[u], pesoAresta);
	if (distNo[v] > distNo[u] + pesoAresta ){
		distNo[v] = distNo[u] + pesoAresta;
		noPai[v] = u;
	}
}

void dijkstra(Grafo *g, int raiz,HEAP h){
	int i;
	initSingleSource(g,raiz);
	initHeap(&h,g->nvertices);
	for (i = 0; i < g->nvertices; i++)	inserirHeap(&h,i,distNo[i]);
	construirHeapMinimo(&h);
	while (h.tamanhoAtual > 0 ){
		int u = HeapExtraiMin(&h);
		//printf("vertice:%d\n",u );
		No **l = &g->lista[u];
		while ( *l != NULL){
			//printf("%d ", &g->lista[i]);
			//printf("id:%d\n ", (*l)->id );
			relaxaAresta(u,(*l)->id,pesoAresta(g,u,(*l)->id));
			l = &(*l)->prox;
		}
	}
}

void reconstroiCaminho(Grafo *g, int inicio, int fim){
//	printf("Pai:%d , filho:%d\n", noPai[fim], fim);
//	printf("inicio:%d , fim:%d\n", inicio, fim);
	int i=0, j;
	while(caminho[g->nvertices-i]!= -1) i++;
	caminho[g->nvertices-i] = fim;
	if (noPai[fim]!= -1)	reconstroiCaminho(g, inicio, noPai[fim]);
}

void imprimeCaminho(Grafo *g){
	int i;
	Serial.print("caminho a ser percorrido: ");
	for( i=0; i< g->nvertices; i++)	if(caminho[i]!= -1)	Serial.print(caminho[i]);
	Serial.println();
}

void imprimePais(Grafo *g){
	int i;
	for (i = 0; i < g->nvertices; i++){
//		printf("noPai[%d] = %d\n",i, noPai[i]); 
                Serial.print("filho: ");
                Serial.print(i);
                Serial.print(", pai: ");
                Serial.print(noPai[i]);
		Serial.println();
	}
}

void imprimeDistancias(Grafo *g){
	int i;
	for (i = 0; i < g->nvertices; i++){
//		printf("distNo[%d] = %d\n",i, distNo[i]); 
                Serial.print("distancia ate ");
                Serial.print(i);
                Serial.print("=");
                Serial.print(distNo[i]);
		Serial.println();
	}
}

void initCaminho(Grafo *g){
	int i;
	for( i=0; i<g->nvertices; i++) caminho[i] = -1;
}

void AcaoVertice(Grafo *g,int inicioTrecho, int fimTrecho){
	int i;
	No *l = g->lista[inicioTrecho];
	while (l != NULL && l->id < fimTrecho)
		l = l->prox;
	if (l != NULL && l->id == fimTrecho){
//		printf("Movimentos necessarios para sair do corredor do sensor %d e entrar no corredor do sensor %d:\n",inicioTrecho,fimTrecho );
                Serial.print("Movimentos necessarios para sair do corredor do sensor ");
                Serial.print(inicioTrecho);
                Serial.print(" e entrar no corredor do sensor ");
                Serial.print(fimTrecho);
                Serial.println();
		for(i = 0; i < 3; i++)
			switch(i){
				case 0 :
//				printf("andar para frente %f centímetros\n", l->acao[i]);
                                Serial.print("andar para frente: ");
                                Serial.print(l->acao[i]);
                                Serial.println();
                                motor.forward(l->acao[i]);
                                delay(500);
				break;
				case 1:
//				printf("girar %f graus para direita \n", l->acao[i]);
                                Serial.print("girar para direita: ");
                                Serial.print(l->acao[i]);
                                Serial.println();
                                motor.right(l->acao[i]);
                                delay(500);
				break;
				case 2:
//				printf("girar %f graus para esquerda \n", l->acao[i]);
                                Serial.print("girar para esquerda: ");
                                Serial.print(l->acao[i]);
                                Serial.println();
                                motor.left(l->acao[i]);
                                delay(500);
				break;
				default:break;
			}
                setHorizontal();
	}
	else{
//		printf("não existe aresta %d->%d\n",inicioTrecho, fimTrecho );
                Serial.print("no existe aresta ");
                Serial.print(inicioTrecho);
                Serial.print(",");
                Serial.print(fimTrecho);
                Serial.println();
	}
}

void localizaAcao(Grafo *g, int verticeVisto){
	int i;
	for (i = 0; i < g->nvertices-1; i++){
		if (caminho[i] == verticeVisto)
		{
			AcaoVertice(g, caminho[i], caminho[i+1]);
		}
		
	}
}

void PercorreCaminho(Grafo *g,int destino){
        int verticeVisto = idSensor;
  	if (verticeVisto == destino)
	{
		printf("Cheguei ao fim do caminho\n");
		exit(0);
	}else{
                idSensor = -1;
		localizaAcao(g, verticeVisto);        
	}
}
/*
int main() {
	printf("selecione a origem: ");
	scanf("%d",&origem);
	printf("selecione o destino: ");
	scanf("%d",&destino);

	dijkstra(&g,origem,h);
	reconstroiCaminho(&g, origem, destino);
	imprimeCaminho(&g);
	//imprimePais(&g);
	//imprimeDistancias(&g);
	testaPercorreCaminho(&g, destino);

	destroi(&g);
	destruirHeap(&h);
	return 0;
}*/

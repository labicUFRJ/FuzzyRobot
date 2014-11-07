void initHeap(HEAP * h, int tamanhoMax){
	h->elem.A = (int*) malloc(sizeof(int)*(tamanhoMax+1));
	h->elem.peso = (int*) malloc(sizeof(int)*(tamanhoMax+1));
	h->tamanhoAtual = 0;
	h->tamanhoMaximo = tamanhoMax;
}

void destruirHeap(HEAP * h){
	int tamanho = h->tamanhoMaximo;
	free(h->elem.A);
	h->tamanhoMaximo=0;
	h->tamanhoAtual=0;
}

int pai(int i){
	return i/2;
}

int filhoEsquerda(int i){
	return 2*i;
}

int filhoDireita(int i){
	return 2*i + 1;
}

void minHeapify(HEAP * h, int i){
	int esq = filhoEsquerda(i);
	int dir = filhoDireita(i);
	int temp;
	int menor = i;

	if ((esq <= h->tamanhoAtual) && (h->elem.peso[esq]<h->elem.peso[i]))
		menor = esq;
	if ((dir <= h->tamanhoAtual) && (h->elem.peso[dir]<h->elem.peso[menor])) 
		menor = dir;
	if (menor != i) {
		temp = h->elem.A[i];
		h->elem.A[i] = h->elem.A[menor];
		h->elem.A[menor] = temp;

		temp = h->elem.peso[i];
		h->elem.peso[i] = h->elem.peso[menor];
		h->elem.peso[menor] = temp;
		minHeapify(h,menor);
	}
}

// Constroi heap a partir do arranjo usando o metodo minHeapify
void construirHeapMinimo(HEAP * h){
	int i;
	int metadeTamanho = h->tamanhoAtual/2;
	for (i=metadeTamanho;i>0;i--) minHeapify(h,i);
}

// Insere no final do arranjo do heap
boolean inserirForaDeOrdem(HEAP * h, int valor){
	if (h->tamanhoAtual < h->tamanhoMaximo){
		(h->tamanhoAtual)++;
		h->elem.A[h->tamanhoAtual] = valor;
		return true;
	}
	return false;
}

// Imprime o arranjo (na ordem que estiver)
void imprimirArranjo(HEAP h){
	int tamanho = h.tamanhoAtual;
	int i;
	for (i=1;i<=tamanho;i++) Serial.print(h.elem.A[i]);
	Serial.println();
	for (i=1;i<=tamanho;i++) Serial.print(h.elem.peso[i]);
	Serial.println();
}

// Imprime elementos em ordem decrescente e esvazia o heap
void heapSort(HEAP * h){
	int tamanho = h->tamanhoAtual;
	int i, temp;
	construirHeapMinimo(h);  // se o arranjo jah for um heap, nao precisa desta linha
	for (i=tamanho;i>1;i--){
		temp = h->elem.A[1];
		h->elem.A[1] = h->elem.A[i];
		h->elem.A[i] = temp;
		//printf("%d ",temp);
		(h->tamanhoAtual)--;
		minHeapify(h,1);
	}
	//printf("\n");
	h->tamanhoAtual = tamanho;
}

boolean inserirHeap(HEAP * h, int id, int peso){
	int i, temp;
	if (h->tamanhoAtual == h->tamanhoMaximo) return false;
	(h->tamanhoAtual)++;
	i = h->tamanhoAtual;
	h->elem.A[i] = id;
	h->elem.peso[i] = peso;
	while ((i>1) && (h->elem.peso[pai(i)]<h->elem.peso[i])){
		temp = h->elem.A[i];
		h->elem.A[i] = h->elem.A[pai(i)];
		h->elem.A[pai(i)] = temp;       
		temp = h->elem.peso[i];
		h->elem.peso[i] = h->elem.peso[pai(i)];
		h->elem.peso[pai(i)] = temp;       
		i = pai(i);
	}
	return true;
}

int heapMin(HEAP *h){
	return h->elem.A[1];
}

int HeapExtraiMin(HEAP *h){
	int minimo;
	if (h->tamanhoAtual < 1)
	{
		Serial.println("heap underflow");
	}
	minimo = h->elem.A[1];
	h->elem.A[1]= h->elem.A[h->tamanhoAtual];
	h->elem.peso[1]= h->elem.peso[h->tamanhoAtual];
	h->tamanhoAtual--;
	minHeapify(h,1);
	return minimo;
}

typedef struct {
	int *A;
	int *peso;
}Elem;

typedef struct {
	Elem elem;
	int tamanhoAtual;
	int tamanhoMaximo;
} HEAP;

void initHeap(HEAP * h, int tamanhoMax);
void destruirHeap(HEAP * h);
int pai(int i);
int filhoEsquerda(int i);
int filhoDireita(int i);
void minHeapify(HEAP * h, int i);
void construirHeapMinimo(HEAP * h);
boolean inserirForaDeOrdem(HEAP * h, int valor);
void imprimirArranjo(HEAP h);
void heapSort(HEAP * h);
boolean inserirHeap(HEAP * h, int id, int peso);
int heapMin(HEAP *h);
int HeapExtraiMin(HEAP *h);

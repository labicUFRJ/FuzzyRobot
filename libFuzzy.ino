// Esse arquivo contem as funcoes da biblioteca fuzzy do LabIC.
// Nada aqui deve ser modificado.
// Mario Cecchi - 2011- LabIC - UFRJ

// #include "libFuzzy.h"

//#define NUMCONJUNTOS

void rodarSistema(float x[], float y[], Variavel var[], Regra regras[]) {
	float pesoConjuntos[NUMOUTPUTS][NUMCONJUNTOS]; // Matriz que vai guardar os pesos de cada conjunto de cada saida
	float saidaConjuntos[NUMOUTPUTS][INTERVALO]; // Matriz que vai guardar os pontos y maximos de cada saida
	int i;

	//puts("\n--Iniciando sistema fuzzy --\n");
	//puts("Entradas:");

	for (i=0; i < NUMINPUTS; i++) {
		//printf("%s = %f\n", var[i].nome, x[i]);
	}

	//puts("");
	
	//avaliarRegras(x, pesoConjuntos[0], var, regras); // TODO *****
        avaliarRegras(x, pesoConjuntos, var, regras); // TODO *****

	for (i=0; i < NUMOUTPUTS; i++) {
		agregarSaida(var[NUMINPUTS+i], pesoConjuntos[i], saidaConjuntos[i]);
		//y[i] = centroid(saidaConjuntos[i], var[NUMINPUTS+i].valMin, var[NUMINPUTS+i].valMax);
                y[i] = func_defuzz(saidaConjuntos[i], var[NUMINPUTS+i].valMin, var[NUMINPUTS+i].valMax);
	}

	//puts("\n-- Terminou de rodar o sistema fuzzy --");
}


// Calcula o mi de x [u(x)] em determinado conjunto conj que eh triangulo ou trapezio
float membershipFunction(float x, Conjunto conj) {
	float u;

	if (x < conj.a) {
		u = 0;
	}
	else if (x >= conj.a && x < conj.b) {
		u = (x - conj.a)/(conj.b - conj.a);
	}
	else if (x >= conj.b && x < conj.c)	{
		u = 1;
	}
	else if (x >= conj.c && x < conj.d) {
		u = (conj.d - x)/(conj.d - conj.c);
	}
	else {
		u = 0;
	}
	
	return u;
}

//void avaliarRegras(float x[], float forcas[], Variavel var[], Regra regras[]) {
void avaliarRegras(float x[], float forcas[NUMOUTPUTS][NUMCONJUNTOS], Variavel var[], Regra regras[]) {
	//printf("Avaliando %d regras...\nRegras ativadas:\n", NUMRULES);
	int i, j, k;
	float uEntrada[NUMINPUTS];
	float pesoRegra;
	
	// Zerar a forca de cada conjunto de saida antes de determinar
	// TODO tirar esse 7
        for (i=0; i<NUMOUTPUTS; i++) {
  	  for (j=0; j<NUMCONJUNTOS; j++) {
  		forcas[i][j] = 0;
  	  }
        }
	
	// Para cada regra
	for (i=0; i<NUMRULES; i++) {
            pesoRegra = 1.0;
        
		// Calcular o mi de x para cada variavel de entrada daquela regra
		for (j=0; j<NUMINPUTS; j++) {
			k = regras[i].conjEntrada[j] - 1;
			// Antes de calcular o mi de x, confirma se a regra depende daquela entrada, senao ja coloca como 0
			if (k >= 0) {
			    uEntrada[j] = membershipFunction(x[j], var[j].conj[k]);
                            pesoRegra = libFuzzyMin(pesoRegra,uEntrada[j]);
			} else {
				uEntrada[j] = 0.0;
			}
		}
        
        // Supondo que o sistema usa libFuzzyMax() pra escolher a maior forca de cada conjunto
		if (pesoRegra > 0) {
			//printf("[Regra %d]\n", i+1);
			for (j=0; j<NUMINPUTS; j++) {
				//printf("	%s: %f\n", var[j].nome, uEntrada[j]);
			}
			for (j=0; j<NUMOUTPUTS; j++) {
				//printf("	Forca %s: %f no conjunto %d\n", var[NUMINPUTS+j].nome, pesoRegra, regras[i].conjSaida[j]);
			}
			
                        for (j=0; j<NUMOUTPUTS; j++) {
			  forcas[j][regras[i].conjSaida[j]-1] = libFuzzyMax(forcas[j][regras[i].conjSaida[j]-1], pesoRegra);
                        }
            
		}
	}
	
	/*puts("\nVerificando forca final de cada conjunto da saida:");
	
	// Imprimir qual foi a forca de cada conjunto de cada saida
	for (i=0; i<NUMOUTPUTS; i++) {
		printf("[%s]\n", var[i].nome);
		for (j=0; j<var[i].numConjuntos; j++) {
			printf("Conjunto %d ficou com maximo %f\n", j+1, forcas[j]);
		}
	}
    
	puts("");*/
	
}


// Vai gerar o conjunto final daquela variavel de saida, para a posterior defuzificacao
void agregarSaida(Variavel var, float forcas[], float saida[]) {

	float x = var.valMin;
	int i, j;

	float pedaco = (var.valMax - var.valMin)/INTERVALO;

	// Tenho que avaliar de pedaco em pedaco o mi de x
	for (i=0; i < INTERVALO; i++) {
		saida[i] = 0;

		// Para cada conjunto nesse ponto, verificar qual tem maior forca
		// Roda para cada conjunto
		for (j=0; j<var.numConjuntos; j++) {
			if (forcas[j] > 0) {
				saida[i] = libFuzzyMax(saida[i], forcas[j]*membershipFunction(x,var.conj[j]));
			}
		}

		//printf("(%f, %f)\n", x, saida[i]);

		x += pedaco;
	}

}

float centroid(float y[], float rangeMin, float rangeMax) {
	float x = rangeMin;
	float yNom = 0;
	float yDen = 0;
	float yCentroid;
	int i;

	float pedaco = (rangeMax - rangeMin)/INTERVALO;

	for (i=0; i<INTERVALO; i++) {
		yNom += x*y[i];
		yDen += y[i];
		x += pedaco;
	}

	//printf("yNom = %f\nyDen = %f\n", yNom, yDen);
	
	yCentroid = yNom/yDen;
	
	return yCentroid;
}




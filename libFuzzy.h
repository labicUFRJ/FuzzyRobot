// Cabecalho da biblioteca fuzzy do LabIC.
// Nada aqui deve ser modificado.
// Mario Cecchi - 2011- LabIC - UFRJ

#include "meuFIS.h"

#define INTERVALO 100

typedef struct
{
	float a, b, c, d;
} Conjunto;

typedef struct 
{
	char *nome;
	int numConjuntos;
	float valMin, valMax;
	Conjunto conj[NUMCONJUNTOS];
} Variavel;

typedef struct
{
	int conjEntrada[NUMINPUTS], conjSaida[NUMOUTPUTS];
} Regra;

void rodarMeuFis(float x[], float y[]);
float membershipFunction(float x, Conjunto conj);
void avaliarRegras(float x[], float pesoConj[NUMOUTPUTS][NUMCONJUNTOS], Variavel var[], Regra regras[]);
void rodarSistema(float x[], float y[], Variavel var[], Regra regras[]);
void agregarSaida(Variavel var, float forcas[], float saida[]);
float centroid(float y[], float rangeMin, float rangeMax);

#define libFuzzyMin(a, b) ((a) < (b) ? (a) : (b))

#define libFuzzyMax(a, b) ((a) > (b) ? (a) : (b))



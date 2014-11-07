/************************************************************************
*   Código para navegação de um robô baseado em Arduino Mega            *
*   através de um controlador nebuloso.                                 *
*                                                                       *
*   Trabalho de Conclusão de Curso de Mariam Afonso (mariam@ufrj.br)    *
*   Universidade Federal do Rio de Janeiro - UFRJ                       *
*   Departamento de Ciência da Computação - DCC                         *
*   Laboratório de Inteligência Computacional - LabIC                   *
*   Julho de 2012                                                       *
*                                                                       *
*   (Colaboração de Mário Cecchi.)                                      *
************************************************************************/
#include <Arduino.h>
#include <DCMotor.h>
#include <Servo.h>
#include <math.h>
#include "ponto.h"
#include "libFuzzy.h"
#include "libHeap.h"
#include "libGrafo.h"
#include <IRSendRev.h>

#define INFINITO 10000
#define NVERTICES 6

int noPai[NVERTICES];
int distNo[NVERTICES];
int caminho[NVERTICES];

////LibPosicao\\\\
//Valor de referência para a horizontal do ambiente:
float horizontal;
//Ângulo da frente do robô com a horizontal:
float phi;
//Dados auxiliares da iteração anterior:
float prevPhi = 0;
float prevGamma = 0;

//Fator de proporção para medidas de distância entre
//os mundos virtual (do controlador) e real (6:12):
const float fatorPropDist = 2;

//Vetores para as entradas e as saídas do controlador:
float entrada[NUMINPUTS];
float saida[NUMOUTPUTS];

//Saídas do controlador: ângulo de giro e tamanho do passo:
float theta;
float delta;

int idSensor = -1;

boolean fim = 0;

DCMotor motor(0.5, 0.05, 0.05) ;

Grafo g;
HEAP h;

int origem = 0;
int destino = 5;

void setup(){
  
  initHeap(&h, 6);
  cria_grafo(&g, 6);
  initCaminho(&g);
  adiciona_aresta(&g,0,1,1,10,0,30);
  adiciona_aresta(&g,0,2,3,10,0,30);
  adiciona_aresta(&g,0,3,6,10,0,30);
  adiciona_aresta(&g,1,2,8,10,0,30);
  adiciona_aresta(&g,2,5,9,10,0,30);
  adiciona_aresta(&g,3,1,7,10,0,30);
  dijkstra(&g,origem,h);
  reconstroiCaminho(&g, origem, destino);
  
  attachInterrupt(2,receiveIdSensor,CHANGE);
  IR.Init(21); // inicializa o pino de sinal do receptor IR

  Serial.begin(38400);// frequencia do emissor de infravermelho
  Serial1.begin(38400);
  //Serial2.begin(38400);
  //Serial1.begin(115200);
  
  servoPingSetup();
  
  motor.begin();
  
  Serial.println("Log de navegacao:");
  Serial.println();
  
  Serial.println("Inicializando o sensor HM55B...");
  Serial.println();
  HM55B_Reset();
  //HM55B_Calibration();
  HM55B_CurrentCalibrationValues();
  
  setHorizontal();
  //currentHorizontal();
  
  //Espera para colocar o robô na posição inicial do ambiente:
  //delay(60000);
  
  //Zerando as saídas do controlador para a 1ª iteração:
  for(int i = 0; i < NUMOUTPUTS; i++){
    saida[i] = 0.0;
  }
  
  //Já precisam estar definidos no início da 1ª iteração:
  theta = saida[0];
  delta = saida[1];
  
  //randomSeed(analogRead(0)); //Usado pra testar a memória
  
  delay(1500);
}

void loop(){
  //debug();
  //delay(600000);
    switch(idSensor){
        case -1 : controlador(); 
        break;
        default: PercorreCaminho(&g,destino);
        break;
    }
/*  while(!fim){
    
//    drawSurface();
    /*        
    //Saídas do controlador na última iteração:
    theta = saida[0];
    delta = saida[1];
    Serial.print("Theta = ");
    Serial.println(theta);
    Serial.print("Delta = ");
    Serial.println(delta);
    Serial.println();
    
    //Determina localização:
    phi = getPhi();
    Serial.print("Phi = ");
    Serial.println(phi);
    Serial.println();
    
    //Recebe id do sensor IR que estiver proximo
    //receiveIdSensor();
    
    //Detecta obstáculos:
    Serial.println("Deteccoes:");
    detectar();
    
    //Chama o pré-processador...
    //1. Atualiza a memória:
    atualizarMemoria();
    
    //2. Define as entradas do controlador:
    definirEntradasControlador();
    
    //Chama o controlador, com saídas theta e delta:
    rodarMeuFis(entrada, saida);
    theta = saida[0];
    delta = saida[1];
    Serial.println();
    Serial.println("Saidas do controlador:");
    Serial.print("   Theta = ");
    Serial.println(theta);
    Serial.print("   Delta = ");
    Serial.print(delta);
    //Correção pelo fator de proporção:
    delta *= fatorPropDist;
    Serial.print("  ==>  ");
    Serial.println(delta);
    
    //Movimentação:
    //1. Girar:
    if(theta > 0.0){
      motor.left(theta);
    }
    else{
      theta = -theta;
      motor.right(theta);
    }
    delay(500);
    
    //2. Andar:
    motor.forward(delta);
    prevPhi = phi;
    
    //delay(1500);
    
    Serial.println();
    Serial.println("===========================================");
    Serial.println();
  
  } */
}

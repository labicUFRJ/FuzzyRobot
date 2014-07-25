/*****************************************************************
  Biblioteca com os tipos e funções relativos às representações
  dos pontos detectados (coordenadas polares e cartesianas).
*****************************************************************/

#include <math.h>

typedef struct{
  float d, beta;
} PontoPolar;

typedef struct{
  float x, y;
} PontoCart;

const float pi = M_PI;


float grau2rad(float a){
  /*********************************************
    Converte um ângulo em graus para radianos.
  *********************************************/
  return (pi * (a / 180.0));
}


float rad2grau(float r){
  /*********************************************
    Converte um ângulo em radianos para graus.
  *********************************************/
  return (180.0 * (r / pi));
}


PontoPolar cart2polar(float x, float y){
  /***********************************************
    Converte um ponto cartesiano em ponto polar.
    O ângulo fica em graus.
  ***********************************************/
  PontoPolar pto;
  
  pto.d = sqrt((x * x) + (y * y));
  pto.beta = rad2grau(atan2(y, x));
  
  return pto;
}


PontoPolar cart2polar(PontoCart ptoC){
  /***********************************************
                  Sobrecarga
    Converte um ponto cartesiano em ponto polar.
    O ângulo fica em graus.
  ***********************************************/
  PontoPolar ptoP = cart2polar(ptoC.x, ptoC.y);
  return ptoP;
}


PontoCart polar2cart(float d, float a){
  /***********************************************
    Converte um ponto polar em ponto cartesiano.
    Recebe o ângulo em graus.
  ***********************************************/
  PontoCart pto;
  int atemp = a;
  
  //Tratando resultado -0.00.
  if(a > 0){
    //Tentando a = 90 + 360k
    if(((atemp - 90) % 360) == 0){
      a = -270.0;
    }
    //Tentando a = 180 + 360k
    else if(((atemp - 180) % 360) == 0){
      a = -180.0;
    }
  }
  else if(a < 0){
    //Tentando a = -90 + (-360k)
    if(((atemp + 90) % 360) == 0){
      a = 270.0;
    }
    //Tentando a = -360 + (-360k)
    else if(((atemp + 360) % 360) == 0){
      a = 0.0;
    }
  }
  
  pto.x = d * cos(grau2rad(a));
  pto.y = d * sin(grau2rad(a));
  
  return pto;
}


PontoCart polar2cart(PontoPolar ptoP){
  /***********************************************
                  Sobrecarga
    Converte um ponto polar em ponto cartesiano.
  ***********************************************/
  PontoCart ptoC = polar2cart(ptoP.d, ptoP.beta);
  return ptoC;
}


//PontoPolar getPontoPolar(float d, float a){
//  PontoPolar pto;
//  
//  pto.d = d;
//  pto.beta = a;
//  
//  return pto;
//}




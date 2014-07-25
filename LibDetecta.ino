/************************************************************************
*         Funções para manipulação da detecção do sensor de             *
*       distância e para mudança de referencial das medições.           *
************************************************************************/


//Servo do PING))):
Servo servoPing;
//Quantidade de sensores lógicos:
const int sensLogicos = 3;
//Representação dos 3 sensores lógicos de distância.
//Alfa: 15º (15º à esquerda), 0º (centro), -15º (15º à direita)
const int alfa[] = {113, 98, 83};
//ID's (1, 2 ou 3) dos sensores lógicos:
const int UM = 0;
const int DOIS = 1;
const int TRES = 2;
//Distância medida por cada sensor lógico:
float distAlfa[sensLogicos];
//Indicador de medições dúbias, que devem ser ignoradas:
int ignorarPonto[sensLogicos];
//Representação dos pontos detectados:
PontoPolar ptoDetec[sensLogicos];
//Valor de corte para detecções dúbias:
const float corteDetecDub = 368.0;


void servoPingSetup(){
  /***********************************************************
  *   Conecta o servo do Ping))) e o coloca na posição 0º.   *
  ***********************************************************/
  
  servoPing.attach(42);
  servoPing.write(alfa[DOIS]);
}


void detectar(){
  /*******************************************************
  *   Realiza a detecção nas 3 posições dos sensores     *
  *   lógicos, fazendo as devidas correções (zona de     *
  *   imprecisão e valores dúbios) e a mudança de        *
  *   referencial para o centro imaginário do robô.      *
  *******************************************************/
  
  int i;
  const float correcao = 5.0;
  
  for(i = 0; i < sensLogicos; i++){
    ignorarPonto[i] = 0;
  }
  
  for(i = 0; i < sensLogicos; i++){
    distAlfa[i] = distSensor(alfa[i], (i+1));
//    distAlfa[i] = distSensor(alfa[i], (i+1));
//    distAlfa[i] = distSensor(alfa[i], (i+1));
//    distAlfa[i] = distSensor(alfa[i], (i+1));
//    distAlfa[i] = distSensor(alfa[i], (i+1));
    
    //Se a medição está na zona de imprecisão...
    if(distAlfa[i] < correcao){
      distAlfa[i] = correcao;
    }
    
    //Ignorando medições dúbias de 369 e 370:
    if(distAlfa[i] > corteDetecDub){
      ignorarPonto[i] = 1;
    }
  }
  
  mudarReferencial();
}


float distSensor(int angulo, int sensorID){
  /****************************************************
  *   Gira o servo do Ping para a posição             *
  *   "angulo" do sensor lógico "sensorID",           *
  *   realiza a medição e retorna o valor obtido.     *
  *                                                   *
  *   O servo pode girar de 0º a 180º, sendo 0º       *
  *   correspondente a sua extrema direita (que       *
  *   não é exatamente a extrema direita do robô).    *
  ****************************************************/
  
  float dist;
  unsigned long tempo = 1000;
  
  //Gira o servo para a posição devida.
  servoPing.write(angulo);
  delay(tempo);
  
  dist = medirDistanciaCm();
  
  Serial.print("alfa ");
  Serial.print(sensorID);
  Serial.print(": ");
  Serial.println(dist);
  delay(tempo);
  
  return dist;
}


void mudarReferencial(){
  /**************************************************
  *   Muda o centro de referência, que está na      *
  *   parte frontal do sensor de distância, para    *
  *   o centro imaginário do robô. Para tanto,      *
  *   simplesmente usamos a Lei dos Cossenos.       *
  **************************************************/
  
  //Distância entre a frente do Ping e o centro imaginário do robô:
  const float offset = 9.0;
  int i;
  
  for(i = 0; i < sensLogicos; i++){
    if(ignorarPonto[i] == 0){
      getDadosPonto(i, offset);
    }
  }
  
  Serial.println();
  for(i = 0; i < sensLogicos; i++){
    Serial.print("Ponto ");
    Serial.print(i + 1);
    if(ignorarPonto[i] == 0){
      Serial.print(": distancia = ");
      Serial.println(ptoDetec[i].d);
      Serial.print("         angulo = ");
      Serial.println(ptoDetec[i].beta);
    }
    else{
      Serial.print(": medicao = ");
      Serial.print(distAlfa[i]);
      Serial.println(" ==> Ignorado");
    }
  }
}


void getDadosPonto(int i, float off){
  /*********************************************
  *   Aplicação da Lei dos Cossenos para       *
  *   obtenção da distância e do ângulo        *
  *   em relação ao centro imaginário do       *
  *   robô. Em alfa3, levamos em conta         *
  *   que o ângulo final deve ser negativo.    *
  *********************************************/
  
  const float off2 = off * off;
  float ang, dist2, a, b;
  
  //180º - Alfa i
  if(i == DOIS){
    //Alfa2 = 0º
    ang = 180.0;
  }
  else{
    //|Alfa1| = |Alfa3| = 15º
    ang = 165.0;
  }
  
  dist2 = distAlfa[i] * distAlfa[i];
  a = 2 * off * distAlfa[i] * cos(grau2rad(ang));
  //Distância ao quadrado:
  ptoDetec[i].d = off2 + dist2 - a;
  a = ptoDetec[i].d + off2 - dist2;
  //Distância de fato:
  ptoDetec[i].d = sqrt(ptoDetec[i].d);
  b = 2 * ptoDetec[i].d * off;
  //Ângulo com relação à frente, em graus:
  ptoDetec[i].beta = rad2grau(acos(a / b));
  
  //Para Alfa3 o ângulo é negativo:
  if(i == TRES){
    ptoDetec[i].beta *= -1.0;
  }
}



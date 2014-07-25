/************************************************************************
*           Funções para o Módulo Decisor - Pré-processador:            *
*    manipulação da memória e definição das entradas do controlador.    *
************************************************************************/


//Tamanho da memória:
const int tamMem = 9;
//Memória do pré-processador:
PontoPolar memoria[tamMem];
//Próxima posição da memória a ser preenchida:
int posMem = 0;
//Quantidade de elementos na memória:
int nMem = 0;
//Flag de pontos da iteração atual a serem armazenados em memória:
int incluir[sensLogicos];


/////////////////////////////////////////////////////////////////////////
//                Funções para manipulação da memória.                 //
/////////////////////////////////////////////////////////////////////////

void atualizarMemoria(){
  /****************************************************************
  *   Atualiza os pontos já armazenados em memória,               *
  *   avalia a inserção dos pontos detectados na iteração atual   *
  *   e os coloca no lugar dos elementos mais antigos.            *
  ****************************************************************/
  
  int i, j;
  //Temporários para memória e ponto detectado:
  PontoCart tempMem, tempDet;
  //Para o cálculo da distância euclidiana aos pontos em memória:
  float dist, difX, difY;
  //Valor para o critério de inclusão de novos pontos:
  const float criterio = 3.0;
  
  if(nMem == 0){
    //1ª iteração: inclusão direta de todos os
    //pontos que não devam ser ignorados.
    for(i = 0; i < sensLogicos; i++){
      if(ignorarPonto[i] == 0){
        incluir[i] = 1;
      }
      else{
        incluir[i] = 0;
      }
    }
  }
  else{
    //A partir da 2ª iteração...
    //1. Atualização dos pontos em memória.
    for(i = 0; i < nMem; i++){
//      Serial.print("memoria[");
//      Serial.print(i);
//      Serial.print("]: ");
//      Serial.print(memoria[i].d);
//      Serial.print("  |  ");
//      Serial.println(memoria[i].beta);
      
      //a. Considera apenas o giro do robô:
      memoria[i].beta -= (phi - prevPhi);
//      Serial.print("  memoria[");
//      Serial.print(i);
//      Serial.print("].beta = ");
//      Serial.println(memoria[i].beta);
      
      //b. Cálculo das coordenadas cartesianas do ponto no novo
      //sistema de coordenadas (sempre centrado no centro do
      //robô, com eixo x equivalente à linha da frente dele):
      tempMem = polar2cart(memoria[i]);
      
      //c. Considera que o robô andou
      //(ele anda sobre o eixo x do novo plano):
      tempMem.x -= delta;
//      Serial.print("  memoria[");
//      Serial.print(i);
//      Serial.print("].x = ");
//      Serial.println(tempMem.x);
      
      //d. Retornamos o ponto para coordenadas polares:
      memoria[i] = cart2polar(tempMem);
//      Serial.print("  memoria[");
//      Serial.print(i);
//      Serial.print("]: ");
//      Serial.print(memoria[i].d);
//      Serial.print("  |  ");
//      Serial.println(memoria[i].beta);
//      Serial.println();
    }
    //2. Avaliar a inclusão dos pontos detectados na iteração
    //atual. Podem entrar 0, 1, 2 ou os 3 pontos.
    for(i = 0; i < sensLogicos; i++){
      incluir[i] = 1;
//      Serial.println();
//      Serial.print("Ponto ");
//      Serial.print(i + 1);
//      Serial.println(":");
      
      //Se o ponto não deve ser ignorado...
      if(ignorarPonto[i] == 0){
        //a. Cálculo das coordenadas cartesianas do ponto:
        tempDet = polar2cart(ptoDetec[i]);
        
        for(j = 0; j < nMem; j++){
          //b. Cálculo das coordenadas cartesianas do ponto
          //j em memória:
          tempMem = polar2cart(memoria[j]);
          
          //c. Cálculo da distância ao ponto j em memória
          //(distância euclidiana):
          difX = abs(tempDet.x - tempMem.x);
          difY = abs(tempDet.y - tempMem.y);
          dist = sqrt( (difX * difX) + (difY * difY) );
//          Serial.print("  Distancia a memoria[");
//          Serial.print(j);
//          Serial.print("] = ");
//          Serial.println(dist);
          
          //d. Avaliação do novo possível ponto:
          if(dist < criterio){
            incluir[i] = 0;
            break;
          }
        }
      }
      else{
        incluir[i] = 0;
      }
    }
  }
  
  Serial.println();
  for(i = 0; i < sensLogicos; i++){
    Serial.print("Incluir ponto ");
    Serial.print(i + 1);
    Serial.print("?    ");
    if(incluir[i] == 1){
      Serial.println("SIM");
    }
    else{
      Serial.println("NAO");
    }
  }
  
  //Inclusão dos pontos escolhidos:
  incluirNaMemoria();
  
  Serial.println();
  Serial.println("Memoria:");
  Serial.print("(Usando delta = ");
  Serial.print(delta);
  Serial.println(")");
  Serial.print("Proxima posicao = ");
  Serial.println(posMem);
  Serial.print("Elementos em memoria = ");
  Serial.println(nMem);
  for(i = 0; i < nMem; i++){
    Serial.print("   ");
    Serial.print(i);
    Serial.print(":  d = ");
    Serial.print(memoria[i].d);
    Serial.print(",  beta = ");
    Serial.println(memoria[i].beta);
  }
}


void incluirNaMemoria(){
  /************************************************
  *   Inclui na memória os pontos selecionados.   *
  ************************************************/
  
  int i;
  
  for(i = 0; i < sensLogicos; i++){
    if(incluir[i] == 1){
      memoria[posMem] = ptoDetec[i];
      posMem = (posMem + 1) % tamMem;
      nMem = min((nMem + 1), tamMem);
    }
  }
}


/////////////////////////////////////////////////////////////////////////
//               Funções para definição das entradas do                //
//               controlador (saídas do módulo decisor).               //
/////////////////////////////////////////////////////////////////////////

void definirEntradasControlador(){
  /*****************************************************************
  *   Define os pontos detectados cuja distância ao robô é         *
  *   mínima dentro de 3 faixas (em relação à frente do robô):     *
  *    -> Faixa 1: de 15º a 105º                                   *
  *    -> Faixa 2: de -15º a 15º                                   *
  *    -> Faixa 3: de -105º a -15º                                 *
  *   Divide as distâncias desses pontos pelo fator de proporção   *
  *   entre os mundos virtual (do controlador) e real (6:13.6)     *
  *   e os atribui às devidas entradas do controlador, e também    *
  *   atribui sua 4ª entrada (phi).                                *
  *****************************************************************/
  
  int i;
  //Mínimo e máximo de cada faixa:
  const float faixa1[] = {15, 105};
  const float faixa2[] = {-15, 15};
  const float faixa3[] = {-105, -15};
  //Índices representando mínimo e máximo das faixas:
  const int L = 0;
  const int U = 1;
  //Índice do elemento escolhido em cada faixa:
  int indexMin[] = {-1, -1, -1};
  //Distância máxima padrão para os obstáculos:
  const float maxDistObst = 400.0;
  
//  Serial.println();
//  Serial.print(n);
//  Serial.println(" elementos em memoria...");
  //Para cada ponto em memória...
  for(i = 0; i < nMem; i++){
//    Serial.print("Testando memoria[");
//    Serial.print(i);
//    Serial.println("]:");
    //Testando pertinência à faixa 1:
    if((faixa1[L]<=memoria[i].beta) && (memoria[i].beta<=faixa1[U])){
//      Serial.println("  Pertence a faixa 1.");
      if(indexMin[0] != -1){
        if(memoria[i].d < memoria[indexMin[0]].d){
          indexMin[0] = i;
//          Serial.print("  -- Minimo da faixa: ");
//          Serial.println(memoria[i].d);
        }
      }
      else{
        indexMin[0] = i;
//        Serial.print("  - Primeiro da faixa: ");
//        Serial.println(memoria[i].d);
      }
    }
    //Testando pertinência à faixa 2:
    else if((faixa2[L]<=memoria[i].beta)&&(memoria[i].beta<=faixa2[U])){
//      Serial.println("  Pertence a faixa 2.");
      if(indexMin[1] != -1){
        if(memoria[i].d < memoria[indexMin[1]].d){
          indexMin[1] = i;
//          Serial.print("  -- Minimo da faixa: ");
//          Serial.println(memoria[i].d);
        }
      }
      else{
        indexMin[1] = i;
//        Serial.print("  - Primeiro da faixa: ");
//        Serial.println(memoria[i].d);
      }
    }
    //Testando pertinência à faixa 3:
    else if((faixa3[L]<=memoria[i].beta)&&(memoria[i].beta<=faixa3[U])){
//      Serial.println("  Pertence a faixa 3.");
      if(indexMin[2] != -1){
        if(memoria[i].d < memoria[indexMin[2]].d){
          indexMin[2] = i;
//          Serial.print("  -- Minimo da faixa: ");
//          Serial.println(memoria[i].d);
        }
      }
      else{
        indexMin[2] = i;
//        Serial.print("  - Primeiro da faixa: ");
//        Serial.println(memoria[i].d);
      }
    }
  }
  
  //Para cada possível mínimo de cada faixa...
  Serial.println();
  Serial.println("Entradas do controlador:");
  for(i = 0; i < 3; i++){
    //Se um pto foi selecionado na faixa i, é uma entrada do controlador:
    if(indexMin[i] != -1){
      entrada[i] = memoria[indexMin[i]].d / fatorPropDist;
      //Para não extrapolar os limites da variável:
      if(entrada[i] > maxDistObst){
        entrada[i] = maxDistObst;
      }
    }
    //Se não há ponto nela, recebe o maior valor possível:
    else{
      entrada[i] = maxDistObst;
    }
    Serial.print("   Df");
    Serial.print(i+1);
    Serial.print(" = ");
    Serial.print(entrada[i]);
    Serial.print("  (mem[");
    Serial.print(indexMin[i]);
    Serial.println("])");
  }
  
  //4ª entrada (phi):
  entrada[3] = phi;
  Serial.print("   Phi = ");
  Serial.println(entrada[3]);
}



/************************************************************************
*       Funções para calibragem e uso do magnetômetro HM55B.            *
*                                                                       *
*   Código adaptado de Arduino Playground                               *
*   (http://arduino.cc/playground/Main/HM55B).                          *
*                                                                       *
*   Referências para adaptação e comentários:                           *
*   (http://www.parallax.com/dl/docs/prod/compshop/HM55BModDocs.pdf)    *
*   (http://www.playchild.de/uni/schweden/Sensors.pdf)                  *
************************************************************************/


byte DIO_pin = 35;  //Pinos Din e Dout (1 e 2)
byte CLK_pin = 39;  //Pino Clk (4)
byte EN_pin = 37;   //Pino /EN (5)

const int maxSetores = 16;
const float spanIdeal = 22.5;

//Flag de status do sensor:
int statusFlag;
//Força do campo magnético e ângulo correspondente:
float X_Data, Y_Data, gamma;
//Variáveis de calibragem e interpolação:
float X_offset, Y_offset, interpolTable[maxSetores];


/////////////////////////////////////////////////////////////////////////
//                       Funções para interação                        //
/////////////////////////////////////////////////////////////////////////

void HM55B_Reset(){
  /***********************************
  *   Executa o protocolo "Reset".   *
  ***********************************/
  
  pinMode(EN_pin, OUTPUT);
  pinMode(CLK_pin, OUTPUT);
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  ShiftOut(B0000, 3);
  digitalWrite(EN_pin, HIGH);
}


void HM55B_MainRoutine(){
  /********************************************************
  *   Rotina principal de interação com o sensor HM55B.   *
  ********************************************************/
  
  HM55B_Measurement();
  HM55B_CorrectOffsets();
  HM55B_AngleDegree();
  HM55B_Interpolation();
  
//  printMeasurement();
//  printGamma();
}


void HM55B_Measurement(){
  /***********************************************
  *   Executa a obtenção de medidas do sensor.   *
  ***********************************************/
  
  HM55B_StartMeasurementCommand();
  
  //Os dados estão prontos 40ms depois:
  delay(40);
  statusFlag = HM55B_ReadCommand();
  
  //Medição da força do campo magnético nas direções dos
  //eixos X e Y, cada valor com 11 bits:
  X_Data = ShiftIn(11);
  Y_Data = ShiftIn(11);
  
  //Finalização do protocolo para obtenção da leitura:
  digitalWrite(EN_pin, HIGH);
}


void HM55B_StartMeasurementCommand(){
  /***********************************************
  *   Executa o protocolo "Start Measurement".   *
  ***********************************************/
  
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  ShiftOut(B1000, 3);
  digitalWrite(EN_pin, HIGH);
  //Fazemos /EN voltar para HIGH porque o protocolo seguinte,
  //em HM55B_ReadCommand(), exige um pulso positivo inicial.
  //Então basta fazê-lo ir a LOW no início do outro protocolo.
}


int HM55B_ReadCommand(){
  /*******************************************************
  *   Executa o protocolo "Report Measurement Status".   *
  *******************************************************/
  
  int result;
  
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  ShiftOut(B1100, 3);
  result = ShiftIn(3);
  return result;
}


void ShiftOut(int Value, int BitsCount){
  /*************************************************************
  *   DIO_pin emite pulsos com o valor de cada bit de Value.   *
  *************************************************************/
  
  for(int i = BitsCount; i >= 0; i--){
    digitalWrite(CLK_pin, LOW);
    //Seleção do i-ésimo bit de Value
    //(referência em http://arduino.cc/en/Reference/BitwiseAnd):
    if((Value & (1 << i)) == (1 << i)){
      digitalWrite(DIO_pin, HIGH);
      //Serial.print("1");
    }
    else{
      digitalWrite(DIO_pin, LOW);
      //Serial.print("0");
    }
    digitalWrite(CLK_pin, HIGH);
    delayMicroseconds(1);
  }
  //Serial.println(" ");
}


int ShiftIn(int BitsCount){
  /*****************************************************************
  *   Acumula em ShiftIn_result a leitura de DIO_pin.              *
  *   A 1ª parte faz o acúmulo de fato, inserindo cada bit lido    *
  *   sempre à direita do valor já acumulado.                      *
  *   A 2ª parte trata valores negativos da leitura da força do    *
  *   campo magnético. O valor lido é negativo se o 11º bit é 1.   *
  *   Nesse caso, para completar a representação (em complemento   *
  *   a dois), é necessário aplicar a máscara B11111000 ao byte    *
  *   mais significativo de ShiftIn_result.                        *
  *****************************************************************/
  
  int ShiftIn_result;
  
  ShiftIn_result = 0;
  pinMode(DIO_pin, INPUT);
  
  for(int i = BitsCount; i >= 0; i--){
    digitalWrite(CLK_pin, HIGH);
    delayMicroseconds(1);
    if(digitalRead(DIO_pin) == HIGH){
      ShiftIn_result = (ShiftIn_result << 1) + 1;
      //Serial.print("x");
    }
    else{
      ShiftIn_result = (ShiftIn_result << 1);
      //Serial.print("_");
    }
    digitalWrite(CLK_pin, LOW);
    delayMicroseconds(1);
  }
  //Serial.print(":");
  
  //Tratamento de valores negativos:
  //Selecionamos o 11º bit. Se for 1, o valor é negativo.
  if((ShiftIn_result & (1 << 11)) == (1 << 11)){
    ShiftIn_result = (B11111000 << 8) | ShiftIn_result;
    //Primeiro transforma B11111000 num valor de 16 bits, com os 8 bits
    //menos significativos iguais a 0. Depois realiza um OU bit a bit.
  }
  
  //Serial.print(ShiftIn_result);
  return ShiftIn_result;
}


void HM55B_AngleDegree(){
  /****************************************************************
  *   O ângulo já é convertido para graus. Usamos -y para obter   *
  *   o valor do ângulo em relação ao N, em sentido horário.      *
  *   Intervalode de gamma = [-180, 180].                         *
  ****************************************************************/
  
  gamma = 180.0 * (atan2(-1.0 * Y_Data , X_Data) / M_PI);
}


void printMeasurement(){
  Serial.print("Status: ");
  Serial.print(statusFlag);
  Serial.print("  --  X: ");
  Serial.print(X_Data);
  Serial.print("  ,  Y: ");
  Serial.print(Y_Data);
}


void printGamma(){
  Serial.print("  ,  Angle: ");
  Serial.println(gamma);
}


float getGammaFiltrado(){
  /*******************************************************************
  *   Realiza a quantidade tamFiltro de medições, acumula o gamma    *
  *   obtido em cada uma delas e, por fim, divide essa soma por      *
  *   tamFiltro.                                                     *
  *******************************************************************/
  
  int i = 0;
  const int tamFiltro = 10;
  float gammaFiltrado = 0.0;
  
//  Serial.println();
  while(i < tamFiltro){
//    Serial.print(i);
//    Serial.print(":   ");
    HM55B_MainRoutine();
    //Se houve falha na leitura, recomeço a sequência de medições:
    if(statusFlag != 12){
      gammaFiltrado = 0.0;
      i = 0;
      continue;
    }
    gammaFiltrado += gamma;
    i++;
  }
  gammaFiltrado /= tamFiltro;
  
//  Serial.print("    MEDIA = ");
//  Serial.println(gammaFiltrado);
  
  return (gammaFiltrado);
}


/////////////////////////////////////////////////////////////////////////
//                      Funções para calibragem                        //
/////////////////////////////////////////////////////////////////////////

void HM55B_Calibration(){
  /******************************************************************
  *              Calibragem do magnetômetro HM55B.                  *
  *                                                                 *
  *   Mais informações e código original em                         *
  *   (www.parallax.com/dl/docs/prod/compshop/HM55BModDocs.pdf).    *
  *   O código original também pode ser obtido em                   *
  *   (parallax.com/Portals/0/Downloads/src/prod/29123Src.zip).     *
  *                                                                 *
  *   Para recalibrar o sensor:                                     *
  *    -> No setup(), descomente a função "HM55B_Calibration()" e   *
  *    comente a função "HM55B_CurrentCalibrationValues()".         *
  *    -> Faça o upload para o arduino e abra o Serial Monitor      *
  *    para acompanhar a execução.                                  *
  *    -> Armazene MANUALMENTE os valores retornados pela           *
  *    calibragem nas devidas variáveis da função                   *
  *    "HM55B_CurrentCalibrationValues()".                          *
  ******************************************************************/
  
  HM55B_GetAxesOffsets();
  HM55B_GetInterpolation();
}


void HM55B_GetAxesOffsets(){
  /***************************************************************
  *   Esta função faz 4 medições consecutivas, que devem ser     *
  *   realizadas com o sensor nas direções Norte, Leste, Sul e   *
  *   Oeste, nesta ordem. Imprime a média das medições em        *
  *   cada eixo (máximo e mínimo em X e em Y).                   *
  *   Esses dois valores devem ser armazenados MANUALMENTE nas   *
  *   variáveis X_offset e Y_offset, respectivamente.            *
  ***************************************************************/
  
  int dir;
  const int N = 0, E = 1, S = 2, W = 3;
  String direcao[] = {"NORTE", "LESTE", "SUL", "OESTE"};
  
  Serial.println();
  Serial.println("===========================================");
  for(dir = N; dir <= W; dir++){
    Serial.print("Posicione o sensor na direcao ");
    Serial.print(direcao[dir]);
    Serial.println(".");
    delay(30000);
    Serial.println("Realizando medicao...");
    delay(1000);
    switch(dir){
      case N:
        X_offset = getDataFiltrado(N);
        break;
      case E:
        Y_offset = getDataFiltrado(E);
        break;
      case S:
        X_offset += getDataFiltrado(S);
        X_offset /= 2.0;
        break;
      case W:
        Y_offset += getDataFiltrado(W);
        Y_offset /= 2.0;
        break;
    }
    delay(1000);
    Serial.println();
    Serial.println("-------------------------------------------");
  }
  
  Serial.println();
  Serial.println("Offsets:");
  Serial.print("  X_offset = ");
  Serial.print(X_offset);
  Serial.println(";");
  Serial.print("  Y_offset = ");
  Serial.print(Y_offset);
  Serial.println(";");
  Serial.println();
  Serial.println("===========================================");
}


float getDataFiltrado(int dir){
  /*************************************************************
  *   Realiza a quantidade tamFiltro de medições, acumula o   *
  *   X_Data ou Y_Data obtido em cada uma delas e, por fim,    *
  *   divide essa soma por tamFiltro.                         *
  *************************************************************/
  
  int i = 0;
  const int tamFiltro = 10;
  const int N = 0, E = 1, S = 2, W = 3;
  float data = 0.0;
  
  while(i < tamFiltro){
    Serial.print("    ");
    Serial.print(i);
    Serial.print(":  ");
    HM55B_Measurement();
    //Se houve falha na leitura, recomeço a sequência de medições:
    if(statusFlag != 12){
      data = 0.0;
      i = 0;
      continue;
    }
    if((dir == N) || (dir == S)){
      data += X_Data;
      Serial.println(X_Data);
    }
    else if((dir == E) || (dir == W)){
      data += Y_Data;
      Serial.println(Y_Data);
    }
    i++;
  }
  
  data /= tamFiltro;
  Serial.print("    MEDIA = ");
  Serial.println(data);
  
  return data;
}


void HM55B_GetInterpolation(){
  /******************************************************************
  *   Esta função realiza 16 medições consecutivas em direções      *
  *   separadas por 22.5º. O usuário deve apontar o sensor          *
  *   inicialmente para a posição 0º (norte), depois para           *
  *   22.5º, 45º etc., até 337.5º.                                  *
  *   Os ângulos medidos em cada uma dessas posições devem ser      *
  *   armazenados MANUALMENTE no vetor interpolTable.               *
  *   Os ângulos obtidos são convertidos para graus, no intervalo   *
  *   [0, 360).                                                     *
  *                                                                 *
  *   Cada uma das 16 medições é, na verdade, composta por          *
  *   tamFiltro medições sequenciais com sucesso que são            *
  *   somadas e, por fim, divididas por tamFiltro.                  *
  ******************************************************************/
  
  int i, dir;
  const int tamFiltro = 10;
  float gammaFiltrado;
  
  Serial.println();
  Serial.println("===========================================");
  for(dir = 0; dir < maxSetores; dir++){
    Serial.print("Posicione o sensor na direcao ");
    Serial.print(spanIdeal * dir);
    Serial.println(" graus.");
    delay(30000);
    Serial.println("Realizando medicao...");
    delay(1000);
    i = 0;
    gammaFiltrado = 0.0;
    while(i < tamFiltro){
      Serial.print("    ");
      Serial.print(i);
      Serial.print(":  ");
      HM55B_Measurement();
      //Se houve falha na leitura, recomeço a sequência de medições:
      if(statusFlag != 12){
        gammaFiltrado = 0.0;
        i = 0;
        continue;
      }
      Serial.print("X = ");
      Serial.println(X_Data);
      Serial.print("        Y = ");
      Serial.println(Y_Data);
      HM55B_CorrectOffsets();
      Serial.print("        X corrigido = ");
      Serial.println(X_Data);
      Serial.print("        Y corrigido = ");
      Serial.println(Y_Data);
      HM55B_AngleDegree();
      //Passando domínio de [-180, 180] para [0, 360):
      if(gamma < 0){
        gamma += 360;
      }
      gammaFiltrado += gamma;
      i++;
      Serial.print("        Gamma = ");
      Serial.println(gamma);
    }
    gammaFiltrado /= tamFiltro;
    interpolTable[dir] = gammaFiltrado;
    Serial.print("    MEDIA = ");
    Serial.println(gammaFiltrado);
    delay(1000);
    Serial.println();
    Serial.println("-------------------------------------------");
  }
  
  Serial.println();
  Serial.println("Tabela de interpolacao:");
  for(dir = 0; dir < maxSetores; dir++){
    Serial.print("  interpolTable[");
    Serial.print(dir);
    Serial.print("] = ");
    Serial.print(interpolTable[dir]);
    Serial.println(";");
  }
  Serial.println();
  Serial.println("===========================================");
}


void HM55B_CorrectOffsets(){
  /************************************************************
  *   Esta função corrige eventuais interferências no campo   *
  *   magnético com base nos valores de offset armazenados    *
  *   nas variáveis X_offset e Y_offset.                      *
  ************************************************************/
  
  X_Data -= X_offset;
  Y_Data -= Y_offset;
}


void HM55B_CurrentCalibrationValues(){
  /***************************************************************
  *   Offsets e tabela de interpolação gerados em 20-03-2012.    *
  *                                                              *
  *   -> Depois da calibragem, coloque nas variáveis seus        *
  *   respectivos valores.                                       *
  *   -> No setup(), descomente a função                         *
  *   "HM55B_CurrentCalibrationValues()" e comente novamente a   *
  *   função "HM55B_Calibration()".                              *
  ***************************************************************/
  
  X_offset = 13.40;
  Y_offset = 136.80;

  interpolTable[0] = 353.36;
  interpolTable[1] = 25.79;
  interpolTable[2] = 73.57;
  interpolTable[3] = 80.52;
  interpolTable[4] = 94.84;
  interpolTable[5] = 108.81;
  interpolTable[6] = 123.25;
  interpolTable[7] = 146.68;
  interpolTable[8] = 192.77;
  interpolTable[9] = 223.81;
  interpolTable[10] = 235.07;
  interpolTable[11] = 255.67;
  interpolTable[12] = 264.50;
  interpolTable[13] = 274.01;
  interpolTable[14] = 285.30;
  interpolTable[15] = 303.62;
}


void HM55B_Interpolation(){
  /******************************************************************
  *   Esta função refina o ângulo resultante da medição do sensor   *
  *   com a aplicação de uma interpolação linear, procurando        *
  *   corrigir erros como a inclinação dos eixos do sensor.         *
  *   Ela usa os valores em interpolTable, que correspondem às      *
  *   medições reais do sensor para 0, 22.5, 45, 90, ..., 337.5     *
  *   graus. O algoritmo encontra o setor a que pertence o ângulo   *
  *   medido na iteração corrente e realiza a interpolação com      *
  *   relação aos dois valores extremos deste setor.                *
  ******************************************************************/
  
  int i;
  //Índices dos limitantes do setor de gamma:
  int iPrev, iNext;
  //Tamanho do setor que contém gamma:
  float spanTabela;
  //Distância entre o elemento com índice iPrev e gamma:
  float offsetInicial;
  
  //iNext começa com o índice do menor valor tabelado.
  //iPrev contém o índice anterior a iNext, imaginando que
  //interpolTable é circular. Como as medições são
  //monotônicas, podemos assumir que o elemento com índice
  //iPrev será o maior valor tabelado.
  iNext = 0;
  iPrev = maxSetores - 1;
  for(i = 1; i < maxSetores; i++){
    if(interpolTable[i] < interpolTable[iNext]){
      iNext = i;
      iPrev = i - 1;
    }
  }
  
//  Serial.println();
//  Serial.println("===========================================");
//  Serial.print("Gamma = ");
//  Serial.print(gamma);
  
  //Passando o domínio de gamma para [0, 360):
  if(gamma < 0){
    gamma += 360;
  }
  
//  Serial.print(" = ");
//  Serial.println(gamma);
//  
//  Serial.println();
//  Serial.println("Setor inicial:");
//  Serial.print("  iPrev = ");
//  Serial.print(iPrev);
//  Serial.print(" : ");
//  Serial.println(interpolTable[iPrev]);
//  Serial.print("  iNext = ");
//  Serial.print(iNext);
//  Serial.print(" : ");
//  Serial.println(interpolTable[iNext]);
  
  //Avaliando se gamma está no setor entre maior e menor valores:
  if(gamma >= interpolTable[iPrev]){
    spanTabela = 360 - interpolTable[iPrev] + interpolTable[iNext];
    offsetInicial = gamma - interpolTable[iPrev];
  }
  //Ainda avaliando se gamma está nesse setor:
  else if(gamma <= interpolTable[iNext]){
    spanTabela = 360 - interpolTable[iPrev] + interpolTable[iNext];
    offsetInicial = 360 - interpolTable[iPrev] + gamma;
  }
  //Se gamma estiver em outro setor...
  else{
    for(i = 1; i < maxSetores; i++){
      iPrev = iNext;
      iNext++;
      if(iNext == maxSetores){
        iNext = 0;
      }
      if((gamma>=interpolTable[iPrev]) && (gamma<interpolTable[iNext])){
        spanTabela = interpolTable[iNext] - interpolTable[iPrev];
        offsetInicial = gamma - interpolTable[iPrev];
        break;
      }
    }
  }
  
//  Serial.println();
//  Serial.println("Setor final:");
//  Serial.print("  iPrev = ");
//  Serial.print(iPrev);
//  Serial.print(" : ");
//  Serial.print(interpolTable[iPrev]);
//  Serial.print("   => ");
//  Serial.println(spanIdeal * iPrev);
//  Serial.print("  iNext = ");
//  Serial.print(iNext);
//  Serial.print(" : ");
//  Serial.print(interpolTable[iNext]);
//  Serial.print("   => ");
//  Serial.println(spanIdeal * iNext);
//  Serial.print("  Largura do setor = ");
//  Serial.println(spanTabela);
//  Serial.print("  Distancia prev gamma = ");
//  Serial.println(offsetInicial);
  
  //Interpolação:
  gamma = (spanIdeal * iPrev) + (offsetInicial * (22.5 / spanTabela));
  
//  Serial.println();
//  Serial.print("Gamma corrigido = ");
//  Serial.print(gamma);
  
  //Retornando gamma para o domínio [-180, 180]:
  if(gamma > 180){
    gamma -= 360;
  }
  
//  Serial.print(" = ");
//  Serial.println(gamma);
}


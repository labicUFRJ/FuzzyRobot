/************************************************************************
*     Biblioteca para interação com o sensor de distância PING))).      *
************************************************************************/


//Pino do PING))):
const int pinoPing = 40;


long medirDistanciaCm(){
  /************************************************************
  *   Mede a distância ao objeto mais próximo ao alcance do   *
  *   sensor, retornando o valor em centímetros.              *
  *   Adaptado de (http://arduino.cc/en/Tutorial/Ping)        *
  ************************************************************/

  //Emite um pulso HIGH de 2 ou mais microssegundos.
  pinMode(pinoPing, OUTPUT);
  digitalWrite(pinoPing, LOW);
  delayMicroseconds(2);
  digitalWrite(pinoPing, HIGH);
  delayMicroseconds(5);
  digitalWrite(pinoPing, LOW);
  
  //Escuta o eco do pulso HIGH emitido.
  //As divisões convertem a resposta de microsseg. para cm.
  pinMode(pinoPing, INPUT);
  return (pulseIn(pinoPing, HIGH) / 29 / 2);
  delay(100);
}



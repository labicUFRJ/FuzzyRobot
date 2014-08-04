                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
/* Graph

   I-----|sensor1|--
                    |
                    ||sensor2|
                    |
                    |
                    |
                    ||sensor3|
                    |
                     --|sensor4|-----|sensor5|-
                                               |
                                               |
                                               F
*/
//////////////////////////////////////////////////////
// OBS: o receptor detecta o sinal a 60cm do emissor//
//////////////////////////////////////////////////////

void buildGraph(){
  
    int i,j;
    for(i = 0; i< NVERTICES; i++)
            for(j = 0; j< NVERTICES; j++){
                 if(j == i+1)
                     graph[i][j] = edges[i] ;
                 else 
                     graph[i][j] = -1;
            }
}

void printGraph(){
  
    int i,j;
    
    Serial.println("matriz de adjacencia");
    for(i = 0; i< NVERTICES; i++){
              Serial.println(" ");
              for(j = 0; j< NVERTICES; j++){
                  Serial.print(graph[i][j]);
                  Serial.print(" ");                  
              }
    }
    Serial.println(" ");
}

void drawSurface(){ // Faz o robo se mover na trajetoria desejava a partir do valor do id do sensor
    
  switch (idSensor){
      case 0: // id 0
          
          /*   I-----|sensor1|--
                               |
                               ||sensor2|
                               |  */
        
          Serial.println(graph[idSensor][idSensor + 1] / 2);
          motor.forward(graph[idSensor][idSensor + 1] / 2);
          motor.right(90);
          motor.forward(graph[idSensor][idSensor + 1] / 2);
          idSensor = 255;
          
      break;
      
      case 1: // id 1
                    /*
                      |
                      ||sensor2|
                      |
                      |
                      |
                      ||sensor3|
                      |
                    */
          setHorizontal();  
          idSensor = 255;
      break;
      
      case 2: // id 2
                   /*
                   |
                   ||sensor 3|
                   |
                   |
                    --|sensor4|---
                   */      

          motor.forward(graph[idSensor][idSensor + 1] / 2);
          motor.left(90);
          motor.forward(graph[idSensor][idSensor + 1] / 2);
          idSensor = 255;
      break;
      
      case 3: // id 3
              /*
              
               --|sensor4|-----|sensor5|-
                                         |
                                         |
                                         F
              */
          setHorizontal();
          idSensor = 255;
      break;
      
      case 4: // id 4
              /*
              
              ---|sensor5|-
                          |
                          |
                          F
              */
          motor.forward(graph[idSensor][idSensor + 1] / 2);
          motor.right(90);
          motor.forward(graph[idSensor][idSensor + 1] / 2);      
          fim = 1;         
          idSensor = 255;
      break;      
      
      default: 
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
      break;
  }
  
}

void receiveIdSensor(){
  unsigned char dta[20];
  if(IR.IsDta())
  {
    int length= IR.Recv(dta);  
    idSensor = dta[length-1];
  }
  
} 


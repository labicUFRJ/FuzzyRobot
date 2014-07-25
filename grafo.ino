
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
int edges[NEDGES] = {5,20,10,5};

int graph[NVERTICES][NVERTICES];

void buildGraph(int edges[NEDGES]){
  
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
    for(i = 0; i< NVERTICES; i++)
              for(j = 0; j< NVERTICES; j++)
                  Serial.println(graph[i][j]);
                                  
}

void drawSurface(int idSensor){
    
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
                    
           drawSurface(idSensor);          
      break;
      
      case 2: // id 2
                   /*
                   |
                   ||sensor 3|
                   |
                   |
                    --|sensor4|---
                   */      

          drawSurface(idSensor);          
      break;
      
      case 3: // id 3
              /*
              
               --|sensor4|-----|sensor5|-
                                         |
                                         |
                                         F
              */
      
          drawSurface(idSensor);          
      break;
      
      case 4: // id 4
          drawSurface(idSensor);          
      break;      
      
      default: 
      break;
  }
  
}

void receiveIdSensor(){
  unsigned char dta[20];
  if(IR.IsDta())
    {
        int length= IR.Recv(dta);
        int idSensor = dta[length-1];
        Serial.println(idSensor);
        drawSurface(idSensor);
    }
    
} 


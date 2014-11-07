void controlador(){
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
   
  }

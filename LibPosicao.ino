/************************************************************************
*               Funções para determinar a horizontal                    *
*             do ambiente e a posição (estado) do robô.                 *
************************************************************************/


void setHorizontal(){
  /*************************************************************
  *   Computa a "horizontal" do ambiente, ou seja, a           *
  *   direção para onde a navegação deve tender. Este          *
  *   valor será usado como referência para obter-se phi.      *
  *                                                            *
  *   Para computar a horizontal 1 única vez e mantê-la        *
  *   ao longo de diversos testes (recomendado quando o        *
  *   ambiente de teste é o mesmo):                            *
  *    -> No setup(), descomente a função "setHorizontal()"    *
  *    e comente a função "currentHorizontal()".               *
  *    -> Faça o upload para o arduino e abra o Serial         *
  *    Monitor para acompanhar a execução.                     *
  *    -> Armazene MANUALMENTE o valor retornado na variável   *
  *    horizontal, na função "currentHorizontal()".            *
  *                                                            *
  *   Se for interessante computar a horizontal a cada         *
  *   teste, basta comentar a última linha de delay da         *
  *   função "setHorizontal()". Nesse caso, não é              *
  *   neccessário incluir o valor impresso da horizontal       *
  *   na função "currentHorizontal()", que não será usada.     *
  *************************************************************/
  
  Serial.println("Computando a horizontal...");
  horizontal = getGammaFiltrado();
  Serial.print("  horizontal = ");
  Serial.println(horizontal);
  Serial.println();
  Serial.println();
//  debugHor();
//  delay(600000);
}


void currentHorizontal(){
  /***************************************************************
  *        Valor fixo da horizontal do ambiente de testes,       *
  *                   obtido em 20-07-2012.                      *
  *                                                              *
  *   -> Depois de computar a horizontal, coloque em             *
  *   "horizontal" o devido valor.                               *
  *   -> No setup(), descomente a função "currentHorizontal()"   *
  *   e comente novamente a função "setHorizontal()".            *
  ***************************************************************/
  
  horizontal = -132.36;
  Serial.print("Horizontal = ");
  Serial.println(horizontal);
  Serial.println();
  Serial.println();
}


float getPhi(){
  /*************************************************
  *   Retorna o valor de phi na iteração atual.    *
  *************************************************/
  
  float out, in;
  
  in = getGammaFiltrado();
  out = horizontal - 360.0 - in;
  
  if(abs(out) > 179.99){
    out = round(out);
  }
  
  while(abs(out) > 180.0){
    out += 360.0;
  }
  
  if(abs(out) == 180.0){
    if(prevPhi < 0){
      out = -180.0;
    }
    else if(prevPhi > 0.0){
      out = 180.0;
    }
    else{
      if(prevGamma < 0.0){
        out = -180.0;
      }
      else{
        out = 180.0;
      }
    }
  }
  
  prevGamma = in;
  
  return out;
}



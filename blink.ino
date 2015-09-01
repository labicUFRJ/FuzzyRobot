void blink(){
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for a second
}

void blinkById(){
  if (noAtual!=-1){
    Serial.print("Piscarei ");
    Serial.print(noAtual);
    Serial.println(" vezes");
    int i;
    for (i=0; i<= noAtual;i++){
      blink();
      delay(1000);
    }
  }
    
}

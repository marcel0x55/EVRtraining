//goofy implementation cu wdt

ISR(WDT_vect){      //intrerupere de la watchdog timer
  if ((PIND & ((1 << PIND2) | (1 << PIND3) | (1 << PIND4))) == 0b00011100){
    PORTD = 0;
    return;
  }
  PORTD = (PORTD << 1) | 0b00000001;
}

void setup() {
  DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4);  //pini pentru leduri
  
  //folosire wdt ca timer 0.25s
  //din cate vad putem folosi pentru pwm doar timer1 (ca sa avem exact 1kHz)
  //iar timer0 si timer2 sunt de 8 biti asa ca nu putem scoate exact 0.25s si trebuie sa 
  //numaram intreruperi
  //avand in vedere ca nu folosim wdt il putem folosi pentru a numara 0.25s
  //TODO ar fi bine sa fie pus intro functie pentru a integra toate taskurile
  cli();                                              //dezactivam intreruperile cat lucram la wdt
  MCUSR &= ~(1 << WDRF);                              //ca sa ne asiguram ca putem scrie in wdtcsr
  WDTCSR |= (1 << WDCE) | (1 << WDE);                 //ca sa putem schimba configuratia wdt
  WDTCSR = (1 << WDIE) | (1 << WDP2);                 //activam intreruperile si setam prescaleru
                                                      //pentru 0.25s aka 0100
  sei();                                              //pornim inapoi intreruperile
}

void loop() {
  //totul se face in isr
}

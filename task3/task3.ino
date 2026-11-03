ISR(PCINT0_vect){
  if (!(PINB & (1 << PINB3))){
    PORTC ^= (1 << PORTC0);   //inversam starea doar atunci cand butonul e apasat pentru ca avem
                              //interrupt si la press si la release
  }
}

void setup() {
  DDRC |= (1 << DDC0);      //PC0 (A0) iesire
  PORTC &= ~(1 << PORTC0);  //Initial pe 0 
                            
  DDRB &= ~(1 << DDB3);     //PB3 (D11) input
  PORTB |= (1 << PORTB3);   //internal pull-up

  PCICR |= (1 << PCIE0);    //interrupt on change pe portul B
  PCMSK0 |= (1 << PCINT3);  //pornim interrupt on change pe pinul 11

  sei();                    //setam global interrupt enable
}

void loop() {
  //totul se intampla in ISR
}

uint8_t btnWasPressed = 0;

void setup() {
  //setup Task 2
  DDRD |= (1 << DDD5) | (1 << DDD6);
  PORTD |= (1 << PORTD5);
  PORTD |= (1 << PORTD7); //pornim weak pullup sa nu stea floating
  
  //setup Task 3
}

void debounceDelay(void){
  for (volatile uint32_t i = 0; i < 80000; i++){
    asm volatile ("nop");
  }
}

void loop() {
  //Task 2
  uint8_t btnIsPressed = !(PIND & (1 << PIND7));
  if (btnIsPressed && !btnWasPressed){
    debounceDelay();

    if (!(PIND & (1 << PIND7))){                //verificam inca odata pentru debounce mai bun
      PORTD ^= (1 << PORTD5) | (1 << PORTD6);
      btnWasPressed = 1;      //butonul e tinut
    }
  }else if (!btnIsPressed && btnWasPressed){
    debounceDelay();         //debounce la release
    if (PIND & (1 << PIND7)){ //verificam daca sigur e released
      btnWasPressed = 0;
    }
  }

  //Task 3
}

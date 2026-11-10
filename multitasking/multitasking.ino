uint8_t btnWasPressed = 0;    //parte din Task 2

volatile uint8_t leds = 0;    //parte din Task 4
const uint8_t LED_MASK = (1 << PIND2) | (1 << PIND3) | (1 << PIND4);  //parte din Task 4
                                                                      
uint8_t dir = 1;              //parte din Task 5

ISR(PCINT0_vect){   //interrupt on change pentru Task 3
  if (!(PINB & (1 << PINB3))){
  PORTC ^= (1 << PORTC0);   //inversam starea doar atunci cand butonul e apasat pentru ca avem
                            //interrupt si la press si la release
  }
}

ISR(WDT_vect){      //intrerupere de la watchdog timer pentru Task 4
  if (leds & LED_MASK == LED_MASK){         //if leds == 0b00011100
    leds = 0;
  }
  leds = (leds << 1) | (1 << PIND1);

  PORTD = (PORTD & ~LED_MASK) | (leds & LED_MASK);    //schimba starea ledurilor lasand ceilalti
                                                      //pini din PORTD neschimbati
}

void debounceDelay(void){   //parte din Task 2
  for (volatile uint32_t i = 0; i < 80000; i++){
    asm volatile ("nop");
  }
}

void setup() {
  //setup Task 2
  DDRD |= (1 << DDD5) | (1 << DDD6);
  PORTD |= (1 << PORTD5); //primul led aprins din start
  PORTD |= (1 << PORTD7); //pornim weak pullup sa nu stea floating
  

  //setup Task 3
  DDRC |= (1 << DDC0);      //PC0 (A0) iesire
  PORTC &= ~(1 << PORTC0);  //Initial pe 0

  DDRB &= ~(1 << DDB3);     //PB3 (D11) input
  PORTB |= (1 << PORTB3);   //internal pull-up

  PCICR |= (1 << PCIE0);    //interrupt on change pe portul B
  PCMSK0 |= (1 << PCINT3);  //pornim interrupt on change pe pinul 11


  //setup Task 4
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
                                                      

  //setup Task 5
  DDRB |= (1 << DDB1) | (1 << DDB2);    //pinii 9 si 10 output pentru pwm

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;                            //resetam registrii de control pentru timer1

  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);     //OC1A/B clear on compare match set at
                                                          //bottom (non inverting)
                                                          //WGM13|WGM12|WGM11|WGM10 = 1110
                                                          //corespunde la Fast PWM cu TOP = ICR1;
                                                          //CS12|CS11|CS10 = 010 corespunde
                                                          //unui prescaler de 1:8
  ICR1 = 1999;                          //TOP pentru 1kHz PWM (16MHz / (8 * 1000Hz) - 1)
  OCR1A = 0;                            //Duty cycle 0%
  OCR1B = 0;                            //Duty cycle 0%


  //setup Task 6
  //pentru PWM deja facut la setup Task 5
  //setup ADC     TODO: refactor in functie
  ADMUX = (1 << REFS0) | (1 << MUX0) | (1 << MUX2);   //MUX = 0101 adica pinul A5
                                                      //pe care avem potentiometrul
                                                      //REFS = 01 adica folosim VCC ca referinta
                                                      //ADLAR = 0 adica aliniere default
                                                      //bitii 0 pana la 7 in ADCL, 8 si 9 in ADCH
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
      //pornim ADC-ul si setam prescaleru pe 1:128 pentru ca nu avem nevoie de viteza dar
      //acuratetea ar fi binevenita


  //setup Task 7



  sei();                                              //pornim intreruperile la final
}

void loop() {
  ADCSRA |= (1 << ADSC);                //dam start la o conversie ADC pentru taskurile 6 si 7,
                                        //facem asta la inceput pentru a minimiza deleiul adica
                                        //pentru a face ceva util pana ADC-ul termina conversia

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
  //totul se intampla in ISR


  //Task 4
  //totul se intampla in ISR


  //Task 5
  if (dir){
    OCR1A++;
  }else {
    OCR1A--;
  }                   //crestem sau scadem duty cycle in functie de directie

  for (uint32_t volatile i = 0; i < 1000; i++); //mic delei

  if (OCR1A == 1999){
    dir = !dir;       //inversam directia
  }else if (OCR1A == 0){
    dir = !dir;       //inversam directia
  }


  //Task 6
  while (ADCSRA & (1 << ADSC));         //asteptam sa se termine conversia (cleared in hardware)
  OCR1B = ADC;                          //punem direct valoarea de pe adc ca duty cycle


  //Task 7
}

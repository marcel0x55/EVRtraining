//Taskurile 5 si 6 folosesc secvente de cod comune pentru ca folosesc acelasi timer pentru pwm

void setup() {
  DDRB |= (1 << DDB1) | (1 << DDB2);    //pinii 9 si 10 output pentru pwm

  //setup Timer1    TODO: refactor in functie
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
  OCR1A = 0;                            //Duty cycle 0% pe pin 9
  OCR1B = 0;                            //Duty cycle 0% pe pin 10
                
  //setup ADC     TODO: refactor in functie
  ADMUX = (1 << REFS0) | (1 << MUX0) | (1 << MUX2);   //MUX = 0101 adica pinul A5 
                                                      //pe care avem potentiometrul
                                                      //REFS = 01 adica folosim VCC ca referinta
                                                      //ADLAR = 0 adica aliniere default 
                                                      //bitii 0 pana la 7 in ADCL, 8 si 9 in ADCH
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
      //pornim ADC-ul si setam prescaleru pe 1:128 pentru ca nu avem nevoie de viteza dar
      //acuratetea ar fi binevenita
}

void loop() {
  ADCSRA |= (1 << ADSC);                //dam start la o conversie ADC
  while (ADCSRA & (1 << ADSC));         //asteptam sa se termine conversia (cleared in hardware)
  OCR1B = ADC;                          //punem direct valoarea de pe adc ca duty cycle
}

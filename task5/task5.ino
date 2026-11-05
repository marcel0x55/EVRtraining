uint8_t dir = 1;

void setup() {
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
}

void loop() {
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
}

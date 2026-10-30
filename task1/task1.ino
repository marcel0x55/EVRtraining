void mydelay(uint32_t val){
  for(uint32_t i = 0; i < val; i++){
    asm volatile ("nop");
  }
}

void setup() {
  DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4);
}

void loop() {
  PORTD = 0;
  mydelay(400000);
  PORTD |= (1 << PIND2);
  mydelay(400000);
  PORTD |= (1 << PIND3);
  mydelay(400000);
  PORTD |= (1 << PIND4);
  mydelay(800000);
}

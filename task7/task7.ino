#define AS1115_ADDR 0x00 //adresa i2c default

void i2c_start(void){   //generam un start condition
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); //clear twint pentru a trimite (write 1 to 
                                                    //clear),
                                                    //trimitem start condition,
                                                    //pornim TWI
  while (!(TWCR & (1 << TWINT)));                   //polling la bit sa asteptam pana termina
                                                    //transmisia
}

void i2c_stop(void){
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);               //clear twint pentru a trimite
                                                                   //TWSTO -> generam stop cmd
                                                                   //TWEN -> lasam two wire
                                                                   //peripheral pornit
  //daca incheiem tranzactia nu mai are rost sa asteptam pana termina de trimis
}

void i2c_write(uint8_t data){
  TWDR = data;                        //punem byte-ul pe care vrem sa il trimitem in 
                                      //two wire data register

  TWCR = (1 << TWINT) | (1 << TWEN);  //clear TWINT -> trigger transmission
                                      //TWEN = 1 -> lasam perifericul pornit
  while (!(TWCR & (1 << TWINT)));     //Asteptam pana se trimite byte-ul
}

//in acest caz nu avem nevoie de functii de read pentru i2c pentru ca in cazul driverului AS1115
//doar trimitem

void setup() {
  //i2c setup
  TWSR = 0x00;        //prescaler = 1 (TWPS1 = 00)
  TWBR = 72;          //(16MHz / (16 + 2 * TWBR * prescaler)) = 100kHz -> 16MHz/(16 + 2 * 72 * 1)
  TWCR = (1 << TWEN); //pornim two wire peripheral
                      
  //initializarea AS1115
  i2c_start();
  i2c_write((AS1115_ADDR << 1) | 0);    //Trimitem adresa pe 7 biti + write bit 0
  i2c_write(0x0C);                      //0x0C = shutdown register
  i2c_write(0x01);                      //il punem in Wake Up / Normal Operation
  i2c_stop();

  i2c_start();
  i2c_write((AS1115_ADDR << 1) | 0);    //Trimitem adresa pe 7 biti + write bit 0
  i2c_write(0x0A);                      //0x0A = brightness register
  i2c_write(0x0F);                      //max brightness
  i2c_stop();

  i2c_start();
  i2c_write((AS1115_ADDR << 1) | 0);    //Trimitem adresa pe 7 biti + write bit 0
  i2c_write(0x09);                      //0x09 = decode mode register
  i2c_write(0x0F);                      //pornim decode pentru toate 4 digits
  //un pic de cheating cu decode mode ca sa trimitem cifra direct sa nu trebuiasca sa transformam
  //in masca
  
  //setup ADC
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
  ADCSRA |= (1 << ADSC);          //initiem o conversie ADC

  uint16_t adcval = ADC;
  uint16_t mv = ((uint32_t)adcval * 5000) / 1024;     //scalam valoarea din adc 0-1023 in
                                                      //milivolti 0-5000 (trebuie cast la uint32
                                                      //ca sa nu dea overflow cand inmultim
  uint8_t d3u = mv / 1000;            //unitati
  uint8_t d2z = (mv / 100) % 10;      //zecimi
  uint8_t d1s = (mv / 10) % 10;       //sutimi
  uint8_t d0m = mv % 10;              //miimi

  i2c_start();
  i2c_write((AS1115_ADDR << 1) | 0);    //Trimitem adresa pe 7 biti + write bit 0
  
  i2c_write(0x01);                      //primul digit

  //trimitem cifrele cu auto increment (nu e necesar sa trimitem stop command dupa fiecare si
  //sa mutam digit register
  i2c_write(d0m);
  i2c_write(d1s);
  i2c_write(d2z);
  i2c_write(d3u | 0x80);              //0x80 -> pune decimal point 

  i2c_stop();

  while (ADCSRA & (1 << ADSC));         //asteptam sa se termine conversia (cleared in hardware)
                                        //facem asta la sfarsit ca sa nu asteptam prea mult
}

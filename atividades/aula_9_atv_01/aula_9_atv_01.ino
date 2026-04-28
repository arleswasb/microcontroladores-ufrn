void setup() {
  // CONFIGURANDO DE PINOS (I/O)
  // Pino 9 -> PB1 (OC1A - saida PWM do Timer1)
  DDRB |= (1 << PB1);

  // CONFIGURANDO O ADC (Leitura do Potenciometro)
  // ADMUX:
  // REFS0 = 1 -> Referencia AVcc (5V)
  // MUX = 0000 -> Canal ADC0 (A0)
  ADMUX = (1 << REFS0);

  // ADCSRA:
  // ADEN -> habilita ADC
  // ADPS2:0 -> prescaler 128 (125kHz)
  ADCSRA = (1 << ADEN)  |
           (1 << ADPS2) |
           (1 << ADPS1) |
           (1 << ADPS0);

  // CONFIGURANDO TIMER1 (PWM - pino 9)
  // Modo: Fast PWM 8 bits

  // TCCR1A:
  // COM1A1 = 1 -> ativa PWM no OC1A
  // WGM10 = 1 -> modo PWM
  TCCR1A = (1 << COM1A1) | (1 << WGM10);

  // TCCR1B:
  // WGM12 = 1 -> completa modo Fast PWM
  // CS11 = 1 -> prescaler 8
  TCCR1B = (1 << WGM12) | (1 << CS11);

  // REALIZANDO INICIALIZACAO DE SEGURANCA
  EICRA = 0;
  EIMSK = 0;
}

// =======================================================
// Funcao: leitura_ADC
// Descricao: Le o valor do potenciometro (A0)
// Retorno: 0 a 1023
// =======================================================
unsigned int leitura_ADC() {

  // Inicia conversao
  ADCSRA |= (1 << ADSC);

  // Aguarda finalizar
  while (ADCSRA & (1 << ADSC)) {
    __asm__("nop");
  }

  return ADC;
}

// =======================================================
// Funcao: reescrever_PWM
// Descricao: Atualiza o duty cycle do PWM (pino 9)
// Parametro: 0 a 255
// =======================================================
void reescrever_PWM(unsigned char valor) {

  // OCR1A controla o PWM
  OCR1A = valor;
}

// LOOP PRINCIPAL (EXECUCAO CONTINUA EM TEMPO REAL)
void loop() {
  // lLENDO O POTENCIOMETRO
  unsigned int valorPot = leitura_ADC(); // 0 a 1023

  // CONVERSTENDO PARA PWM
  // Divide por 4 -> escala 0 a 255
  unsigned char velocidade = valorPot >> 2;

  // ATUALIZANDO O PWM (CONTROLE DO MOTOR)
  reescrever_PWM(velocidade);
}
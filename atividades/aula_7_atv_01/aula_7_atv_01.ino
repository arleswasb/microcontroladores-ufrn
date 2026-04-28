#include <avr/io.h>
#include <util/delay.h>
// Inicializa a UART para 9600 bps
void UART_init() {
  // Baud rate = 9600
  // UBRR = (F_CPU / (16 * BAUD)) - 1
  // Para 16MHz: UBRR ≈ 103
  UBRR0H = 0;
  UBRR0L = 103;
  // Habilita transmissor (TX)
  UCSR0B = (1 << TXEN0);

  // Define formato: 8 bits de dados, 1 stop bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
// Envia um caractere via UART
void UART_transmit(char data) {
  // Espera buffer ficar vazio
  while (!(UCSR0A & (1 << UDRE0)));

  // Coloca dado no registrador de envio
  UDR0 = data;
}
// Envia string
void UART_print(const char *str) {
  while (*str) {
    UART_transmit(*str++);
  }
}
// Envia número inteiro (simples)
void UART_printInt(int num) {
  char buffer[10];
  itoa(num, buffer, 10);
  UART_print(buffer);
}
// Inicializa o ADC
void ADC_init() {
  // Referência AVcc (5V) e canal A0
  ADMUX = (1 << REFS0);  

  // Habilita ADC e prescaler 128 (125kHz)
  ADCSRA = (1 << ADEN) | 
           (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}
// Lê canal ADC (A0 = 0)
uint16_t ADC_read(uint8_t channel) {
  // Limpa bits anteriores do canal
  ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

  // Inicia conversão
  ADCSRA |= (1 << ADSC);

  // Espera terminar
  while (ADCSRA & (1 << ADSC));

  // Retorna valor (10 bits)
  return ADC;
}
int main(void) {
  UART_init();
  ADC_init();

  while (1) {
    int valor = ADC_read(0); // leitura do A0

    UART_print("Leitura: ");
    UART_printInt(valor);
    UART_print(" -> ");

    if (valor < 341) {
      UART_print("BAIXO\r\n");
    }
    else if (valor <= 682) {
      UART_print("MEDIO\r\n");
    }
    else {
      UART_print("ALTO\r\n");
    }
    _delay_ms(500);
  }
}
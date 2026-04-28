#include <avr/io.h>
#include <stdlib.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

static char buffer[5];
static uint8_t i = 0;
static uint32_t last_char_time = 0;

void USART_Print(const char* str) {
    while (*str) {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = *str++;
    }
}

void setup() {
    // Configuracao USART (9600 bps, 8-N-1)
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)MYUBRR;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Configuracao PWM no Pino 9 (PB1) - Timer 1 Fast PWM 8-bit
    DDRB |= (1 << DDB1);
    TCCR1A = (1 << COM1A1) | (1 << WGM10);
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
    OCR1A = 0; 
}

void loop() {
    if (UCSR0A & (1 << RXC0)) {
        char c = UDR0;
        if (c >= '0' && c <= '9') {
            if (i < 4) {
                buffer[i++] = c;
                last_char_time = millis(); 
            }
        }
    }

    // Processamento por timeout (Fim de Frame)
    if (i > 0 && (millis() - last_char_time > 500)) {
        buffer[i] = '\0';
        long valor = atol(buffer);
        i = 0;

        if (valor >= 0 && valor <= 255) {
            OCR1A = (uint8_t)valor;
            USART_Print("Sucesso: Brilho alterado.\r\n");
        } else {
            USART_Print("ERRO: Valor fora do intervalo (0-255).\r\n");
        }
    }
}
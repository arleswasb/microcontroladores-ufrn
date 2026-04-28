#include <avr/io.h>

// Tempo global em ms
int tempo_ms = 1000;

// Timer1
void timer1_init() {   
    // CS11 e CS10 = 1, prescaler = 64
    TCCR1B = (1 << CS11) | (1 << CS10);
    // 1 ms = 250 ticks, 0 ate 249
    OCR1A = 249;
}

// Delay baseado no Timer1 (em milissegundos)
void delay_ms_timer(unsigned int ms) {
    for (unsigned int i = 0; i < ms; i++) {
        // Zera o contador para iniciar nova contagem
        TCNT1 = 0;
        // Espera ate o timer atingir OCR1A
        while (TCNT1 < OCR1A) {
            __asm__("nop"); // Impede otimizacao agressiva
        }
    }
}

int main(void) {
    // DDRB: define direcao dos pinos da porta B
    // PB5 como saida (LED onboard)
    DDRB |= (1 << PB5);
    // Inicia o temporizador
    timer1_init();

    while (1) {
        // Liga o LED (coloca PB5 em nivel alto)
        PORTB |= (1 << PB5);
        // Espera tempo definido na variavel global
        delay_ms_timer(tempo_ms);
        // Desliga o LED (coloca PB5 em nivel baixo)
        PORTB &= ~(1 << PB5);
        // Espera novamente
        delay_ms_timer(tempo_ms);
    }
}
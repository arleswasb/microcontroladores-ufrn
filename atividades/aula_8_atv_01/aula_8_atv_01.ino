#include <avr/io.h>
#include <avr/interrupt.h>

// Variavel global volatil para contagem de interrupcoes
volatile uint8_t cont = 0;

int main(void) {
    // Configuracao dos LEDs como saida
    DDRB |= 0b00011100;
    
    // Configuracao do botao como entrada com pull-up interno
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);
    
    // Configuracao da interrupcao externa INT0
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    EIMSK |= (1 << INT0);
    
    // Habilita interrupcoes globais
    sei();
    
    while (1) {
        // Atualiza os LEDs com o valor da contagem
        PORTB = (cont << 2);
    }
}

// Rotina de interrupcao externa
ISR(INT0_vect) {
    // Contagem ciclica limitada a 3 bits (0 a 7) por mascara binaria
    cont = (cont + 1) & 0b00000111;
}
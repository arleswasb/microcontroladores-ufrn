#include <avr/io.h>

// Variavel que controla o periodo de piscagem (em ms)
int tempo = 500;

// Configura o Hardware do Timer 1 (16 bits) para uma base de tempo de 1ms.
// O clock de 16MHz com prescaler de 64 resulta em 250.000 incrementos/segundo.
void setup_timer() {
    // TCCR1A e TCCR1B: Registradores de controle do Timer 1
    TCCR1A = 0; // Modo normal de operacao

    // CS11 e CS10 em 1: Define o Prescaler (divisor) em 64
    TCCR1B = (1 << CS11) | (1 << CS10);

    // OCR1A: Registrador de comparacao. 250 ticks equivalem a exatamente 1ms.
    OCR1A = 250;
}

// Funcao delay_timer: Pausa a execucao por 'ms' milissegundos.
// Utiliza o registrador TCNT1 que conta fisicamente os ciclos do processador.
void delay_timer(unsigned int ms) {
    for (unsigned int i = 0; i < ms; i++) {
        // TCNT1: Registrador que guarda a contagem atual do Timer
        TCNT1 = 0; // Reseta o contador para iniciar um novo ciclo de 1ms
        // Polling: segura a CPU ate o contador atingir o valor de 1ms (OCR1A)
        while (TCNT1 < OCR1A) {
            // Instrucao assembly NOP (No Operation) impede que o compilador
            // exclua o loop durante a otimizacao.
            __asm__("nop");
        }
    }
}

int main(void) {
    // DDRB: Data Direction Register B.
    // Atribuicao direta (=) garante que APENAS o PB5 (pino 13) seja SAIDA (1).
    DDRB = (1 << DDB5);

    // Inicializa a configuracao do hardware de tempo
    setup_timer();

    while (1) {
        // PORTB: Registrador de dados da Porta B.
        // Seta o bit 5: nivel logico alto -> LED aceso.
        PORTB |= (1 << PORTB5);
        delay_timer(tempo);

        // Limpa o bit 5: nivel logico baixo -> LED apagado.
        PORTB &= ~(1 << PORTB5);
        delay_timer(tempo);
    }

    return 0;
}

#include <avr/io.h>

// ==========================================================================
// VARIÁVEL GLOBAL
// ==========================================================================
// Controla o período de piscagem do LED (em milissegundos)
unsigned int tempo = 400;


/*
 * ============================================================================
 * Função: inicializar_temporizador
 * Descrição: Configura o Timer1 para gerar uma base de tempo de ~1 ms
 * ============================================================================
 */
void inicializar_temporizador()
{
    // ==========================================================================
    // 1. CONFIGURAÇÃO DO TIMER1 (Hardware de contagem de tempo)
    // ==========================================================================
    // TCCR1A e TCCR1B (Timer/Counter Control Registers):
    // Controlam o modo de operação e o prescaler do Timer1 (16 bits)

    TCCR1A = 0; // Garante modo normal

    // Clock do sistema: 16 MHz
    // Prescaler 64 → 16.000.000 / 64 = 250.000 Hz
    // Logo:
    // 250.000 ticks = 1 segundo
    // 250 ticks = 1 ms

    // CS11 e CS10 = 1 → ativa prescaler 64
    TCCR1B = (1 << CS11) | (1 << CS10);

    // OCR1A (Output Compare Register 1A):
    // Define o valor alvo para 1 ms
    OCR1A = 250;
}


/*
 * ============================================================================
 * Função: atraso_temporizador
 * Descrição: Gera atraso em milissegundos usando hardware (Timer1)
 * Parâmetro: ms → tempo de atraso
 * ============================================================================
 */
void atraso_temporizador(unsigned int ms)
{
    // Repete o ciclo de 1 ms 'ms' vezes
    for (unsigned int i = 0; i < ms; i++) {

        // ==========================================================================
        // 2. CONTAGEM DE TEMPO (TCNT1)
        // ==========================================================================
        // TCNT1 (Timer/Counter Register):
        // Armazena o valor atual da contagem do Timer

        TCNT1 = 0; // Reinicia contagem

        // Loop de espera até atingir ~1 ms
        while (TCNT1 < OCR1A) {

            // ==========================================================================
            // IMPORTANTE (EVITAR OTIMIZAÇÃO DO COMPILADOR)
            // ==========================================================================
            // "nop" (No Operation):
            // Consome 1 ciclo de clock e impede que o compilador remova o loop
            __asm__("nop");
        }
    }
}


// ==========================================================================
// FUNÇÃO PRINCIPAL
// ==========================================================================
int main(void)
{
    // ==========================================================================
    // 1. CONFIGURAÇÃO DE PINOS (I/O)
    // ==========================================================================
    // DDRB (Data Direction Register B):
    // Define direção dos pinos da Porta B

    // DDB5 = 1 → PB5 (pino 13) como saída
    // Atribuição direta (=) garante controle total do registrador
    DDRB = (1 << DDB5);


    // ==========================================================================
    // 2. INICIALIZAÇÃO DO TEMPORIZADOR
    // ==========================================================================
    inicializar_temporizador();


    // ==========================================================================
    // 3. LOOP PRINCIPAL (EXECUÇÃO CONTÍNUA)
    // ==========================================================================
    // Em sistemas embarcados não há sistema operacional,
    // portanto usamos loop infinito
    while (1) {

        // ==========================================================================
        // 3.1 SEQUÊNCIA DE PISCAGEM (5 VEZES)
        // ==========================================================================
        for (unsigned int i = 0; i < 5; i++) {

            // --- LIGA O LED ---
            // PORTB (Port Data Register B):
            // Escreve nível lógico nos pinos

            // Seta PB5 → nível alto (5V)
            PORTB |= (1 << PORTB5);

            atraso_temporizador(tempo);


            // --- DESLIGA O LED ---
            // Limpa PB5 → nível baixo (0V)
            PORTB &= ~(1 << PORTB5);

            atraso_temporizador(tempo);
        }


        // ==========================================================================
        // 3.2 PAUSA ENTRE CICLOS
        // ==========================================================================
        // Aguarda 2000 ms (2 segundos)
        atraso_temporizador(2000);
    }

    return 0;
}
/* * Atividade 2 - Aula 06: Semáforo com ciclo contínuo
 * Implementacao purista utilizando Registradores e Hardware Timer 1.
 */

#include <avr/io.h>

/*
 * ============================================================================
 * Função: setup_timer
 * Descrição: Configura o hardware do Timer 1 para uma base de tempo de 1ms.
 * ============================================================================
 */
void setup_timer() {
  // TCCR1A e TCCR1B (Timer/Counter Control Registers):
  // Controlam o modo de operação e o divisor de frequência (prescaler).
  TCCR1A = 0; // Garantimos o modo normal de operação[cite: 1474].

  // O cristal oscilador do Arduino UNO opera a 16 MHz[cite: 451].
  // Aplicamos um "Prescaler" de 64: 16.000.000 / 64 = 250.000 Hz[cite: 1476].
  // Isso significa que o contador (TCNT1) incrementa 250.000 vezes por segundo[cite: 1477].
  // Definimos os bits CS11 e CS10 para ativar essa divisão no hardware[cite: 1478, 1479].
  TCCR1B = (1 << CS11) | (1 << CS10); 
  
  // OCR1A (Output Compare Register 1 A): Valor alvo para comparação.
  // Se 250.000 incrementos equivalem a 1s, então 250 incrementos equivalem a 1ms[cite: 1480].
  OCR1A = 250;
}

/*
 * ============================================================================
 * Função: delay_timer
 * Descrição: Pausa a execução utilizando o registrador TCNT1 do hardware.
 * Parâmetros: ms (Quantidade de milissegundos a pausar)
 * ============================================================================
 */
void delay_timer(unsigned int ms) {
  for (unsigned int i = 0; i < ms; i++) {
    // TCNT1: Registrador que guarda a contagem física dos pulsos de clock[cite: 1486, 1487].
    TCNT1 = 0; // Zeramos o contador para iniciar um novo ciclo de 1ms.

    // O laço segura a CPU enquanto o contador não atinge o alvo de 1ms (250 ticks)[cite: 1488, 1489].
    while (TCNT1 < OCR1A) {
      // Instrução Assembly "No Operation" (NOP) para evitar que o compilador
      // delete o laço vazio durante o processo de otimização[cite: 1490, 1491, 1492].
      __asm__("nop");
    }
  }
}

int main(void) {
  // DDRB (Data Direction Register B): Define a direção dos pinos do Port B[cite: 1298, 1470].
  // Mapeamento: PB0 (Verde/D8), PB1 (Amarelo/D9) e PB2 (Vermelho/D10)[cite: 1417].
  // A máscara (7 << DDB0) equivale a 00000111 em binário, setando os 3 pinos como Saída.
  DDRB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2);
  
  setup_timer(); // Inicializa a configuração dos registradores de tempo.

  while (1) {
    // --- ESTADO: VERDE ---
    // Atribuição direta (=) limpa todos os bits do PORTB enquanto liga apenas PB0[cite: 1497, 1498].
    PORTB = (1 << PORTB0); 
    delay_timer(5000); // Aguarda 5 segundos em hardware.

    // --- ESTADO: AMARELO ---
    // Garante exclusividade: apenas o bit PB1 (pino 9) fica em nível ALTO (5V)[cite: 1462].
    PORTB = (1 << PORTB1);
    delay_timer(2000); // Aguarda 2 segundos em hardware.

    // --- ESTADO: VERMELHO ---
    // Apenas o bit PB2 (pino 10) fica ativo, resetando PB1 e PB0 no mesmo ciclo[cite: 1417].
    PORTB = (1 << PORTB2);
    delay_timer(5000); // Aguarda 5 segundos em hardware.
  }
  return 0;
}

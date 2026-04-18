/* * Atividade 2 - Aula 05: Controle de Frequencia do LED Built-in
 * Implementacao purista utilizando Registradores e Hardware Timer 1.
 */

#include <avr/io.h>

/* 
 * ============================================================================
 * Função: setup_timer
 * Descrição: Configura o Hardware do Timer 1 (16 bits) para uma base de tempo de 1ms.
 * O clock de 16MHz com prescaler de 64 resulta em 250.000 incrementos/segundo.
 * ============================================================================
 */
void setup_timer() {
  // TCCR1A e TCCR1B: Registradores de controle do Timer 1
  TCCR1A = 0; // Modo normal de operacao
    
  // CS11 e CS10 em 1: Define o Prescaler (divisor) em 64
  TCCR1B = (1 << CS11) | (1 << CS10); 
    
  // OCR1A: Registrador de comparacao. 250 ticks equivalem a exatamente 1ms.
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
  // Atribuição direta (=) garante que apenas o PB5 (pino 13) seja saída (1).
  DDRB = (1 << DDB5);

  // Inicializa a configuracao do hardware de tempo
  setup_timer();

  // Variavel de modo: 1 para rapido (200ms), 0 para lento (1000ms)
  uint8_t modo = 1;

  while (1) {
    // PORTB: Registrador de dados da Porta B.
    // Operador XOR (^) inverte o estado do bit 5 (Toggle do LED L).
      PORTB ^= (1 << PORTB5); 

      // Selecao da frequencia baseada no modo
    if (modo == 1) {
      delay_timer(200);  // Espera 200ms (5 Hz)
    } else {
      delay_timer(1000); // Espera 1000ms (1 Hz)
    }
  }
  return 0;
}

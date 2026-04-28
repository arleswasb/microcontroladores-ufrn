void setup() {
  // ==========================================
  // 1. CONFIGURAÇÃO DE PINOS (I/O)
  // ==========================================
  // DDRB (Data Direction Register B): Registrador que define se os pinos da 
  // Porta B atuarão como Entrada (0) ou Saída (1).
  // O pino 13 do Arduino corresponde fisicamente ao pino PB5 do microcontrolador.
  // A operação (1 << PB5) cria a máscara binária 00100000.
  // O operador '|=' (OR) aplica a máscara, setando apenas o bit 5 como Saída (1),
  // preservando as configurações dos outros pinos da Porta B.
  DDRB |= (1 << PB5); 
  // ==========================================
  // 2. CONFIGURAÇÃO DO TIMER 1 (Hardware de contagem de tempo)
  // ==========================================
  // TCCR1A e TCCR1B (Timer/Counter Control Registers): Controlam o modo de 
  // operação e a velocidade (prescaler) do Timer de 16 bits.
  TCCR1A = 0; // Zeramos para garantir o modo normal de operação.  
  // O microcontrolador roda a um clock de 16 MHz (16.000.000 ciclos por segundo).
  // Se dividirmos isso por um "Prescaler" de 64, o Timer contará a 250.000 Hz.
  // Ou seja: 250.000 incrementos = 1 segundo. Logo, 250 incrementos = 1 milissegundo.
  // Setamos os bits CS11 e CS10 para 1, o que ativa o Prescaler de 64 no hardware.
  TCCR1B = (1 << CS11) | (1 << CS10); 
  // OCR1A (Output Compare Register 1 A): Registrador que guarda o nosso valor alvo.
  // Como calculamos acima que 1ms exige 250 "ticks" do timer, salvamos esse limite aqui.
  OCR1A = 250; 
  // ===========================================
  // 3. INICIALIZAÇÃO DE SEGURANÇA DOS DEMAIS REGISTRADORES EXIGIDOS
  // ===========================================
  // Como o escopo do projeto exige a manipulação dos registradores de Conversão 
  // Analógica (ADC) e Interrupções Externas, nós os zeramos na inicialização 
  // para garantir que essas funções estejam explicitamente desativadas, 
  // evitando comportamentos erráticos no sistema.
  
  ADMUX = 0;   // Desativa a seleção do multiplexador analógico
  ADCSRA = 0;  // Desativa o controle e status do ADC
  EICRA = 0;   // Limpa as configurações de gatilho das interrupções externas
  EIMSK = 0;   // Desabilita as máscaras de interrupção externa
}

/*
 * ==============================================
 * Função: delay_timer
 * Descrição: Pausa a execução do programa por um tempo determinado em hardware.
 * Parâmetros: ms (Quantidade de milissegundos a pausar)
 * ==============================================
 */
void delay_timer(unsigned int ms) {
  // Repete o ciclo de 1ms pela quantidade de vezes solicitada
  for (unsigned int i = 0; i < ms; i++) {
    
    // TCNT1 (Timer/Counter 1): É o registrador que efetivamente guarda a contagem.
    // Zeramos ele no início de cada ciclo de 1 milissegundo.
    TCNT1 = 0; 
    
    // O laço segura a execução do código aqui enquanto o contador for menor 
    // que o nosso alvo de 1ms (que definimos como 250 no OCR1A).
    while (TCNT1 < OCR1A) {
      
      // TRUQUE PARA SIMULADORES (Tinkercad):
      // __asm__("nop") insere a instrução Assembly "No Operation" (Não faça nada).
      // Isso consome exato 1 ciclo de clock do processador. É obrigatório aqui 
      // porque compiladores modernos são espertos e deletam laços 'while' vazios 
      // para otimizar o código. O "nop" força o laço a existir e o tempo a passar.
      __asm__("nop"); 
    }
  }
}
/*
 * =============================================
 * Função: piscaled
 * Descrição: Alterna o estado do pino PB5 entre ALTO (5V) e BAIXO (0V).
 * Parâmetros: 
 * - numPiscadas: Quantidade de vezes que o LED vai acender e apagar.
 * - tempoDelay: Duração (em milissegundos) que ele passa em cada estado.
 * =============================================
 */
 
void piscaled(int numPiscadas, int tempoDelay) {
  for (int i = 0; i < numPiscadas; i++) {  
    // --- LIGA O LED ---
    // PORTB (Port Data Register B): Define a voltagem de saída dos pinos.
    // (1 << PB5) = Máscara 00100000.
    // Operador OR (|=): Se o bit for 1 na máscara, vira 1 no PORTB. O resto não muda.
    PORTB |= (1 << PB5);  

    
    delay_timer(tempoDelay); // Aguarda o tempo estipulado com o LED aceso
    // --- DESLIGA O LED ---
    // ~(1 << PB5) = Inverte a máscara, gerando 11011111.
    // Operador AND (&=): Como o bit 5 da máscara é 0, a porta lógica AND 
    // força o pino PB5 a virar 0 (GND). Os outros pinos são multiplicados 
    // por 1, mantendo seus estados originais intocados.
    PORTB &= ~(1 << PB5); 
    
    delay_timer(tempoDelay); // Aguarda o mesmo tempo com o LED apagado
  }
}

// ==========================================
// CICLO PRINCIPAL (Equivalente ao comportamento rotineiro do sistema)
// ==========================================

void loop() {
  // Chama a nossa função modularizada: pisca 5 vezes com pulsos de 200ms
  piscaled(5, 200);      
  // Pausa longa de 2 segundos antes de mudar de comportamento
  delay_timer(2000);       
  // Chama a função novamente: pisca 3 vezes com pulsos longos de 600ms
  piscaled(3, 600);        
  // Pausa de 2 segundos antes de o Arduino reiniciar o loop automaticamente
  delay_timer(2000);     
}
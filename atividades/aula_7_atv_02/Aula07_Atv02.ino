#include <avr/io.h>

// ---------- Parametros do circuito ----------
#define VREF    5.0f       // Tensao de referencia do ADC (AVcc = 5 V)
#define R_SERIE 220.0f     // Resistor em serie com o LED (ohms)
#define VF_LED  2.0f       // Tensao direta tipica do LED vermelho (V)
#define I_TH    0.005f     // Limiar de corrente para acender: 5 mA

// ================================================================
//                  Configuracao do Timer 1
// ================================================================
// Base de tempo de 1ms: clock 16MHz / prescaler 64 = 250kHz -> 250 ticks = 1ms
void setup_timer() {
    TCCR1A = 0;
    TCCR1B = (1 << CS11) | (1 << CS10); // Prescaler 64
    OCR1A = 250;
}

void delay_timer(unsigned int ms) {
    for (unsigned int i = 0; i < ms; i++) {
        TCNT1 = 0;
        while (TCNT1 < OCR1A) {
            __asm__("nop");
        }
    }
}

// ================================================================
//                  Inicializacao do ADC
// ================================================================
void setup_adc() {
    // ADMUX:
    //  REFS1:REFS0 = 01 -> referencia em AVcc (5V) com capacitor em AREF
    //  ADLAR = 0         -> resultado alinhado a direita (10 bits completos)
    //  MUX3..MUX0 = 0000 -> canal ADC0 (pino A0)
    ADMUX = (1 << REFS0);

    // ADCSRA:
    //  ADEN = 1           -> habilita o modulo ADC
    //  ADPS2:ADPS0 = 111  -> prescaler = 128
    //  Clock do ADC: 16MHz / 128 = 125kHz (faixa recomendada: 50-200kHz)
    ADCSRA = (1 << ADEN)
           | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

// Le o valor do canal ADC especificado (0 a 1023)
uint16_t adc_read(uint8_t canal) {
    // Seleciona o canal preservando os bits de referencia (REFS1:REFS0)
    ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);

    // ADSC: ADC Start Conversion - inicia uma conversao
    ADCSRA |= (1 << ADSC);

    // Polling: aguarda ADSC retornar a 0 (conversao concluida)
    while (ADCSRA & (1 << ADSC)) {
        __asm__("nop");
    }

    // ADC: registrador de 16 bits (ADCL + ADCH) com o resultado
    return ADC;
}

// ================================================================
//                  Inicializacao da UART
// ================================================================
void setup_uart(uint16_t baud) {
    // UBRR0: define o baud rate. Formula: UBRR = (F_CPU / 16 / baud) - 1
    // Para 9600 bps com 16MHz: UBRR = 103
    uint16_t ubrr = (16000000UL / 16UL / baud) - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr);

    // UCSR0B: habilita transmissor (TXEN0) e receptor (RXEN0)
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    // UCSR0C: frame 8N1 (8 bits de dados, sem paridade, 1 stop bit)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmite um caractere pela UART
void uart_tx(char c) {
    // UDRE0: bit que indica buffer de transmissao vazio
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c; // UDR0: registrador de dados - escrever aqui envia o byte
}

// Transmite uma string pela UART
void uart_print(const char *s) {
    while (*s) uart_tx(*s++);
}

// Transmite um inteiro sem sinal pela UART
void uart_print_uint(uint16_t v) {
    char buf[6];
    int i = 0;
    if (v == 0) { uart_tx('0'); return; }
    while (v) { buf[i++] = '0' + (v % 10); v /= 10; }
    while (i--) uart_tx(buf[i]);
}

// Transmite um float com 2 casas decimais pela UART
void uart_print_float(float v) {
    if (v < 0) { uart_tx('-'); v = -v; }
    uint16_t inteiro = (uint16_t)v;
    uint16_t frac    = (uint16_t)((v - inteiro) * 100.0f);
    uart_print_uint(inteiro);
    uart_tx('.');
    if (frac < 10) uart_tx('0');
    uart_print_uint(frac);
}

// ================================================================
//                        Programa Principal
// ================================================================
int main(void) {
    // DDRB: configura PB0 (pino digital 8) como saida para o LED externo
    DDRB = (1 << DDB0);
    // PORTB: inicia com LED apagado
    PORTB &= ~(1 << PORTB0);

    // Inicializa perifericos
    setup_timer();
    setup_adc();
    setup_uart(9600);

    while (1) {
        // 1) Le o canal 0 do ADC (pino A0 - potenciometro)
        uint16_t leitura = adc_read(0);

        // 2) Converte a leitura para tensao (0 a 5V)
        float tensao = ((float)leitura) * (VREF / 1023.0f);

        // 3) Estima a corrente no LED pelo modelo linear
        float corrente = 0.0f;
        if (tensao > VF_LED) {
            corrente = (tensao - VF_LED) / R_SERIE;
        }

        // 4) Aciona ou desliga o LED conforme o limiar
        if (corrente >= I_TH) {
            PORTB |= (1 << PORTB0);    // Seta bit 0: LED aceso
        } else {
            PORTB &= ~(1 << PORTB0);   // Limpa bit 0: LED apagado
        }

        // 5) Envia dados ao terminal via UART
        uart_print("ADC = ");
        uart_print_uint(leitura);
        uart_print(" | V = ");
        uart_print_float(tensao);
        uart_print(" V | I = ");
        uart_print_float(corrente * 1000.0f);
        uart_print(" mA\r\n");

        // Intervalo entre leituras
        delay_timer(200);
    }

    return 0;
}

// Definições de bits em cada pino
#define LED_CARRO_VERDE   PB0  // pino 8
#define LED_CARRO_AMARELO PB1  // pino 9
#define LED_CARRO_VERMELHO PB2 // pino 10
#define LED_PED_VERDE     PB3  // pino 11
#define LED_PED_VERMELHO  PB4  // pino 12
#define BOTAO PD2 // pino 2 (INT0)

// Cria uma variável global para sinalizar se o botão foi pressionado
volatile bool solicitacao = false; 

// Cria um conjunto de estados para o semáforo
enum Estado { 
  NORMAL,
  AMARELO,
  VERMELHO_CARROS,
  PEDESTRE,
  RETORNO
};

// Variável para armazenar o estado atual
Estado estado = NORMAL;

// Tempos em milissegundos para cada fase
unsigned long tempoAnterior = 0;
const long tAmarelo = 2000;
const long tPedestre = 5000;
const long tRetorno = 1000;

// Interrupção INT0 para detectar o botão
ISR(INT0_vect) {
  solicitacao = true;
}

void setup() {
  // Configura PB0–PB4 como saída
  DDRB |= (1 << LED_CARRO_VERDE) | (1 << LED_CARRO_AMARELO) | (1 << LED_CARRO_VERMELHO) | (1 << LED_PED_VERDE) | (1 << LED_PED_VERMELHO);

  // Botão (PD2) como entrada
  DDRD &= ~(1 << BOTAO);

  // Ativa pull-up interno
  PORTD |= (1 << BOTAO);

  // Configura interrupção externa INT0 (borda de descida)
  EICRA |= (1 << ISC01); 
  EICRA &= ~(1 << ISC00);

  EIMSK |= (1 << INT0); // habilita INT0

  sei(); // habilita interrupções globais

  estadoNormal(); // inicia com estado normal
}

void loop() {
   // Obtém tempo atual para controle de temporização
  unsigned long agora = millis();

  // Cria um switch para controlar a lógica do semáforo com base no estado atual
  switch (estado) {
    case NORMAL:
    // Se o botão for pressionado, muda para estado amarelo
      if (solicitacao) {
        estado = AMARELO;
        tempoAnterior = agora;
        solicitacao = false;
        luzAmarela();
      }
      break;

    // Muda para vermelho de carros após o tempo do amarelo  
    case AMARELO:
      if (agora - tempoAnterior >= tAmarelo) {
        estado = VERMELHO_CARROS;
        tempoAnterior = agora;
        luzVermelhaCarros();
      }
      break;

    // Muda para pedestre após o tempo do vermelho de carros  
    case VERMELHO_CARROS:
      estado = PEDESTRE;
      tempoAnterior = agora;
      luzPedestre();
      break;

    // Muda para retorno após o tempo do pedestre  
    case PEDESTRE:
      if (agora - tempoAnterior >= tPedestre) {
        estado = RETORNO;
        tempoAnterior = agora;
        retorno();
      }
      break;

    // Volta para estado normal após o tempo do retorno  
    case RETORNO:
      if (agora - tempoAnterior >= tRetorno) {
        estado = NORMAL;
        estadoNormal();
      }
      break;
  }
}

// Função para configurar LEDs no estado normal
void estadoNormal() {
  PORTB |= (1 << LED_CARRO_VERDE); // Carro verde aceso
  PORTB &= ~((1 << LED_CARRO_AMARELO) | (1 << LED_CARRO_VERMELHO)); // Carro amarelo e vermelho apagados
  PORTB &= ~(1 << LED_PED_VERDE); // Pedestre verde apagado
  PORTB |= (1 << LED_PED_VERMELHO); // Pedestre vermelho aceso
}

// Função para configurar LEDs no estado amarelo
void luzAmarela() {
  PORTB &= ~(1 << LED_CARRO_VERDE); // Carro verde apagado
  PORTB |= (1 << LED_CARRO_AMARELO); // Carro amarelo aceso
  PORTB &= ~(1 << LED_CARRO_VERMELHO); // Carro vermelho apagado
}

// Função para configurar LEDs no estado vermelho de carros
void luzVermelhaCarros() {
  PORTB &= ~(1 << LED_CARRO_VERDE); // Carro verde apagado
  PORTB &= ~(1 << LED_CARRO_AMARELO); // Carro amarelo apagado
  PORTB |= (1 << LED_CARRO_VERMELHO); // Carro vermelho aceso
  PORTB |= (1 << LED_PED_VERMELHO); // Pedestre vermelho aceso
}

// Função para configurar LEDs no estado pedestre
void luzPedestre() {
  PORTB |= (1 << LED_PED_VERDE); // Pedestre verde aceso
  PORTB &= ~(1 << LED_PED_VERMELHO); // Pedestre vermelho apagado
}

// Função para configurar LEDs no estado de retorno
void retorno() {
  PORTB &= ~(1 << LED_PED_VERDE); // Pedestre verde apagado
  PORTB |= (1 << LED_PED_VERMELHO); // Pedestre vermelho aceso
}
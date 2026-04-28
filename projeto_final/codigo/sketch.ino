#include <DHT.h>
#include <LiquidCrystal_I2C.h>
//############################FUNÇÕES########################//
// --- DEFINIÇÕES EXAUSTOR ---
#define MOTOR_TEMP 9 // Pino PWM para Exaustor de Temperatura usando led
#define MOTOR_UMID 10 // Pino PWM para Exaustor de Umidade usando led
// --- LOGICA DE CONTROLE PROPORCIONAL VIA PWM ---
void controlarExaustoresPWM(float tempAtual, float setTemp, float umidAtual, float setUmid) {
  int velocidadeTemp = 0;
  if (tempAtual > setTemp) {
    float erro = tempAtual - setTemp;
    velocidadeTemp = erro * 51; 
  }

  int velocidadeUmid = 0;
  if (umidAtual > setUmid) {
    float erroUmid = umidAtual - setUmid;
    velocidadeUmid = erroUmid * 25; 
  }

  velocidadeTemp = constrain(velocidadeTemp, 0, 255);
  velocidadeUmid = constrain(velocidadeUmid, 0, 255);

  analogWrite(MOTOR_TEMP, velocidadeTemp);
  analogWrite(MOTOR_UMID, velocidadeUmid);
}
//###############################################################

// --- DEFINIÇÕES DE SEGURANÇA ---
#define PIN_SENSOR_PORTA 2  // Pino com suporte a Interrupção
#define PIN_SIRENE 11
volatile bool intrusaoDetectada = false; // Variável volátil para a interrupção


// --- DEFINIÇÕES DE HARDWARE ---
#define DHT_PIN 4
#define DHTTYPE DHT22
#define POT_TEMP A0 
#define POT_UMID A1 

const int trigPins[] = {22, 24, 26, 28};
const int echoPins[] = {23, 25, 27, 29};

// --- INSTÂNCIAS ---
DHT dht(DHT_PIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- VARIÁVEIS GLOBAIS ---
unsigned long millisAnteriorDHT = 0;
const long intervaloDHT = 2000;
float distancias[4] = {0, 0, 0, 0};

float temperatura = 0;
float umidade = 0;
int setpointTemp = 0;
int setpointUmid = 0;

// --- FUNÇÃO DE INTERRUPÇÃO (ISR) ---
// Executada instantaneamente quando a porta abre
void trataIntrusao() {
  intrusaoDetectada = true;
}

// --- FUNÇÃO DE CÁLCULO DE DISTÂNCIA ---
float lerDistancia(int i) {
  digitalWrite(trigPins[i], LOW);
  delayMicroseconds(2);
  digitalWrite(trigPins[i], HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPins[i], LOW);

  long duracao = pulseIn(echoPins[i], HIGH, 20000); 
  float d = (duracao * 0.0343) / 2;
  return (d > 0) ? d : 400; 
}

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando Sistema...");
  dht.begin();

  // Configuração de Segurança
  pinMode(PIN_SENSOR_PORTA, INPUT_PULLUP); // Mantém o pino em HIGH; o sensor aterra em LOW
  pinMode(PIN_SIRENE, OUTPUT);
  // Configura interrupção: dispara quando o sinal cai (FALLING) ao abrir a porta
  attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_PORTA), trataIntrusao, FALLING);

  // Configuração do exaustor
  pinMode(MOTOR_TEMP, OUTPUT);
  pinMode(MOTOR_UMID, OUTPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
  }

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SGC2AE ATIVO");
  delay(2000);
  lcd.clear();
}

void loop() {

  // 1. VERIFICAÇÃO DE ALERTA (Prioridade Máxima)
  if (intrusaoDetectada) {
    digitalWrite(PIN_SIRENE, HIGH);
    lcd.setCursor(0, 0);
    lcd.print(" ALERTA INTRUSAO");
    lcd.setCursor(0, 1);
    lcd.print(" PORTA ABERTA!  ");
    Serial.println("!!! ALERTA: ACESSO NAO AUTORIZADO A SALA !!!");
    
    // O sistema só volta ao normal após fechar a porta e o operador "resetar" (opcional)
    if (digitalRead(PIN_SENSOR_PORTA) == HIGH) { // Se a porta for fechada
       digitalWrite(PIN_SIRENE, LOW);
       intrusaoDetectada = false;
       lcd.clear();
    }
  }

  // 2. LOGICA EXISTENTE (DHT, Ultrassônicos, PWM)
  unsigned long millisAtual = millis();

  int leituraA0 = analogRead(POT_TEMP);
  int leituraA1 = analogRead(POT_UMID);

  setpointTemp = map(leituraA0, 0, 1023, 10, 70);
  setpointUmid = map(leituraA1, 0, 1023, 20, 90);

  if (millisAtual - millisAnteriorDHT >= intervaloDHT) {
    millisAnteriorDHT = millisAtual;

    float t_lida = dht.readTemperature();
    float h_lida = dht.readHumidity();
    
    for (int i = 0; i < 4; i++) {
      distancias[i] = lerDistancia(i);
    }

    if (!isnan(t_lida) && !isnan(h_lida)) {
      temperatura = t_lida;
      umidade = h_lida;

      // --- ATUALIZAÇÃO DO LCD ---
      lcd.setCursor(0, 0);
      lcd.print("T:"); lcd.print(temperatura, 1);
      lcd.print(" S:"); lcd.print(setpointTemp); lcd.print("C  ");

      lcd.setCursor(0, 1);
      lcd.print("U:"); lcd.print(umidade, 1);
      lcd.print(" S:"); lcd.print(setpointUmid); lcd.print("%  ");

      // --- CÁLCULO DE VOLUME (SALA 4H x 4L x 12C) ---
      float alturaTeto = 400.0;     // Altura da sala em cm (4 metros)
      float areaQuadrante = 12.0;   // Cada sensor cobre 12m² (Área total 48m²)
      float volumeOcupadoTotal = 0;
      float volMaximoSala = 192.0;  // 4 * 4 * 12 metros cúbicos

      // --- SAÍDA SERIAL COM LÓGICA DE INVENTÁRIO ---
      Serial.print("T:"); Serial.print(temperatura, 1);
      Serial.print(" | U:"); Serial.print(umidade, 1);
      Serial.print(" | OCUPACAO: ");

      for (int i = 0; i < 4; i++) {
        // Altura das caixas = Teto - Distância lida pelo sensor
        float alturaCaixa = alturaTeto - distancias[i];
        if (alturaCaixa < 0) alturaCaixa = 0; // Filtro para leituras erradas

        // Volume do quadrante (Área * Altura em metros)
        float volQuad = areaQuadrante * (alturaCaixa / 100.0);
        volumeOcupadoTotal += volQuad;

        Serial.print("Q"); Serial.print(i + 1); 
        Serial.print(":"); Serial.print(alturaCaixa, 0); Serial.print("cm ");
      }

      float porcentagem = (volumeOcupadoTotal / volMaximoSala) * 100.0;

      Serial.print("| Total: "); Serial.print(volumeOcupadoTotal, 1); Serial.print("m3 ");
      Serial.print("("); Serial.print(porcentagem, 1); Serial.println("%)");

      // --- ACIONAMENTO DOS EXAUSTORES ---
      controlarExaustoresPWM(temperatura, (float)setpointTemp, umidade, (float)setpointUmid);

    } else {
      Serial.println("Erro ao ler o DHT22!");
      lcd.setCursor(0, 0);
      lcd.print("Erro no Sensor");
    }
  }
}
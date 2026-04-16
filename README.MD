# Desenvolvimento de Sistemas Embarcados - AVR Architecture

Este repositório compila as atividades práticas e de pesquisa desenvolvidas na disciplina de Sistemas Embarcados. O foco central reside na programação de baixo nível para o microcontrolador **ATmega328P**, priorizando a manipulação direta de registradores e a gestão de periféricos internos sem o uso de abstrações de alto nível (Arduino API).


## Tecnologias e Ferramentas

* **Linguagens:** * **C (AVR-GCC):** Programação de baixo nível para firmware, com foco em operações bit-a-bit.
    * **LaTeX:** Documentação técnica modularizada.
* **Ambientes de Desenvolvimento (IDE/Simulação):**
    * **Tinkercad:** Ambiente de simulação de circuitos e validação de firmware.
    * **Arduino IDE:** Compilação e carga do código-fonte utilizando a *toolchain* AVR.
    * **Overleaf:** Editor colaborativo para a redação do relatório acadêmico.

## Conceitos de Baixo Nível Aplicados
Conforme as diretrizes da disciplina, é **proibido** o uso de funções da API Arduino (como `digitalWrite` ou `delay`). Foram implementados:
* **Gestão de I/O:** Manipulação de `DDRx`, `PORTx` e `PINx`.
* **Temporização:** Uso de **Timer 1 (16-bit)** via registros `TCCRnA/B` e `TCNT1`.
* **ADC:** Configuração de `ADMUX` e `ADCSRA` para leitura analógica.
* **Interrupções:** Gerenciamento de eventos externos via `EICRA` e `EIMSK`.

## Atividades Implementadas

### Aula 05: Fundamentos de I/O e Lógica
* **Atividade 1 & 2:** Controle de estados de LED (*Blink*) utilizando variáveis de tempo e estruturas condicionais (`if/else`) para alternância entre modo rápido (200ms) e lento (1000ms).
* **Atividade 3 & 4:** Implementação de laços de repetição (`for`) e modularização do código através da criação de funções customizadas (ex: `piscaLed`).

### Aula 06: Periféricos Externos e Semáforos
* **Atividade 1:** Controle de LED externo via função modularizada com parâmetros de frequência e contagem.
* **Atividade 2:** Implementação de um **Controlador de Semáforo** com lógica de estados para LEDs Verde, Amarelo e Vermelho em ciclo contínuo.

### Aula 07: Conversão Analógica-Digital (ADC)
* **Atividade I:** Leitura analógica contínua de um potenciômetro (Pino A0) com classificação de níveis (BAIXO, MÉDIO, ALTO) via Serial Monitor em intervalos de 500ms.
* **Atividade II:** Controle e estimativa de corrente em um LED baseada na tensão lida pelo potenciômetro, utilizando modelos matemáticos para acionamento por limiar.

### Aula 08: Interrupções Externas
* **Atividade I:** Contador visual binário (0 a 7) incrementado por um botão configurado via **interrupção externa** (Pino 2).
* **Atividade II:** Semáforo complexo com **botão de pedestre**, integrando `millis()` e interrupções para gerenciamento de estados de travessia.

### Aula 09 & 10: PWM e Comunicação Serial
* **Controle de Carga:** Uso de PWM para controle de velocidade de motores DC e brilho de LEDs.
* **Interface Serial:** Recebimento e tratamento de comandos via UART para ajuste proporcional de atuadores (0-255) com tratamento de erros.


## Especificações Técnicas
* **Target Hardware:** ATmega328P (8-bit AVR RISC architecture).
* **Clock Frequency:** 16 MHz (External Crystal Oscillator).
* **Toolchain:** AVR-GCC compiler, Tinkercad (Simulation), e Overleaf (LaTeX Documentation).
* **Paradigma de Desenvolvimento:** Bare-metal programming com foco em eficiência de ciclos de clock e gerenciamento de memória Flash/SRAM.

## Conceitos e Periféricos Implementados

### 1. Manipulação de I/O Digital via Registradores
Substituição das funções `pinMode()` e `digitalWrite()` pela configuração direta dos registros de direção e dados:
* **DDRB/DDRD:** Definição de direção de fluxo de dados (Data Direction Registers).
* **PORTB/PORTD:** Controle de estados lógicos e ativação de resistores de pull-up internos.
* **PINB/PIND:** Leitura síncrona de estados de entrada.

### 2. Gestão de Temporização por Hardware (Timers/Counters)
Implementação de bases de tempo precisas utilizando o **Timer 1 (16-bit)** em substituição à biblioteca `<util/delay.h>`, evitando o bloqueio ineficiente da CPU:
* **Prescaling:** Divisão de frequência para resoluções de milissegundos (Prescalers de 64, 256 ou 1024).
* **Modo CTC (Clear Timer on Compare Match):** Utilização do registrador `OCR1A` para estabelecer alvos de contagem determinísticos.

### 3. Conversão Analógica-Digital (ADC)
Manipulação dos registros `ADMUX` e `ADCSRA` para amostragem de sinais analógicos com resolução de 10 bits.

## Estrutura do Repositório
* **`/atividades`**: Códigos-fonte `.ino` comentados, utilizando registradores como `DDRx`, `PORTx`, `ADMUX` e `TCCRnA/B` na implementação das atividades.
* **`/relatorio`**: Centraliza a documentação acadêmica final:
* `relatorio_sistemas_embarcados.pdf`: Versão compilada para leitura direta.
* `projeto_latex.zip`: Código-fonte completo do projeto Overleaf para fins de portabilidade e edição futura.
* **`/figuras`**: Esquemáticos de hardware e diagramas de blocos da arquitetura AVR.

## Como Executar
1. Os códigos presentes em `/atividades` podem ser compilados diretamente na **Arduino IDE** ou importados para o ambiente **Tinkercad**.
2. O relatório técnico detalha a fundamentação teórica de cada escolha de projeto, incluindo tabelas de registradores e análises de desempenho.

---


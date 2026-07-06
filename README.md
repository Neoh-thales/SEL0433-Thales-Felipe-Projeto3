# Projeto 3: Controle de Motores, PWM e Comunicação sem Fio (ESP32)

**Disciplina:** SEL0433 - Aplicação de Microprocessadores
 
**Alunos:**
* Thales Vasconcelos Aguiar de Oliveira | NUSP: 15489730 
* Felipe Assis Bernardes Falvo | NUSP: 15004433

## 🚀 1. Visão Geral
Esse repositório tem como objetivo o desenvolvimento do Projeto 3, no qual é utilizado a ESP32 para o controle de periféricos por meio do PWM. O projeto é dividido em três partes que vão desde o controle de um LED RGB, leitura ADC de um servo, até o desenvolvimento de uma cancela de pedágio com wifi e página web. Toda a validação foi realizada no simulador Wokwi mencionado durante as aulas.

---

## 🔴🟢🔵 Parte 1: Controle de LED RGB via PWM

A primeira parte tem como foco o controle de intensidade luminosa de um LED RGB utilizando o ESP32 para o controle de modulação por largura de pulso (LEDC).

### 🛠️ Arquitetura de Hardware e I/O (Simulador Wokwi)
O sistema foi simulado no Wokwi utilizando o ESP32 com o seguinte mapeamento de periféricos:
* **LED RGB (Catodo Comum):** O pino comum está conectado ao GND.
* **Resistores:** Três resistores de 220 Ω conectados em série com os terminais do LED para proteção.
* **GPIO 19:** Conectado ao terminal Vermelho (R).
* **GPIO 18:** Conectado ao terminal Verde (G).
* **GPIO 5:** Conectado ao terminal Azul (B).

### ⚙️ Firmware e Lógica de Controle
O código (`Parte_1.c`) foi desenvolvido em linguagem C utilizando o framework do Arduino para ESP32.

* **Configuração do PWM (LEDC):** O hardware de controle do LED foi configurado com uma **frequência de 5 kHz** para manter a iluminação constante. A **resolução foi colocado em 8 bits**, permitindo que a intensidade da luz varie em uma escala de 0 a 255. Os pinos 19, 18 e 5 foram colocados aos canais usando a função `ledcAttach()`.
* **Lógica de Variação de Cores:** O código roda em um ciclo, atualizando o brilho (*duty cycle*) de cada cor a cada 500 ms. Foram escolhidos passos diferentes para cada pino, criando um efeito visual dinâmico onde as cores se misturam em tempos diferentes:
  * **Verde:** Aumenta de forma mais lenta, em passos de **5**.
  * **Azul:** Aumenta o dobro mais rápido, em passos de **10**.
  * **Vermelho:** Aumenta de forma mais rápida, em passos de **35**.
* **Limite e Resolução:** O PWM foi configurado com uma resolução de 8 bits, o que significa que a intensidade obedece à regra de $2^8 = 256$ níveis discretos (faixa de 0 a 255). Foi colocado uma estrutura condicional (`if`) para garantir que a soma dos incrementos não passe desse limite. Ao passar de 255 (100%), a variável é zerada, reiniciando o ciclo.
* **Interface Serial:** A cada atualização das luzes, o microcontrolador envia os dados pela porta serial a **115200 bps**. Logo, usando o `Serial.printf`, o sistema mostra na tela qual é o *duty cycle* atual de cada LED e o incremento, podendo assim acompanhar o funcionamento do circuito.

### 📚 Conceitos, Bibliotecas e Código
Nesta etapa fundamental, não foram necessárias bibliotecas externas. Explorou-se o conceito de **PWM (Pulse Width Modulation)**, onde a tensão média entregue ao LED (e consequentemente o brilho) é controlada variando o tempo em que o sinal elétrico permanece em nível alto (Duty Cycle) em uma alta frequência.
O principal trecho de código demonstra a manipulação do registrador de hardware e do controle de fluxo:
```c
  // Atualiza o ciclo de trabalho do Led Vermelho
  duty_vermelho = duty_vermelho + soma_vermelho; 
  if (duty_vermelho > 255) { 
    duty_vermelho = 0; // Prevenção contra overflow da resolução de 8 bits
  }
  ledcWrite(pino_vermelho, duty_vermelho); // Envia o sinal PWM para o GPIO físico
```

### 💻 Como Simular a Parte 1
1. Importe os arquivos `parte1.c` e `diagram.json` (presentes na pasta `Parte_1/` do repositório) para um novo projeto no **Wokwi**.
2. Inicie a simulação apertando o botão de Play.
3. Observe a aba **Serial Monitor** para visualizar o envio constante das taxas de PWM para cada cor.
4. Observe a resposta visual do LED no diagrama.

### 📸 Circuito e Demonstração
**Esquemático de circuito:**

<div align="center">
  <img width="783" height="670" alt="image" src="https://github.com/user-attachments/assets/8c920b01-0211-42f7-abe5-c95d1a4fbb10" />
</div>


**Funcionamento em Tempo Real:**

<div align="center">
  <img src="https://github.com/user-attachments/assets/f27772c8-204e-4d96-8569-e38d381a4aa7" width="70%" alt="Funcionamento Parte 1" />
</div>

---

## 🦾 Parte 2.1: Controle de Servomotor via ADC

A segunda parte do projeto tem como foco a manipulação de um servo motor utilizando variação de *duty cycle*, configurando a ESP32 para interpretar sinais analógicos de um potenciômetro.

### 🛠️ Arquitetura de Hardware e I/O
O circuito foi implementado no simulador Wokwi utilizando:
* **Potenciômetro:** Atuando como um divisor de tensão, alimentado por 3.3V.
* **Servomotor:** Alimentado com 5V e referenciado ao GND comum.
* **Pinos de Interface (GPIOs):**
  * **GPIO 35 (Entrada - ADC):** Conectado ao terminal do potenciômetro para leitura analógica.
  * **GPIO 21 (Saída - PWM):** Conectado ao pino de controle do servomotor para o envio dos pulsos PWM.

### ⚙️ Firmware e Lógica de Controle
A lógica desenvolvida no código `Parte_2_1.c` explora a biblioteca `ESP32Servo` para abstrair os complexos registradores de hardware e facilitar a modulação do motor.

* **Alocação de Temporizadores (Timers):** Para garantir o controle do sinal PWM enviado ao servomotor, o sistema aloca quatro Timers (0 a 3) no início da função `setup()`. Essa alocação, realizada através do comando `ESP32PWM::allocateTimer()`, garante que a modulação do motor ocorra de forma dedicada.
* **Configuração Base do Servo:** A frequência central do pino PWM é cravada em **50 Hz**, que é o padrão para servo. Os limites de pulso (*duty cycle*) foram feitos na função `attach()` limitando no intervalo de **500 μs** (ângulo 0°) a **2500 μs** (ângulo 180°).
* **Leitura Analógica e Conversão (ADC):** No `loop()`, o pino analógico 35 amostra constantemente a queda de tensão resultante do potenciômetro. O conversor A/D da ESP32 possui **12 bits** de resolução linear, gerando valores de `0` até `4095`.
* **Mapeamento Angular:** Para diminuir o processamento, a conversão do valor lido pelo ADC é feito pela função `map()`, no qual ela projeta a escala de `0 a 4095` diretamente para o intervalo de `0 a 180` graus do servo.
* **Cálculo de Duty Cycle:** Por razões de interface, a porcentagem da modulação é feita convertendo o valor em um decimal com a fórmula `(ADC / 4095.0) * 100.0`.
* **Telemetria Serial (UART):** Mantendo o padrão, o sistema mostra os dados a cada 200 ms na comunicação serial (`115200 bps`). O texto printa os três dados: o valor do ADC, o ângulo imposto no servo e a porcentagem da modulação (*duty cycle*).

### 📚 Conceitos, Bibliotecas e Código
Nesta etapa, o conceito principal abordado foi a **Conversão Analógico-Digital (ADC)** e sua aplicação em um sistema de malha aberta. A biblioteca **`ESP32Servo.h`** foi empregada para abstrair a geração do PWM e simplificar a calibração de microsegundos do braço robótico sem criar conflitos internos nos registradores da arquitetura Xtensa.
O trecho de destaque ilustra a extração matemática da escala de 12 bits para os graus do mundo físico:
```c
  // Leitura do ADC em 12 bits (Valores variando de 0 a 4095)
  int leitura_potenciometro = analogRead(pino_potenciometro);

  // Mapeamento proporcional nativo de resolução para graus mecânicos (0 a 180)
  angulo_servo = map(leitura_potenciometro, 0, 4095, 0, 180);

  // Envia a instrução física para o servo
  servoMotor.write(angulo_servo);
```

### 💻 Como Simular a Parte 2.1
1. Importe os arquivos `parte_2_1.c` e `diagram.json` (presentes na pasta `Parte_2/Exercicio1/` do repositório) para um novo arquivo no **Wokwi**.
2. **Importante:** No Wokwi, acesse a aba **Library Manager** e adicione a biblioteca `ESP32Servo` para que o código seja compilado.
3. Aperte o botão de iniciar (Play).
4. Na interface gráfica, clique com o cursor sobre o componente do **Potenciômetro** e deslize.
5. Observe que o **Servo** irá girar proporcionalmente com a tensão.
6. Verifique os valores pela aba **Serial Monitor**.

### 📸 Circuito e Demonstração
**Esquemático de circuito:**

<div align="center">
  <img width="1113" height="683" alt="image" src="https://github.com/user-attachments/assets/df9ae018-f91b-42ec-bbe9-5345300c9007" />
</div>


**Funcionamento em Tempo Real:**

<div align="center">
  <img src="https://github.com/user-attachments/assets/79b4928b-b061-4832-8fdc-f676c4f533f4" width="70%" alt="Funcionamento Parte 2.1" />
</div>

**Teste no Ângulo Máximo (180°):**

<div align="center">
  <img src="https://github.com/user-attachments/assets/b2a49d0c-0cf7-4ec7-9990-26b9ba8517b6" width="70%" alt="Funcionamento Parte 2.1 - Angulo Máximo" />
</div>

---

## 🚧 Parte 2.2: Cancela de Pedágio com Web Server

A etapa final do projeto eleva a complexidade integrando o controle PWM de servomotores à leitura de sensores ultrassônicos e telemetria Wi-Fi. O objetivo é simular uma cancela de pedágio automatizada, responsiva e conectada.

### 🛠️ Arquitetura de Hardware e I/O
O circuito foi modelado no Wokwi utilizando o seguinte mapeamento:
* **Servomotor Padrão:** Representa o braço da cancela. Controlado pelo pino PWM `GPIO 13` e alimentado com 5V.
* **Sensor Ultrassônico (HC-SR04):** Simula o radar de detecção veicular do pedágio.
  * `TRIG` (Emissor de pulso) conectado à `GPIO 5`.
  * `ECHO` (Receptor de eco) conectado à `GPIO 18`.
* **Display OLED (SSD1306):** Atua como o painel de operação local via barramento I2C (`SDA = 21`, `SCL = 22`).
* **Conectividade Wi-Fi:** Módulo interno da ESP32 conectado à rede virtual `Wokwi-GUEST`.

### ⚙️ Firmware e Lógica de Estados
O software principal em `Parte_2_2.c` baseia-se em um fluxo não-bloqueante orientado a Máquina de Estados e comunicação Web.

* **Subsistemas:** A ESP32 gerencia o Wi-Fi, a interface I2C (OLED) e a geração de PWM (Servo). Para suportar isso no processador, foram alocados 4 Timers.
* **Leitura do ultrassom:** A função `medirDistancia()` aciona o pino `TRIG` e calcula a distância baseada na velocidade do som. Para evitar travar o processamento caso o pulso se perca no vazio, a instrução `pulseIn()` foi limitada com um *timeout* de 30.000 μs.
* **Lógica de Estados:** O ciclo `loop()` é dividido em dois estados:
  * **Estado 1 (Cancela Fechada):** Se o sensor detectar um veículo a menos de 200 cm (2 metros), o servo recebe a instrução para o ângulo de 90° (abertura). Uma *flag* altera o estado lógico e o momento do disparo (em milissegundos via `millis()`) é armazenado na variável `tempo_abertura`.
  * **Estado 2 (Cancela Aberta):** O sistema exige que **duas** condições sejam validadas para retornar a cancela à estaca zero (0°): (a) a região de passagem deve estar livre (distância > 200cm) **E** (b) o tempo de segurança de 3 segundos (`tempo_espera_fechar`) deve ter acabadp.
* **Conectividade Wi-Fi:** A ESP32 é programada para se conectar à rede de internet do simulador (`Wokwi-GUEST`). Ao ligar (dentro do `setup()`), a placa aciona e aguarda até receber permissão e ganhar um endereço IP. Assim que a conexão é feita com sucesso, esse número IP (como `10.10.0.2`) aparece na tela do OLED para que o usuário saiba qual endereço digitar no navegador.
* **IoT Web Server (Site da Cancela):** O que deveria acontecer na placa física (Limitação do simulador): a placa ESP32 atua como um mini servidor de internet. O comando `server.handleClient()` fica verificando para ver se alguém está tentando acessar o IP dela. Quando acessada, ela monta uma página HTML com as informações (cancela, distância e carros) e envia para o usuário. **Isso não funciona no Wokwi** porque o Wi-Fi do Wokwi e o IP gerado (`10.10.0.2`) são virtuais e fechados dentro do simulador. Esse IP não existe na rede real da sua casa. Portanto, se você tentar abrir o navegador do seu PC e digitar esse endereço, a conexão vai falhar. Para ver a página web rodando de verdade, seria necessário gravar esse mesmo código em uma ESP32 conectada ao roteador real.

### 📚 Conceitos, Bibliotecas e Código
O projeto utilizou conceitos de **Sistemas Concorrentes** (Servidor Web e Controle Físico rodando juntos), **Lógica de estados** e **Timeouts** para evitar processos bloqueantes no Loop Principal. Foram empregadas as seguintes bibliotecas:
* **`<WiFi.h>` e `<WebServer.h>`:** Conjunto responsável pela internet. A `WiFi.h` conecta a placa no Wi-Fi e pega o endereço IP. Já a `WebServer.h` cria o site (servidor HTTP) dentro da placa e montando a página HTML em tempo real com as informações da cancela.
* **`<Wire.h>`, `<Adafruit_GFX.h>` e `<Adafruit_SSD1306.h>`:** Conjunto responsável pelo display OLED. A `Wire.h` faz a comunicação pelos pinos SDA e SCL. As bibliotecas da Adafruit funcionam como um painel, permitindo escrever e organizar os textos na tela usando o `display.display()`.
* **`<ESP32Servo.h>`:** Biblioteca dedicada para controlar o braço da cancela. Ela transforma ângulos (0 a 180 graus) em pulsos elétricos. Para garantir que o motor não trave enquanto o Wi-Fi funciona, ela usa temporizadores de hardware para o sinal do motor.

O trecho de destaque ilustra a lógica de estados:
```c
  // Estado 1 - Cancela Fechada:
  if (!cancela_aberta) {
    if (distancia_carro > 0 && distancia_carro < distancia_carro_min) {
      moverCancela(90); // Gira o servo para 90 graus (abre)
      cancela_aberta = true;
      tempo_abertura = millis(); // Congela o tempo exato da abertura
    }
  } else {
    // Estado 2 - Cancela Aberta:
    // Só fecha após carro sair da distância crítica E passar o tempo de segurança
    if (distancia_carro > distancia_carro_min && (millis() - tempo_abertura > tempo_espera_fechar)) {
      moverCancela(0);
      cancela_aberta = false;
    }
  }
```

### ⚠️ Apêndice Técnico: Justificativa para a não utilização da driver/mcpwm.h
Embora o projeto mencionasse o uso da biblioteca nativa `driver/mcpwm.h`, foi visto uma **limitação no motor gráfico do simulador Wokwi**. O simulador apresenta um bug onde os sinais PWM gerados pelo MCPWM da ESP32 falham em acionar visualmente o servo do componente `Wokwi-Servo`, resultando em um hardware parado na tela.

Para resolver isso problema tem-se a seguinte ideia:
1. **Adaptação para Simulação:** O código foi migrado para a biblioteca `ESP32Servo`. Isso permitiu que a simulação visual da cancela funcionasse perfeitamente na tela.
2. **Desenvolvimento Teórico Mantido:** Para não perder a nota do requisito original, o final do arquivo `Parte_2_2.c` contém toda a lógica estrutural (como `mcpwm_config_t` e a função `MCPWM_driver()`) programada e comentada, provando o entendimento de como ela seria aplicada em uma placa real.

### 💻 Como Simular a Parte 2.2
1. Importe os arquivos `parte_2.2.c` e `diagram.json` (presentes na pasta `Parte_2/Exercicio2/`) para o **Wokwi**.
2. **Importante:** Acesse o **Library Manager** na interface lateral (ou o arquivo `libraries.txt`) e adicione manualmente: `ESP32Servo`, `Adafruit GFX Library`, e `Adafruit SSD1306`.
3. Inicie a simulação clicando no botão Play.
4. O Display OLED irá ligar, se conectar à rede Wi-Fi virtual e exibir o Endereço IP gerado (ex: `10.10.0.2`).
5. Aproxime a leitura do sensor de ultrassom para menos de 200 cm clicando no sensor e alterando a distância. A cancela se abrirá e o display mudará de "FECHADA" para "ABERTA".

### 📸 Circuito e Demonstração
**Esquemático de circuito:**

<div align="center">
  <img width="1448" height="703" alt="image" src="https://github.com/user-attachments/assets/87fd00ad-1ddc-4e3b-a932-81e710791e3c" />
</div>


**Funcionamento Padrão (Sem Trânsito e Wi-Fi):**

<div align="center">
  <img width="1452" height="798" alt="image" src="https://github.com/user-attachments/assets/f4a869db-59e1-4c5f-a9f4-6d8c4f60f5d0" />
</div>

**Funcionamento com Veículo Detectado (Cancela Aberta):**

<div align="center">
  <img width="1452" height="798" alt="image" src="https://github.com/user-attachments/assets/b3e4e780-4d15-47f2-b95b-20114dbc315d" />
</div>

**Foco no Display OLED (Tempo Real):**

<div align="center">
  <img width="1452" height="798" alt="image" src="https://github.com/user-attachments/assets/2f1ade69-7f9b-4d67-8b2d-c06ddb8ce740" />
</div>

---

## 📊 2. Discussão Geral dos Resultados
O desenvolvimento desse projeto mostrou a vantagem da ESP32 em resolver problemas complexos. Na **Parte 1**, o controle nativo do PWM permitiu misturar as cores do LED RGB suavemente, sem travar ou sobrecarregar a placa. Na **Parte 2.1**, aplicamos conceitos básicos para ler o potenciômetro e, através de uma conversão (`map`), transformar essa tensão no ângulo do braço do servo motor.

Os maiores desafios apareceram na **Parte 2.2**. O grande problema que precisávamos evitar era fazer com que o processamento do Wi-Fi ou a leitura do sensor ultrassônico não "roubassem" o tempo do motor, o que faria a cancela engasgar. Nós resolvemos isso de duas formas: usando **Timers dedicados de hardware** (que cuidam do motor de forma independente) e montando uma **Lógica de Estados** inteligente. Essa lógica abandonou o uso do comando `delay()` e passou a usar o `millis()`, que funciona como um cronômetro contínuo que não atrapalha o resto do sistema.

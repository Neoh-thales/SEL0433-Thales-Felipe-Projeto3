// Alunos:
// Felipe Assis Bernardes Falvo - NUSP: 15004433
// Thales Vasconcelos Aguiar de Oliveira - NUSP: 15489730

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h> // biblioteca para o controle de servos no ESP32
// #include "driver/mcpwm.h" ->  Wokwi não suporta

// Simular a cancela dos pedágios

// pinos - servo e ultrasom
const int pino_servo = 13; // pino para Servo
const int pino_trigger_ultrasom = 5;
const int pino_eco_ultrasom = 18;

// Definições do display OLED
const int screenWidth = 128;
const int screenHeight = 64;

// Inicialização do display OLED
Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire, -1);
// cria um objeto display da Adafruit_SSD1306
// e inicializa com os parâmetros definidos

// Conectando ao Wifi
const char* ssid = "Wokwi-GUEST"; 
const char* password = "";
WebServer server(80);

Servo servo_pedagio; // objeto da classe Servo

// variáveis de controle da cancela do pedágio:
int distancia_carro = 0;
int qtd_carros = 0;
bool cancela_aberta = false;
unsigned long tempo_abertura = 0;

// Distância mínima para abrir a cancela
const int distancia_carro_min = 200; // abre se estiver menos de 20cm
const int tempo_espera_fechar = 3000; // Espera 3 segundos para fechar a cancela

// converter para angulo para o servo (0 a 180 graus) 
void moverCancela(int angulo) {
  servo_pedagio.write(angulo);
}

// lógica do sensor de ultrasom
int medirDistancia() {
  // limpa o pino o trigger
  digitalWrite(pino_trigger_ultrasom, LOW);
  delayMicroseconds(2);
  
  // manda o pulso de 10 microssegundos
  digitalWrite(pino_trigger_ultrasom, HIGH); // envia 
  delayMicroseconds(10);
  digitalWrite(pino_trigger_ultrasom, LOW); // para
  
  // pino echo liga quando som é emitido
  // volta para desligado quando o eco retorna
  // função pulseIn mede esse tempo em microssegundos
  // O limite de 30.000us evita ficar travando
  long duracao = pulseIn(pino_eco_ultrasom, HIGH, 30000); 
  
  // Se a duração for 0, significa que o som se perdeu 
  // logo carro não está no alcance
  if (duracao == 0) {
    return 400; // envia 400cm (limite - fora do alcance do pedágio)
  }
  
  // cálculo distância:
  // Como a velocidade do som no ar é 343 m/s (ou 0,0343 cm/us).
  // fórmula é: distância =tempo*velocidade.
  // Como o som precisa ir até o carro e voltar (ida e volta), divide-se por 2.
  return duracao * 0.034 / 2;
}

// pagina web 
// funciona como o back-end do servidor
void paginaWeb() {

  // back-end:
  // Se cancela_aberta=true --> status: "CANCELA ABERTA"
  // Se cancela_aberta=false --> status: "CANCELA FECHADA"
  String status = cancela_aberta ? "CANCELA ABERTA" : "CANCELA FECHADA";
  
  // front-end:
  String html = "<h1>Sistema de Pedagio</h1>";
  html += "<p>Status: <b>" + status + "</b></p>"; // mostra se está aberto ou fechado
  html += "<p>Leitura do radar: <b>" + String(distancia_carro) + " cm</b></p>"; // distância do carro
  html += "<p>Carros atendidos: <b>" + String(qtd_carros) + "</b></p>"; // quantidade de carros que passou
  
  // resposta HTTP enviando o pacote HTML
  server.send(200, "text/html", html);
}

void setup() {
  // inicia a interface serial
  Serial.begin(115200);

  // Configurando os pinos do sensor de ultrassom
  pinMode(pino_trigger_ultrasom, OUTPUT); // pino que envia o pulso de som
  pinMode(pino_eco_ultrasom, INPUT); // pino que recebe o som

  // barramento I2C nos pinos da ESP32 (SDA = 21, SCL = 22) - OLED
  Wire.begin(21, 22);

  // configuração do OLED:
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Configurações do Wifi:
  Serial.println("Ligando o Wifi:");
  WiFi.begin(ssid, password);
  
  // loop para o roteador fornecer um IP
  while (WiFi.status() != WL_CONNECTED) {
    // Aguarda a conexão Wi-Fi ser estabelecida
    delay(1000);
  }
  Serial.println("\nWiFi Conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());


  // Configuração do web:
  // quando acessar a raiz (/) do IP no navegador,
  // o servidor vai para a função 'paginaWeb'.
  server.on("/", paginaWeb);
  server.begin(); // serviço que ficará rodando em segundo plano

  // Configuração do servo:
  // alocação manualmente os 4 Timers de hardware (0 a 3).
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // Configura o tempo para 50 Hz
  servo_pedagio.setPeriodHertz(50);
  // liga o pino do servo e define os limites do pulso
  // Nesse caso o mínimo: 500us e o máximo :2500us )
  servo_pedagio.attach(pino_servo, 500, 2500);
  
  // faz a cancela nascer fechada
  moverCancela(0); 
  delay(500); // meio segundo de delay
}


void loop() {
  // verifica a cada ciclo se algum navegador (celular/PC) 
  // solicitou a página HTML e envia os dados
  server.handleClient(); 

  // atualiza a variável com a distância constantemente
  distancia_carro = medirDistancia();

  // Lógica principal de controle da cancela --> dois estados
  
  // Estado 1 - Cancela fechada:
  if (!cancela_aberta) {
    // Aguarda a detecção de um veículo
    if (distancia_carro > 0 && distancia_carro < distancia_carro_min) {
      Serial.println("Veiculo Proximo...");
      moverCancela(90); // Gira o servo 90 graus (aberto)
      cancela_aberta = true; // muda para o estado de aberto
      tempo_abertura = millis(); // Salva o tempo exato em que a cancela abriu
      qtd_carros = qtd_carros + 1; // soma 1 na quantidade carros
    }
  } else {
    // Estado 2 - Cancela aberta:
    // Verifica duas condições:
    // 1- veículo saiu da região (distancia > distancia minima)
    // 2- passou o tempo de segurança (tempo_espera_fechar).
    if (distancia_carro > distancia_carro_min && (millis() - tempo_abertura > tempo_espera_fechar)) {
      Serial.println("Transito liberado...");
      moverCancela(0); // Gira o servo 0 graus (fechado)
      cancela_aberta = false; // muda para o estado para fechado
    }
  }

  // Atualização do OLED:
  display.clearDisplay(); // Limpa o buffer de vídeo anterior
  
  // Configuração para mostrar os dados
  display.setCursor(0,0);
  display.setTextSize(1); // Fonte pequena padrão
  display.println("     PEDAGIO");
  display.println("---------------------");
  
  display.printf("Radar: %d cm\n", distancia_carro);
  display.printf("Carros: %d\n\n", qtd_carros);
  
  display.setTextSize(2);
  if (cancela_aberta) {
    display.println(" ABERTA ");
  } else {
    display.println(" FECHADA");
  }
  
  // mostrando o IP no OLED
  display.setTextSize(1);
  display.printf("IP: %s\n", WiFi.localIP().toString().c_str());
  
  // envia para o OLED
  display.display();

  // delay 100ms
  delay(100); 
}


// Código em mcpwm:
// Nota-se que a biblioteca "driver/mcpwm.h" não funciona mais no wokwi.
// O simulador tem um bug onde o periférico MCPWM da ESP32 não move o servo.
// 
// Se o driver do mcpwm funcionasse, deveria ser feito as seguintes coisas no código:

// 1- A biblioteca <ESP32Servo.h> e o objeto 'servo_pedagio' seriam apagados
// 2- Os 4 comandos 'ESP32PWM::allocateTimer' e o 'servo_pedagio.attach' seriam 
//  substituídos pela função MCPWM_driver() feita em baixo
// 3- O comando 'moverCancela' usaria a função 'moverCancelaMCPWM'
// 
// void MCPWM_driver() {
//   mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, pino_servo);
//   mcpwm_config_t pwm_config;
//   pwm_config.frequency = 50; 
//   pwm_config.cmpr_a = 0; 
//   pwm_config.cmpr_b = 0;
//   pwm_config.counter_mode = MCPWM_UP_COUNTER;
//   pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
//   mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); 
// }
// 
// void moverCancelaMCPWM(int angulo) {
//   float porcentagem_duty = 2.5 + ((angulo / 180.0) * 10.0);
//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, porcentagem_duty);
// }

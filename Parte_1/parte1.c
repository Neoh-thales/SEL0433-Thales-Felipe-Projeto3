#include <Arduino.h>
// Exportar a biblioteca arduino

// variáveis:

// O quanto vai somar para cada cor:
int soma_vermelho = 35;
int soma_verde = 5;
int soma_azul = 10;

// duty cicle:
int duty_vermelho = 0; 
int duty_verde = 0;
int duty_azul = 0;

// pinos para o LED RGB
const int pino_vermelho= 19;
const int pino_verde= 18;
const int pino_azul= 5;

// frequência
const int frequencia = 5000;

// resolução de 8 bits (2^8=256) -> 0 a 255
const int resolucao = 8;

void setup() {
  Serial.begin(115200);

  // configuração dos pinos
  ledcAttach(pino_vermelho, frequencia, resolucao);
  ledcAttach(pino_verde, frequencia, resolucao);
  ledcAttach(pino_azul, frequencia, resolucao);
}

void loop() {
  // Led vermelho
  duty_vermelho = duty_vermelho +soma_vermelho; // Aumenta
  if (duty_vermelho > 255) { // verifica o limite
    duty_vermelho = 0;}
  ledcWrite(pino_vermelho, duty_vermelho); // liga o pino


  // Led verde
  duty_verde = duty_verde + soma_verde; // Aumenta
  if (duty_verde > 255) { // verifica o limite
    duty_verde = 0;}
  ledcWrite(pino_verde, duty_verde); // liga o pino


  // Led azul
  duty_azul = duty_azul + soma_azul; // Aumenta
  if (duty_azul > 255) { // verifica o limite
    duty_azul = 0;}
  ledcWrite(pino_azul, duty_azul); // liga o pino


  // mostra as informações na interface serial UART
  Serial.printf("Duty cicle para cada um:\n Vermelho: %d (+%d)\n Verde: %d (+%d)\n Azul: %d (+%d)\n\n", duty_vermelho, soma_vermelho, duty_verde, soma_verde, duty_azul, soma_azul);
  
  // delay para ver as mudanças
  delay(500);
}

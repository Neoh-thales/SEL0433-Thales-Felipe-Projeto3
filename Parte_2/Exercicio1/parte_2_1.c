// Alunos:
// Felipe Assis Bernardes Falvo - NUSP: 15004433
// Thales Vasconcelos Aguiar de Oliveira - NUSP: 15489730

#include <Arduino.h>
#include <ESP32Servo.h> // biblioteca para o controle de servos no ESP32

// Colocar biblioteca ESP32Servo no Library Manager

Servo servoMotor; // objeto da classe Servo chamado servoMotor para controlar o servo

// pinos:
int pino_servo = 21;// servo
int pino_potenciometro = 35;// potenciometro

// configurações do servo
int angulo_servo = 0; // angulo do servo
float duty_cycle_servo = 0; // inicializar o duty_cicle

void setup() {
  // inicia a interface serial
  Serial.begin(115200); 
  
  // alocação na ESP para os timers do PWM
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Configura a base de tempo para o motor de 50 Hz
  servoMotor.setPeriodHertz(50);

  // liga o pino do servo e define os limites do pulso
  // Nesse caso o mínimo: 500us e o máximo :2500us )
  servoMotor.attach(pino_servo,500,2500);
}

// controlar o servo com o potenciometro
void loop () {
  
  // leitura do valor no potenciometro 
  // Conversor AC tem 12 bits (0 -> 4095)
  int leitura_potenciometro = analogRead(pino_potenciometro);

  // converter o duty cycle em porcentagem (0 a 100%)
  duty_cycle_servo =(leitura_potenciometro /4095.0)*100.0;

  //Converte o valor do potênciometro feito acima 
  //para angulo do motor, que vai de 0 a 180 graus
  angulo_servo = map(leitura_potenciometro, 0, 4095, 0, 180);

  // Envia o angulo para o servo
  servoMotor.write(angulo_servo);
  
  // Mostra as informacoes na interface UART
  Serial.printf("Informações do Servo:\n Valor ADC: %d\n Angulo do Servo: %d graus\n Duty Cycle: %.1f %%\n\n", leitura_potenciometro, angulo_servo, duty_cycle_servo);

  // delay 200ms para mudar o angulo
  delay(200);
}

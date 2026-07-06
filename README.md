# Projeto 3: Controle de Motores, PWM e Comunicação sem Fio (SEL0433)
*Disciplina:* SEL0433 - Aplicação de Microprocessadores
*Professor:* Pedro Oliveira
*Aluno:* Thales Vasconcelos Aguiar de Oliveira *N.USP:* 15489730
*Aluno:* Felipe Assis Bernando Falvo *N.USP:*

Este repositório contém as soluções desenvolvidas para as três etapas práticas do Projeto 3, envolvendo controle PWM de LEDs, acionamento de Servomotores por ADC e um projeto independente simulado no Wokwi utilizando a biblioteca nativa MCPWM e conectividade Wi-Fi.

## Estrutura do Projeto

*   /Parte_1/: Controle de LED RGB utilizando os periféricos nativos do Core 3.0 (ledcAttach, ledcWrite).
*   /Parte_2_1/: Controle de um Servomotor mapeado através das leituras ADC (12 bits) de um Potenciômetro.
*   /Parte_2_2/: *Projeto Livre: Cancela Inteligente de Pedágio (Web Server + MCPWM + Ultrassônico).*

---

## Destaque: Parte 2.2 - Cancela Inteligente

Para o projeto avançado, foi desenvolvida uma Cancela de Pedágio Inteligente utilizando Máquina de Estados e Telemetria Remota.

### Hardware Utilizado
*   *ESP32 DevKit V4*
*   *Sensor Ultrassônico (HC-SR04):* Lê a aproximação de veículos.
*   *Servo Motor (Atuador):* Simula o braço mecânico da cancela.
*   *Display OLED SSD1306 (I2C):* Atua como IHM (Interface Homem-Máquina) local.

### Funcionalidades e Requisitos Atendidos
1.  *Lógica de Controle:* Se o radar detectar distância < 20cm, a cancela abre (Servo 90º) e o status do OLED altera para "ABERTA". Após o veículo passar, um timer de segurança (3000ms) garante que a cancela não feche sobre o veículo.
2.  *Uso de MCPWM (Rubrica):* O código original foi desenvolvido fazendo uso estrito dos registradores driver/mcpwm.h nativos da Espressif (mcpwm_init, mcpwm_set_duty). Devido a limitações oficiais do simulador Wokwi (que não suporta o periférico MCPWM virtualmente), a simulação utiliza o fallback da ESP32Servo, mantendo a implementação MCPWM devidamente documentada no código como prova de conceito.
3.  *Recurso Adicional (Telemetria Wi-Fi):* A ESP32 opera em modo STA e hospeda um *Web Server* na porta 80. Operadores poderiam acessar o IP da placa pelo navegador para visualizar a telemetria ao vivo (status da cancela e contagem diária de veículos).

### Nota sobre a Simulação da Telemetria (Web Server)
O código implementa de forma completa as rotas HTML e o gerenciamento de clientes (server.handleClient()). Contudo, testar a renderização da página HTML diretamente no simulador web (Wokwi.com) possui uma limitação técnica:
*   A plataforma bloqueia o acesso via Wokwi Private IoT Gateway para contas gratuitas (necessita assinatura Wokwi Club), conforme a evidência abaixo:

![Bloqueio de acesso Wokwi Club](erro_wokwi.png)

*   Na prática, o professor pode avaliar a funcionalidade atestando que o IP gerado (10.10.0.2 via Wokwi-GUEST) aparece com sucesso no display OLED e analisando a rotina de C++ (paginaWeb) que monta e envia o pacote HTML. 
*   *Em um ambiente físico real:* Bastaria conectar a ESP32 no roteador Wi-Fi local e digitar o IP atribuído diretamente em um smartphone conectado à mesma rede para visualizar a IHM administrativa.

### Como Simular (Wokwi)
1. Importe os arquivos .c, diagram.json e libraries.txt.
2. Dê Play na simulação.
3. Interaja com o "slider" do Sensor Ultrassônico HC-SR04 para acionar a cancela.
4. (Opcional) Se for assinante Wokwi Premium, ative o *Wokwi IoT Gateway* via Command Palette (F1) para expor a porta 80 da simulação e acessar a página web administrativa pelo navegador host (localhost:8080).

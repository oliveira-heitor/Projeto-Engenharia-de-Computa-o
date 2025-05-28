 
//#include "BluetoothSerial.h"            // Biblioteca para Bluetooth Serial
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "ThingSpeak.h"
#include <WiFi.h>


#define I2C_SDA 21
#define I2C_SCL 22


LiquidCrystal_I2C lcd(0x27, 16, 2);     // Endereço I2C do LCD: 0x27, 16 colunas, 2 linhas


// Variáveis para controle do tempo
unsigned long lastTime = 0; // Tempo da última execução
const unsigned long interval = 30000; // Intervalo de 30 segundos (em milissegundos)

// Configurações do ThingSpeak
const char* apiKey = "0JF78UX8ZCGYTKSW";      // Substitua pela API Key do seu canal ThingSpeak
const long channelID = 2917601;               // Substitua pelo ID do seu canal ThingSpeak

const char* ssid = "SMG";                   // Roteador do seu celular
const char* password = "12102020";            // Senha do seu roteador

WiFiClient client; // Wifi simples


int led = 2;
int n; 
int cont;                                 // Variável inteira - Global
boolean pisca = false;


const int numReadings = 20;
float CO2Readings[numReadings] = {0};
int readIndex = 0;
float CO2Total = 0;



void setup() {
     pinMode(led, OUTPUT);                // Configura pino 2  como saída
     Serial.begin(115200);              // Configura porta serial com velocidade de 115200 bps
     Serial.println(" IMT 2025 - Eng. Computação");


     Wire.begin(I2C_SDA, I2C_SCL); // Inicializa a comunicação I2C com os pinos definidos
     lcd.begin(16, 2); // Inicializa o LCD com 16 colunas e 2 linhas
     lcd.backlight();  // Liga a luz de fundo
     lcd.setCursor(0, 0);
     lcd.print("IMT 2025");
     lcd.setCursor(0, 1);
     lcd.print("Lab 7 grupo X ");
     delay(1000);
     lcd.clear();
  



    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    cont = 0;
    Serial.print("Setup - Iniciando a conexão WiFi");
    while (WiFi.status() != WL_CONNECTED) 
    {
      pisca = !pisca;
      digitalWrite(led, pisca); // Led Azul

      Serial.print("Setup - Tentando Conexão Wifi: "); // Depuração
      cont = cont + 1;                                 // Depuração
      Serial.println(cont);                            // Depuração

      lcd.clear();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Conectando ");
      lcd.setCursor(0, 1);
      lcd.print("Tentativa: ");
      lcd.setCursor(12, 1);
      lcd.print(cont);
      delay(1000);

    }

    WiFi.setAutoReconnect(true); // Habilitar reconexão automática
    WiFi.persistent(true);       // Salvar configuração na memória
    Serial.println();
    Serial.println("WiFi conectado. ");
    Serial.print("WiFi SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP Modulo: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Conectado");
    lcd.setCursor(0, 1);
    lcd.print("IP");
    lcd.setCursor(3, 1);
    lcd.print(WiFi.localIP());
    delay(3000);
    digitalWrite(led, LOW);

    // Inicializa o ThingSpeak
    ThingSpeak.begin(client);

}



void loop() {
  unsigned long currentTime = millis();
  digitalWrite(led, HIGH);             // Nivel lógico alto no pino I/O 2  
  delay(100);                          // Atraso de 100 ms 
  digitalWrite(led, LOW);              // Nivel lógico baixo no pino I/O 2  
  delay(100);                          // Atraso de 100 ms 
 
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Lab 6_1: ");
  lcd.setCursor(0, 1);
  lcd.print("Sensor MQ4");


  //int leitura = analogRead(34);         // Lendo no pino GPIO34 
  float CO2 = analogRead(34);

  CO2Total -= CO2Readings[readIndex];
  CO2Readings[readIndex] = CO2;
  CO2Total += CO2;
  readIndex = (readIndex + 1) % numReadings;  // Atualizar o índice
  float CO2Media = CO2Total / numReadings;


  Serial.print("Leitura Sensor: ");
  Serial.print(CO2Media);
  Serial.println(" ");



  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("CH4: ");
  lcd.setCursor(0, 1);
  lcd.print(CO2Media);


    // Verifica se o intervalo de 30 segundos passou
    if (currentTime - lastTime >= interval) {
        lastTime = currentTime; // Atualiza o tempo da última execução


  ThingSpeak.setField(1, CO2Media);


  int status = ThingSpeak.writeFields(channelID, apiKey);
  //int status = ThingSpeak.writeField(channelID, 1, dB, apiKey);
  if (status == 200) 
  {
    Serial.println("Dado enviado com sucesso!");
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Dado Enviado ");
    lcd.setCursor(0, 1);
    lcd.print("OK ");
    delay(1000);
  } 
  
  else 
    {
      Serial.print("Erro ao enviar dado: ");
      Serial.println(status);
      lcd.clear();
      lcd.backlight();
      lcd.setCursor(0, 0);
      lcd.print("Falha no Envio ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      delay(1000);
    }
  }



}




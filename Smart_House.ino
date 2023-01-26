#include <Servo.h>
#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C mylcd(0x27,16,2);
Servo servo_11;
Servo servo_10;
Servo servo_9;
dht dht11;
int nivel_solo;
int nivel_agua;
int botao_vermelho;
int botao_verde;
int tempo_botao_verde;
int valor_tela;
int garagem;
int fogo;
int temperatura;
int umidade;
volatile int luminosidade;
volatile int som;
bool estado_led;
bool tela_ativa;
String senha;

void setup(){
  luminosidade = 0;
  garagem = 0;
  nivel_agua = 0;
  nivel_solo = 0;
  valor_tela = 0;
  tempo_botao_verde = 0;
  temperatura = 0;
  umidade = 0;
  fogo = 0;
  estado_led = false;
  senha = "";
  mylcd.init();
  mylcd.backlight();
  mylcd.setCursor(2,0);
  mylcd.print("Smart House");
  servo_10.attach(10); // Janela
  servo_11.attach(11); // Garagem
  servo_9.attach(9);
  dht11.read11(12); // sensor de temperatura e umidade
  pinMode(3, OUTPUT); // Buzzer
  pinMode(6, OUTPUT); // Motor de passo
  pinMode(7, OUTPUT); // LED branco
  pinMode(8, INPUT); // Sensor de incêndio
  pinMode(13, OUTPUT); // LED vermelho
  pinMode(A0, INPUT); // Sensor metal
  pinMode(A1, INPUT); // Fotoresistor
  pinMode(A2, INPUT); // Sensor de chuva
  pinMode(A3, INPUT); // Sensor solo
 
  Serial.begin(9600);

  servo_9.write(0);
}

void loop() {
  
  luzExterna();

  garagem();
 
  soloSeco();

  incendio();
  
  temperatura();
  
  telasLCD();
  
  delay(0); 
}
void luzExterna(){
  // Inicio programação luz externa
  luminosidade = analogRead(A1); // Armazenando o valor do fotoresistor
  som = digitalRead(2); //Armazenando o valor do sensor de som
  //Serial.println(luminosidade);
  //Serial.println(som);
  if (luminosidade > 700){// Se estiver de noite 
    digitalWrite(7,HIGH);
  }else{// Se estiver de dia
    if(estado_led == false){
       digitalWrite(7,LOW);
    }
    if (som == 1){ // Se houver som no valor calibrado (palavra "liga")
      if(estado_led == false){
        //Ligar
        digitalWrite(7,HIGH);
        estado_led = true;
        delay(500);
      }else{
        //Desligar
        digitalWrite(7,LOW);
        estado_led = false;
        delay(500);
      }
    }
  }
}

void garagem(){
  // Inicio programação garagem
 garagem = digitalRead(A0);// Armazenando valor do interruptor de metal
 if (garagem == 0){
  servo_11.write(90);// Aberto
  tela_ativa = true;
  valor_tela = 4;
  telasLCD();
  delay(3000);
 }
 else if (garagem == 1){
  servo_11.write(0);// Fechado
 }
}

janela(){
  // Inicio programação janela dia de chuva
 nivel_agua = analogRead(A2);// Armazenando valor do sensor de chuva
 if (nivel_agua > 100){ // Verificar se está chovendo
  servo_10.write(0);
  valor_tela = 3;
  tela_ativa = true;
  telasLCD();
 }else{ // não está chovendo
  servo_10.write(45);
 }
  //Serial.println(nivel_agua);
}
void soloSeco(){
  nivel_solo = analogRead(A3);
  if (nivel_solo < 50){
    digitalWrite(13,HIGH);
    tone(3,532);
    delay(125);
    delay(200);
    digitalWrite(13,LOW);
    noTone(3);
    delay(200);
    valor_tela = 5;
    tela_ativa = true;
    telasLCD();
  }else{
    digitalWrite(13,LOW);
    noTone(3);
    valor_tela = 0;
    tela_ativa == true;
    telasLCD();
  }
}

void incendio(){
  // Inicio programação janela dia de chuva
 nivel_agua = analogRead(A2);// Armazenando valor do sensor de chuva
 if (nivel_agua > 100){ // Verificar se está chovendo
  servo_10.write(0);
  valor_tela = 3;
  tela_ativa = true;
  telasLCD();
 }else{ // não está chovendo
  servo_10.write(45);
 }
  //Serial.println(nivel_agua);
}

void temperatura(){
  //Inicio da programação sensor de temperatura e umidade
  temperatura = dht11.temperature;
  umidade = dht11.humidity;
  if(temperatura > 25){
    analogWrite(6, 255);
  }else{
    analogWrite(6, 0);
  }

}
void telasLCD(){
    botao_vermelho = digitalRead(4);
    botao_verde = digitalRead(5);
    if(valor_tela == 2){
      confirmarSenha();
      if(botao_vermelho == 0){
        delay(300);
        if(senha == "---.."){
          mylcd.clear();  
          mylcd.setCursor(5,0);
          mylcd.print("Acesso");
          mylcd.setCursor(3,1);
          mylcd.print("autorizado");
          som_senha_correta();
          delay(1000);
          servo_9.write(90);
          valor_tela = 0;
          tela_ativa = true;
          senha = "";
        }else{
          mylcd.clear();  
          mylcd.setCursor(5,0);
          mylcd.print("Acesso");
          mylcd.setCursor(5,1);
          mylcd.print("negado");
          som_senha_incorreta();
          delay(1000);
          valor_tela = 0;
          tela_ativa = true;
          senha = "";
        }
      }

    }else if(botao_vermelho == 0){
      Serial.println(valor_tela);
      delay(300);
      valor_tela++;
      tela_ativa = true;
    }

    if(botao_verde == 0){
      delay(300);
      servo_9.write(0);
    }
    
    if(tela_ativa == true){
      if(valor_tela == 0){
        mylcd.clear();  
        mylcd.setCursor(2,0);
        mylcd.print("Smart House");
         tela_ativa = false;
      }else if(valor_tela == 1){
        mylcd.clear();
        mylcd.setCursor(3,0);
        mylcd.print("Temp: ");
        mylcd.print(temperatura);
        mylcd.print("C"); 
        mylcd.setCursor(3,1);
        mylcd.print("Umid: ");
        mylcd.print(umidade);
        mylcd.print("%");
         tela_ativa = false;
      }else if(valor_tela == 2){
        mylcd.clear();
        mylcd.setCursor(0,0);
        mylcd.print("Senha: "); 
        //mylcd.setCursor(3,1);
         tela_ativa = false;
      }else  if(valor_tela == 3){
      mylcd.clear();
      mylcd.setCursor(1,0);
      mylcd.print("Esta chovendo");
      mylcd.setCursor(0,1);
      mylcd.print("Fechando janela");
      delay(1000);
      valor_tela = 0;
      tela_ativa == true;
      }else  if(valor_tela == 4){
        mylcd.clear();
        mylcd.setCursor(4,0);
        mylcd.print("Abrindo");
        mylcd.setCursor(4,1);
        mylcd.print("Garagem");
        delay(1000);
        valor_tela = 0;
        tela_ativa == true;
      }else  if(valor_tela == 5){
        mylcd.clear();
        mylcd.setCursor(6,0);
        mylcd.print("Solo");
        mylcd.setCursor(6,1);
        mylcd.print("Seco");
        delay(1000);
        tela_ativa = true;
      }else{
        valor_tela = 0;
        tela_ativa == true;
      }
    }   
}

void confirmarSenha(){
   Serial.println(botao_verde);
   botao_verde = digitalRead(5);
   if (botao_verde == 0) {
    delay(300);
    tempo_botao_verde = 1;
     while (botao_verde == 0) {
       delay(100);
       botao_verde = digitalRead(5);
       tempo_botao_verde++;
       
     }
    if (tempo_botao_verde > 1 && tempo_botao_verde < 3) {
      som_senha();
      senha = senha + ".";
    }
    if (tempo_botao_verde > 3) {
      som_senha();
      senha = senha + "-"; 
    } 
  }
  mylcd.setCursor(7,0);
  mylcd.print(senha);
  mylcd.print("_");
}

void som_senha() {
  tone(3,349);
  delay(125);
  delay(100);
  noTone(3);
  delay(100);
}
void som_senha_correta() {
  tone(3,349);
  delay(125);
  tone(3,449);
  delay(125);
  tone(3,549);
  noTone(3);
  delay(100);
}

void som_senha_incorreta() {
  tone(3,349);
  delay(500);
  noTone(3);
  delay(500);
  tone(3,349);
  delay(500);
  noTone(3);
  delay(500);
  tone(3,349);
  delay(500);
  noTone(3);
  delay(100);
}

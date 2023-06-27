#include <LiquidCrystal_I2C.h>
#include <Keypad.h> //inclusão da biblioteca teclado matricial
#include <string.h>
#define LedVermelho 12 //definição pino dos leds
#define LedAzul 13  //definição pino dos leds
#define rele1 2 //pino do rele 1
#define tempo 10 //tempo do som
#define autofalante 7 //pino do autofalante

char* senha = "7355608"; //definir uma senha -> VER COM A EQUIPE
int posicao = 0; //posição do numero na senha
int time = 65; // conntador time //tempo ate explodir //em segundos
const byte rows = 4; //numero de linhas e colunas do teclado
const byte cols = 4;
int freq = 2000;   //frequencia para o bip
int dur = 120;     //duração do bip
int codepos = 0;
int estadoled = LOW;  //variável para o estado do led
int intervalo = 1000; //1 segundo 
int previousMillis = 0;
int frequencia = 0;

char keyMap [rows] [cols] = { //teclado matricial, deixe igual o fisico
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins [rows] = {11, 10, 9, 8}; //linhas do teclado
byte colPins [cols] = {6, 5, 4, 3}; //colunas

Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, rows, cols);

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //endereço I2c módulo display //baixar biblioteca certa para usar esse
//LiquidCrystal_I2C lcd(32, 16, 2); // Address, 16 columns, 2 rows //para usar no thinkercad

void (*funcReset) () = 0;

void setup() 
{
  lcd.begin(16, 2);
  pinMode(LedVermelho, OUTPUT);  //setando o led como saída
  pinMode(ledAzul, OUTPUT);  //setando o led como saída
  pinMode(rele1, OUTPUT); //setando rele como saida
  lcd.setCursor(0, 0);
  lcd.print("BANZER AIRSOFT");
  lcd.setCursor(0, 1);
  lcd.print("TEAM!");
  delay(4000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BEM VINDOS");
  lcd.setCursor(0, 1);
  lcd.print("E BOM JOGO!");
  delay(4000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Senha Ativacao:");
  lcd.setCursor(0, 1);
  lcd.print("*******");
}

void contador(int tempoinicial) { //função para rotina de acionamento da bomba - começa a contagem de tempo da bomba ativada.
  lcd.clear();
  lcd.setCursor(0, 0); //nem é daqui que sai
  lcd.print("C4 ARMADAAA   ");
  lcd.setCursor(0, 1);
  lcd.print("*******");
  
  int count = time; //count é o tempo de jogo

  while (count > 0) { //enquanto o tempo de jogo for maior que zero...

    char numteclado;
    numteclado = keyget(); //... fica lendo as teclas pressionadas.
    Serial.println("Scanning...");
    
    if (numteclado == 'C') { // se ler que foi pressionada a tecla C vai resetar a senha e mandar para a pos inicial para recomeçar-- C de clear
      posicao = 0;
    }
    if (numteclado == senha [posicao]) { // a cada numero apertado avança uma posição para receber o prox numero

      posicao ++;
    }
    if (posicao == 7) { //rotina para desarmar se a senha for correta
      
      lcd.clear();
      lcd.print("BOMBA DESARMADA");
      lcd.setCursor(0, 1);
      lcd.print("MILITARY WINS");
      digitalWrite(LedVermelho, LOW);
      delay(150);
      digitalWrite(LedAzul), HIGH);
      delay(150);
      digitalWrite(LedAzul, LOW);
      delay(150);
      digitalWrite(LedAzul, HIGH);
      delay(150);
      digitalWrite(LedAzul, LOW);
      //delay(300);
      //digitalWrite(LedAzul, HIGH);
      //digitalWrite(rele1, HIGH);
      //delay(8000);
      //digitalWrite(rele1, LOW);
      delay(2000);
     /* digitalWrite(rele1, HIGH);
      delay(8000);
      digitalWrite(rele1, LOW);
      delay(2000);
      digitalWrite(rele1, HIGH);
      delay(8000);
      digitalWrite(rele1, LOW);
      delay(2000); */
      //delay(2000);
      numteclado = ' ';

      while (numteclado !='D'){

      lcd.setCursor(0, 0);
      lcd.print("Reiniciar: D     ");
      lcd.setCursor(0, 1);
      lcd.print("MILITARY WINS");
      //colocar algo para segurança do D
      char numteclado;
      numteclado = keyget();
      if (numteclado == 'D') {   //reinicia arduino após explodir
      break;
        } 
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reiniciando...");
      digitalWrite(LedAzul, LOW);
      delay(3000);
      funcReset();

    }
    
    lcd.setCursor(0, 0); //exibição do tempo regressivo (tempo restante) no lcd
    int temporestante = time - (millis() / 1000 - tempoinicial); //tempo restante é o tempo de jogo em segundos - (o tempo em que o arduino começou a rodar - tempo inicial)
    lcd.print("BOMBA ARMADA  ");
    lcd.setCursor(14, 0);
    lcd.print(temporestante);

    if (temporestante < 10) { // /trecho que adiciona um zero quando a contagem for menor que 10
      lcd.setCursor(13, 0);
      lcd.print("0");
      lcd.setCursor(15, 0);
      lcd.print(" "); //printa o tempo restante menor que 10.
    }

    if (temporestante == 300) { //5min  //rotinas para mudança da velocidade do bip
      intervalo = 800;
    }
    if (temporestante == 120) { //2min
      intervalo = 700;
    }
    if (temporestante == 60) { //1min
      intervalo = 600;
    }
    if (temporestante == 30) { //30 seg
      intervalo = 400;
    }
    if (temporestante == 15) { //15 seg
      intervalo = 250;
    }
    if (temporestante == 5) {  //5 seg
      intervalo = 150;
    }
    if (temporestante == 2) {  //2 seg
      intervalo = 1;
    }

    int currentMillis = millis(); //rotina para piscar led durante contagem

    if (currentMillis - previousMillis >= intervalo) {

      previousMillis = currentMillis;

      if (estadoled == LOW) { //Led piscando durante a contagem
        estadoled = HIGH;
      } else {
        estadoled = LOW;
      }
      digitalWrite(LedVermelho, estadoled);
      tone(autofalante, freq, dur);
    }

    if (temporestante <= 0) { // Quando a bomba explode - Mensagem se o tempo acabar
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("DETONADA");
      lcd.setCursor(0, 1);
      lcd.print("TERRORISTS WIN");
      digitalWrite(LedVermelho, HIGH);
      delay(150);
      digitalWrite(LedVermelho, LOW);
      delay(100);
      digitalWrite(LedVermelho, HIGH);
      delay(150);
      digitalWrite(LedVermelho, LOW);
      delay(300);
      digitalWrite(LedVermelho, HIGH);
      tocarsom();
      delay(500);
      digitalWrite(rele1, HIGH); //ativa rele conectado a sirene
      delay(1000);//aumentar aqui!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      digitalWrite(rele1, LOW); //desativa rele conectado a sirene
      delay(1000);
      lcd.setCursor(0, 0);
      lcd.print("Reiniciar: D     ");
      
      while (1) {  //fica em loooping infinito = true até que satisfaça uma condição
        char numteclado;
        numteclado = keyget();
        if (numteclado == 'D') {   //reinicia arduino após explodir
        break;
        }
      }
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reiniciando...");
      digitalWrite(LedVermelho, LOW);
      delay(1000);
      funcReset();
      }
    }
    
  return;
}

void tocarsom() { //som quando perde
   for (frequencia =150; frequencia < 1800; frequencia += 1)
  {
    tone(autofalante, frequencia, tempo);
    delay(1);
  }
  for (frequencia = 2800; frequencia > 150; frequencia -= 1)
  {
    tone(autofalante, frequencia, tempo);
    delay(1);
  }
}

char keyget() { //leitura da tecla pressionada
  char numteclado = myKeypad.getKey();  //define qual tecla é pressionada com getKey
 
  if (numteclado >= '0') { //posição da senha
    codepos++;
  }

  if (numteclado == 'C') { //reseta senha limpa a tela volta para zero a posição durante a contagem da bomba

    posicao = 0;

    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(" RESETADO!");
    delay(100);
    lcd.clear();
    lcd.print("4******");
    codepos = 0;
    lcd.setCursor(0, 1);
  }

  if (codepos >=7) //se a senha for maior ou igual a quantidade de 7 caracteres e nao for correta, avanca mais um para dar >8 é recomençar
  {
    codepos++;
  }

  if (codepos > 8) // se a quantidade de caracteres da senha for maior que 8 caracteres, limpa tudo e recomeça
  {
    lcd.clear();
    codepos = 0;
    lcd.print("Tente Novamente");
    lcd.setCursor(0, 1);
  }

  lcd.setCursor(codepos - 1, 1); //posição onde fica a senha digitada no lcd //é a posição atual - 1 quadrado
  
  if (numteclado == 'D') { //reinicia arduino durante a contagem da bomba e depois da explosao tbm eu acho
  lcd.clear();
  lcd.print("Reiniciando...");
  delay(2000);
  digitalWrite(LedVermelho, LOW);
  funcReset();
  }

  if (numteclado != NO_KEY)
   {
    //rotina para um som diferente para cada tecla pressionada

    if (numteclado == '1') {
      tone(autofalante, freq, dur);
      lcd.print(1);
    }

    if (numteclado == '2') {
      tone(autofalante, freq * 1.25, dur);
      lcd.print(2);
    }

    if (numteclado == '3') {
      tone(autofalante, freq * 1.5, dur);
      lcd.print(3);
    }

    if (numteclado == '4') {
      tone(autofalante, freq * 1.75, dur);
      lcd.print(4);
    }

    if (numteclado == '5') {
      tone(autofalante, freq, dur);
      lcd.print(5);
    }

    if (numteclado == '6') {
      tone(autofalante, freq * 2, dur);
      lcd.print(6);
    }

    if (numteclado == '7') {
      tone(autofalante, freq * 1.25, dur);
      lcd.print(7);
    }

    if (numteclado == '8') {
      tone(autofalante, freq * 2.75, dur);
      lcd.print(8);
    }

    if (numteclado == '9') {
      tone(autofalante, freq * 3, dur);
      lcd.print(9);
    }

    if (numteclado == 'A') {
      tone(autofalante, freq * 4.25, dur);
      lcd.print('A');
    }
    if (numteclado == 'B') {
      tone(autofalante, freq * 5.25, dur);
      lcd.print('B');
    }
    
    if (numteclado == 'C') {
      tone(autofalante, freq * 3, dur);
      lcd.print('C');
    }
    
    if (numteclado == 'D') {
      tone(autofalante, freq, dur);
      lcd.print('D');
    }
    
    if (numteclado == '0') {
      tone(autofalante, freq, dur);
      lcd.print(0);
    }
    
  }
  return numteclado;
}


void loop() { 
  char numteclado;
  numteclado = keyget();

  if (numteclado == 'C') {

    posicao = 0; //para recomeçar a senha

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SENHA CLEAR!");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Senha Ativacao!");
    lcd.setCursor(0, 1);
    lcd.print("*******");
    codepos = 0;
    lcd.setCursor(0, 1);
  }
  
  if (numteclado == senha [posicao]) {

    posicao ++;
  }
  if (posicao == strlen(senha)) { //se o n° da posição dos dígitos for igual a quantidade de caracteres do tamanho da senha a senha volta pro inicio, e o tempo inicial se torna o valor de milli.
    posicao = 0;
    int tempoinicial = (millis()) / 1000;
    contador(tempoinicial);
  }

  delay(100);
}

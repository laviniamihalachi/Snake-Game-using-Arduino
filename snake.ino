#include <LedControl.h>
#include <MAX7219.h>
#include <Wire.h>

const int dataInPin = 12;            
const int clockPin = 11;            
const int loadPin = 10;           
const int numDevices = 1;
const int VRxPin=A0;
const int VRyPin=A1;
const int SWPin=A5;


LedControl lc = LedControl (dataInPin, clockPin, loadPin, numDevices);
MAX7219 lc2 = MAX7219 (dataInPin, clockPin, loadPin, numDevices);

 
char textName[] = {"  SNAKE 2017 \0"};    //mesaj la inceputul joclui
unsigned long bufferLong [7] = {0};       //folosit la scrollText



bool gameOver = false;
int x, y, fruitX, fruitY, score;                         // (x,y)=coordonatele capului de sarpe , (fruitX,fruitY)= coordonatele fructului
enum eDirection {STOP = 0, left, right, up, down};            
eDirection dir; 
int tailX[70], tailY[70];
int nTail;                                                 //nTail=lungimea cozii
int message = 0, newLevel = 1;                                //message-afiseaza textName[] doar la inceputul jocului.
int lvl2 = 0, lvl3 = 0, lvl1 = 1;                               //lvl(1,2,3)=nivelul la care se afla jucatorul
int newGame = 1;                                       
char textLevel1[] = {" LEVEL 1 \0"};
char textLevel2[] = {" LEVEL 2 \0"};                     //mesaje afisate pe matrice
char textLevel3[] = {" LEVEL 3 \0"};
char textEndGame[] = {"  ~GameOver~ \0"};


void setup() {
  pinMode(VRxPin, INPUT);
  pinMode(VRyPin, INPUT);
  pinMode(SWPin, INPUT);
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  randomSeed( analogRead(0) ); 
  initMAX();
  Serial.begin(9600);
  fruitX = random(1, 7);       //se stabilesc coordonatele fructului
  fruitY = random(1, 7);
}

// face initializarile la fiecare nivel si inceput de joc. Adica sarpele are doar capul si este pozitionat in centru iar scorul=0.
void setup2() {    
  x = 4;     //(4,4)=pozitia de inceput a sarpelui
  y = 4;
  score = 0;
  nTail = 0;   //lungimea cozii sarpelui
  gameOver = false; 
}

//ce se afiseaza pe matrice. La primul nivel matricea nu are pereti.
void drawLevel1() {   
  lc.setRow(0, 0, B00000000);
  lc.setRow(0, 7,B00000000);
  lc.setColumn(0, 0, B00000000);
  lc.setColumn(0, 7, B00000000); 
  
  lc.setLed(0, fruitX, fruitY, true);  //fructul
  lc.setLed(0, x, y, true);   //capul
  
  for(int k = 0; k < nTail; k++)
  lc.setLed(0, tailX[k], tailY[k], true); //coada
  
  delay(200);
 
  lc.setLed(0, x, y, false);
  for(int k = 0; k < nTail; k++)
  lc.setLed(0, tailX[k], tailY[k], false);  
  lc.setLed(0, fruitX, fruitY, false);
}


// la nivelul 2 matricea are 2 pereti
void drawLevel2() { 
  lc.setRow(0, 0, B00000000);
  lc.setRow(0, 7, B00000000);
  lc.setColumn(0, 0, B11111111);
  lc.setColumn(0, 7, B11111111);
  
  lc.setLed(0, fruitX, fruitY, true);
  lc.setLed(0, x, y, true);
  
  for(int k = 0; k < nTail; k++)
  lc.setLed(0, tailX[k], tailY[k], true);
  
  delay(300);
  lc.setLed(0, x, y, false);
  for(int k = 0; k < nTail; k++)
  lc.setLed(0, tailX[k], tailY[k], false);   
  lc.setLed(0, fruitX, fruitY, false);
}

// la nivelul 3 , matricea are 4 pereti
void drawLevel3() {
  lc.setRow(0, 0, B11111111);
  lc.setRow(0, 7, B11111111);
  lc.setColumn(0, 0, B11111111);
  lc.setColumn(0, 7, B11111111);
  
  lc.setLed(0, fruitX, fruitY, true);
  lc.setLed(0, x, y, true);
  
  for(int k = 0; k < nTail; k++)
  lc.setLed(0, tailX[k], tailY[k], true);
  
  delay(300);
  lc.setLed(0, x, y, false);
  for(int k = 0; k < nTail; k++)
  lc.setLed(0, tailX[k], tailY[k], false);  
  lc.setLed(0, fruitX, fruitY, false); 
}

// citim cinformatia data de joystick si alegem directia
void input() {
  int yjoy = analogRead(VRyPin) / 146;
  int xjoy = analogRead(VRxPin) / 146;
  if(xjoy > 3) dir = left;
  if(xjoy < 3) dir = right;
  if(yjoy > 3) dir = down;
  if(yjoy < 3) dir = up;
 
  if(newLevel == 1) {
   dir = STOP; 
   newLevel = 0;
  }  // daca se incepe un nivel nou, prima directie este stop. Sarpele sta pe loc.
 
  if(newGame == 1) {
   dir = STOP; 
   newGame = 0;
  }     //la fel si pentru cazul in care se incepe un joc nou.
}

// conditiile ce trebuiesc respectate la primul nivel
void logicLevel1() { 
  int prevX = tailX[0];    //se retine prima pozitie in coada
  int prevY = tailY[0];
  int prev2X, prev2Y;
  tailX[0] = x;         //in capul cozii se adauga noua directie
  tailY[0] = y;
  for(int i = 1; i < nTail; i++) {//se modifica coada tailX/Y[i]=tailX/Y[i+1];
    prev2X = tailX[i];
    prev2Y = tailY[i];
    tailX[i] = prevX;
    tailY[i] = prevY;
    prevX = prev2X;
    prevY = prev2Y;
  }
  switch(dir) { // in functie de directia aleasa, se modifica coordonatele capului
    case left:  x--;
    break;
    case right:  x++;
    break;
    case up:  y--;
    break;
    case down:  y++;
    break;
    default:
    break;
  }

  if(x == 8) x = 0;  //if-urile ne ajuta sa trecem dintr-un perete in altul
  if(x == -1) x = 7;
  if(y == 8) y = 0;
  if(y == -1) y = 7;
  for(int i = 0; i < nTail; i++)   //conditie ce verifica daca sarpele isi mananca coada
  if(tailX[i] == x && tailY[i] == y ) { 
     gameOver = true;   //in caz afirmativ, jucatorul pierde. ceea ce inseamna ca in cazul in care vrea sa inceapa un joc nou,
     message = 0;       //message va trebui sa ia valoare 0 pentru a afisa iar mesajul initial
     newGame = 1;
  }
 
  if(x == fruitX && y == fruitY) { // se verifica daca sarpele a mancat fructul
     score += 10;      // in caz afirmativ se creste scorul si se schimba coordonatle fructului si se creste si coada sarpelelui
     fruitX = random(1,7);
     fruitY = random(1,7);
     nTail++;  
  }
}

// aceeasi gandire ca la nivelul 1 , apar modificari pentru ca avem pereti noi prin care sarpele nu poate trece.
void logicLevel2() {
  int prevX = tailX[0];
  int prevY = tailY[0];
  int prev2X, prev2Y;
  tailX[0] = x;
  tailY[0] = y;
  for(int i = 1; i < nTail; i++) {
     prev2X = tailX[i];
     prev2Y = tailY[i];
     tailX[i] = prevX;
     tailY[i] = prevY;
     prevX = prev2X;
     prevY = prev2Y;
   }
  switch(dir) {
     case left:  x--;
     break;
     case right:  x++;
     break;
     case up: y--;
     break;
     case down:  y++;
     break;
     default:
     break;
   }
 
  if(y > 6 || y < 1) { // daca sarpele intra in perete , jucatorul pierde
      gameOver = true;
      message = 0;
      lvl2 = 0;
      lvl1 = 1;
      newGame = 1;
   }
        
  if(x == 8) x = 0;
  if(x == -1) x = 7;
  
  for(int i = 0; i < nTail; i++)
  if(tailX[i] == x && tailY[i] == y) { 
      gameOver = true; 
      message = 0;  
      lvl2 = 0; 
      lvl1 = 1;
      newGame = 1;
  }
        
  if(x == fruitX && y == fruitY) {
     score += 10;
     fruitX = random(1,7);
     fruitY = random(1,7);
     nTail++;
  }
}

// sarpele este blocat intre 4 pereti
void logicLevel3() {
  int prevX = tailX[0];
  int prevY = tailY[0];
  int prev2X, prev2Y;
  tailX[0] = x;
  tailY[0] = y;
 
  for(int i = 1; i < nTail; i++) {
     prev2X = tailX[i];
     prev2Y = tailY[i];
     tailX[i] = prevX;
     tailY[i] = prevY;
     prevX = prev2X;
     prevY = prev2Y;       
  }
  switch(dir) {
     case left:  x--;
     break;
     case right:  x++;
     break;
     case up:  y--;
     break;
     case down: y++;
     break;
     default:
     break;
   }
 
  if(y > 6 || y < 1 || x < 1 || x > 6){ 
     gameOver = true;
     message = 0;
     lvl3 = 0; 
     lvl1 = 1;
     newGame = 1;
  }
 
  for(int i = 0; i < nTail; i++)
  if(tailX[i] == x && tailY[i] == y) { 
      gameOver = true;
      message = 0; 
      lvl3 = 0; 
      lvl1 = 1;
      newGame = 1;
   }
  if(x == fruitX && y == fruitY) {
     score += 10 ; 
     fruitX = random(1,7);
     fruitY = random(1,7); 
     nTail++;
  }
}


void loop() {
  int startButton = analogRead(SWPin);
  Serial.println(startButton);
 
  if(startButton == 0) {
      if(message == 0){ 
          scrollMessage(textName); 
          message = 1;
          setup2();
      }
                                                          
      if(startButton == 0) { 
          scrollMessage(textLevel1);
          scrollMessage("3 2 1 ");
                       
          while(!gameOver) {
                if(lvl3 == 1) {
                    drawLevel3();
                    input();
                    logicLevel3();
                    if(score == 20) {
                       scrollMessage("WINNER!!"); 
                       lvl3 = 0;
                       lvl1 = 1;
                       gameOver = true;
                       message = 0;
                       newGame = 1;
                    }
                }
                if(lvl2 == 1) {
                    drawLevel2();
                    input();
                    logicLevel2();
                    if(score == 20) {
                       scrollMessage(textLevel3);
                       lvl3 = 1;
                       lvl2 = 0;
                       setup2();
                       newLevel = 1;
                    }
                }
                                      
                if(lvl1 == 1) {
                    drawLevel1();
                    input();
                    logicLevel1();
                    if(score == 50) {
                       scrollMessage(textLevel2);
                       lvl2 = 1;
                       lvl1 = 0;
                       setup2();
                       newLevel = 1;
                    }
                }
          }

          scrollMessage(textEndGame); 
      }
   } 
}

const byte font5x7 [] = {      //Numeric Font Matrix
  B00000000,  //Space (Char 0x20)
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,

  B00100000,  //!
  B00100000,
  B00100000,
  B00100000,
  B00000000,
  B00000000,
  B00100000,

  B01010000,  //"
  B01010000,
  B01010000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,

  B01010000,  //#
  B01010000,
  B11111000,
  B01010000,
  B11111000,
  B01010000,
  B01010000,

  B00100000,  //$
  B01111000,
  B10100000,
  B01110000,
  B00101000,
  B11110000,
  B00100000,

  B11000000,  //%
  B11001000,
  B00010000,
  B00100000,
  B01000000,
  B10011000,
  B00011000,

  B01100000,  //&
  B10010000,
  B10100000,
  B01000000,
  B10101000,
  B10010000,
  B01101000,

  B01100000,  //'
  B00100000,
  B01000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,

  B00010000,  //(
  B00100000,
  B01000000,
  B01000000,
  B01000000,
  B00100000,
  B00010000,

  B01000000,  //)
  B00100000,
  B00010000,
  B00010000,
  B00010000,
  B00100000,
  B01000000,

  B00000000,  //*
  B00100000,
  B10101000,
  B01110000,
  B10101000,
  B00100000,
  B00000000,

  B00000000,  //+
  B00100000,
  B00100000,
  B11111000,
  B00100000,
  B00100000,
  B00000000,

  B00000000,  //,
  B00000000,
  B00000000,
  B00000000,
  B01100000,
  B00100000,
  B01000000,

  B00000000,  //-
  B00000000,
  B11111000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,

  B00000000,  //.
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B01100000,
  B01100000,

  B00000000,  ///
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
  B00000000,

  B01110000,  //0
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  B01110000,

  B00100000,  //1
  B01100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B01110000,

  B01110000,  //2
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B11111000,

  B11111000,  //3
  B00010000,
  B00100000,
  B00010000,
  B00001000,
  B10001000,
  B01110000,

  B00010000,  //4
  B00110000,
  B01010000,
  B10010000,
  B11111000,
  B00010000,
  B00010000,

  B11111000,  //5
  B10000000,
  B11110000,
  B00001000,
  B00001000,
  B10001000,
  B01110000,

  B00110000,  //6
  B01000000,
  B10000000,
  B11110000,
  B10001000,
  B10001000,
  B01110000,

  B11111000,  //7
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B00100000,
  B00100000,

  B01110000,  //8
  B10001000,
  B10001000,
  B01110000,
  B10001000,
  B10001000,
  B01110000,

  B01110000,  //9
  B10001000,
  B10001000,
  B01111000,
  B00001000,
  B00010000,
  B01100000,

  B00000000,  //:
  B01100000,
  B01100000,
  B00000000,
  B01100000,
  B01100000,
  B00000000,

  B00000000,  //;
  B01100000,
  B01100000,
  B00000000,
  B01100000,
  B00100000,
  B01000000,

  B00010000,  //<
  B00100000,
  B01000000,
  B10000000,
  B01000000,
  B00100000,
  B00010000,

  B00000000,  //=
  B00000000,
  B11111000,
  B00000000,
  B11111000,
  B00000000,
  B00000000,

  B01000000,  //>
  B00100000,
  B00010000,
  B00001000,
  B00010000,
  B00100000,
  B01000000,

  B01110000,  //?
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B00000000,
  B00100000,

  B01110000,  //@
  B10001000,
  B00001000,
  B01101000,
  B10101000,
  B10101000,
  B01110000,

  B01110000,  //A
  B10001000,
  B10001000,
  B10001000,
  B11111000,
  B10001000,
  B10001000,

  B11110000,  //B
  B10001000,
  B10001000,
  B11110000,
  B10001000,
  B10001000,
  B11110000,

  B01110000,  //C
  B10001000,
  B10000000,
  B10000000,
  B10000000,
  B10001000,
  B01110000,

  B11100000,  //D
  B10010000,
  B10001000,
  B10001000,
  B10001000,
  B10010000,
  B11100000,

  B11111000,  //E
  B10000000,
  B10000000,
  B11110000,
  B10000000,
  B10000000,
  B11111000,

  B11111000,  //F
  B10000000,
  B10000000,
  B11110000,
  B10000000,
  B10000000,
  B10000000,

  B01110000,  //G
  B10001000,
  B10000000,
  B10111000,
  B10001000,
  B10001000,
  B01111000,

  B10001000,  //H
  B10001000,
  B10001000,
  B11111000,
  B10001000,
  B10001000,
  B10001000,

  B01110000,  //I
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B01110000,

  B00111000,  //J
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B10010000,
  B01100000,

  B10001000,  //K
  B10010000,
  B10100000,
  B11000000,
  B10100000,
  B10010000,
  B10001000,

  B10000000,  //L
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B11111000,

  B10001000,  //M
  B11011000,
  B10101000,
  B10101000,
  B10001000,
  B10001000,
  B10001000,

  B10001000,  //N
  B10001000,
  B11001000,
  B10101000,
  B10011000,
  B10001000,
  B10001000,

  B01110000,  //O
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,

  B11110000,  //P
  B10001000,
  B10001000,
  B11110000,
  B10000000,
  B10000000,
  B10000000,

  B01110000,  //Q
  B10001000,
  B10001000,
  B10001000,
  B10101000,
  B10010000,
  B01101000,

  B11110000,  //R
  B10001000,
  B10001000,
  B11110000,
  B10100000,
  B10010000,
  B10001000,

  B01111000,  //S
  B10000000,
  B10000000,
  B01110000,
  B00001000,
  B00001000,
  B11110000,

  B11111000,  //T
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,

  B10001000,  //U
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,

  B10001000,  //V
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B01010000,
  B00100000,

  B10001000,  //W
  B10001000,
  B10001000,
  B10101000,
  B10101000,
  B10101000,
  B01010000,

  B10001000,  //X
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  B10001000,

  B10001000,  //Y
  B10001000,
  B10001000,
  B01010000,
  B00100000,
  B00100000,
  B00100000,

  B11111000,  //Z
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
  B11111000,

  B01110000,  //[
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B01110000,

  B00000000,  //(Backward Slash)
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  B00001000,
  B00000000,

  B01110000,  //]
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B01110000,

  B00100000,  //^
  B01010000,
  B10001000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,

  B00000000,  //_
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B11111000,

  B01000000,  //`
  B00100000,
  B00010000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,

  B00000000,  //a
  B00000000,
  B01110000,
  B00001000,
  B01111000,
  B10001000,
  B01111000,

  B10000000,  //b
  B10000000,
  B10110000,
  B11001000,
  B10001000,
  B10001000,
  B11110000,

  B00000000,  //c
  B00000000,
  B01110000,
  B10001000,
  B10000000,
  B10001000,
  B01110000,

  B00001000,  //d
  B00001000,
  B01101000,
  B10011000,
  B10001000,
  B10001000,
  B01111000,

  B00000000,  //e
  B00000000,
  B01110000,
  B10001000,
  B11111000,
  B10000000,
  B01110000,

  B00110000,  //f
  B01001000,
  B01000000,
  B11100000,
  B01000000,
  B01000000,
  B01000000,

  B00000000,  //g
  B01111000,
  B10001000,
  B10001000,
  B01111000,
  B00001000,
  B01110000,

  B10000000,  //h
  B10000000,
  B10110000,
  B11001000,
  B10001000,
  B10001000,
  B10001000,

  B00100000,  //i
  B00000000,
  B01100000,
  B00100000,
  B00100000,
  B00100000,
  B01110000,

  B00010000,  //j
  B00000000,
  B00110000,
  B00010000,
  B00010000,
  B10010000,
  B01100000,

  B10000000,  //k
  B10000000,
  B10010000,
  B10100000,
  B11000000,
  B10100000,
  B10010000,

  B01100000,  //l
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B01110000,

  B00000000,  //m
  B00000000,
  B11010000,
  B10101000,
  B10101000,
  B10001000,
  B10001000,

  B00000000,  //n
  B00000000,
  B10110000,
  B11001000,
  B10001000,
  B10001000,
  B10001000,

  B00000000,  //o
  B00000000,
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,

  B00000000,  //p
  B00000000,
  B11110000,
  B10001000,
  B11110000,
  B10000000,
  B10000000,

  B00000000,  //q
  B00000000,
  B01101000,
  B10011000,
  B01111000,
  B00001000,
  B00001000,

  B00000000,  //r
  B00000000,
  B10110000,
  B11001000,
  B10000000,
  B10000000,
  B10000000,

  B00000000,  //s
  B00000000,
  B01110000,
  B10000000,
  B01110000,
  B00001000,
  B11110000,

  B01000000,  //t
  B01000000,
  B11100000,
  B01000000,
  B01000000,
  B01001000,
  B00110000,

  B00000000,  //u
  B00000000,
  B10001000,
  B10001000,
  B10001000,
  B10011000,
  B01101000,

  B00000000,  //v
  B00000000,
  B10001000,
  B10001000,
  B10001000,
  B01010000,
  B00100000,

  B00000000,  //w
  B00000000,
  B10001000,
  B10101000,
  B10101000,
  B10101000,
  B01010000,

  B00000000,  //x
  B00000000,
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,

  B00000000,  //y
  B00000000,
  B10001000,
  B10001000,
  B01111000,
  B00001000,
  B01110000,

  B00000000,  //z
  B00000000,
  B11111000,
  B00010000,
  B00100000,
  B01000000,
  B11111000,

  B00010000,  //{
  B00100000,
  B00100000,
  B01000000,
  B00100000,
  B00100000,
  B00010000,

  B00100000,  //|
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,

  B01000000,  //}
  B00100000,
  B00100000,
  B00010000,
  B00100000,
  B00100000,
  B01000000,

  B00000000,  //~
  B00000000,
  B00000000,
  B01101000,
  B10010000,
  B00000000,
  B00000000,

  B01100000,  // (Char 0x7F)
  B10010000,
  B10010000,
  B01100000,
  B00000000,
  B00000000,
  B00000000,
};



// Initialize the max7219 chips
void initMAX(){

  for(int address=0;address<numDevices;address++) 
  {
    lc2.shutdown(address,false);
    lc2.setIntensity(address,1);
    lc2.clearDisplay(address);
  }
}

// Load character into scroll buffer
void loadBufferLong(int ascii){
  if (ascii >= 0x20 && ascii <=0x7f){
    for (int a=0;a<7;a++){                    // Loop 7 times for a 5x7 font
      unsigned long c = font5x7[((ascii - 0x20) * 7) + a];     // Index into character table to get row data
      unsigned long x = bufferLong [a];       // Load current scroll buffer
      x = x | c;                              // OR the new character onto end of current
      bufferLong [a] = x;                     // Store in buffer
     
    }
  }
}

// Rotate the buffer
void rotateBufferLong(){
  for (int a=0;a<7;a++){                    // Loop 7 times for a 5x7 font
    unsigned long x = bufferLong [a];       // Get buffer entry
    x = x<<1;                               // Rotate left one bit
    bufferLong [a] = x;                     // Store new buffer
  }
}  

// Display Buffer on LED matrix
void printBufferLong(){
  for (int a=0;a<7;a++){                    // Loop 7 times for a 5x7 font
    unsigned long x = bufferLong [a];       // Get buffer entry
    int y = (x>>16) & 0xff;                   // Mask off left hand character
    lc2.setRow(1,a,y);                       // Send row to relevent MAX7219 chip
    y = (x>>8) & 0xff;                        // Mask off right hand character
    lc2.setRow(0,a,y);                       // Send row to relevent MAX7219 chip
    
  }
  
}
// Scroll Message
void scrollMessage(char * messageString) {
  int counter = 0;
  while (messageString[counter] !=0){
    loadBufferLong(messageString[counter]);
    for (int x=0; x<7;x++){
      rotateBufferLong();
      printBufferLong();
      delay(100);
    }
    counter++;
  }
}


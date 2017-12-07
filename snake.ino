#include "LedControl.h"
#include "math.h" 
#include<LiquidCrystal.h>
#include<EEPROM.h>
#define EN 2
#define RS 1
#define BUTTON 13
#define JOY_X A0
#define JOY_Y A1
/* 
 n=lungimea sarpelui
 wait=delay-ul, dat in functie de gameMode
 invert=are valoarea 1 cand axele nu sunt inversate si -1  in caz contrar. se inmulteste cu valoarea ce schimba pozitia sarpelui
 pointer=decide nivelul : easy, medium, hard
 c=are valoarea 0 daca jocul nu a inceput si 1 daca da
*/
int  wait, score, iValue, jValue,hearts;
char pointer, yApple, xApple, invert, bonus, gameMode, xFood, yFood, jMove, iMove, n;
bool beginG;
unsigned long time, prevTime, bonusTime;
int address;

LedControl lc=LedControl (12, 11, 10, 1);
LiquidCrystal lcd (RS, EN, 4, 5, 6, 7);

struct Snake
{ 
  char i, j;
}p[65];

void setup() 
{ 
  pinMode (BUTTON, INPUT);
  pinMode (3, OUTPUT);
  analogWrite (3, 100);
  lcd.begin (16, 2);

  lcd.setCursor (1, 0);
  lcd.print ("Welcome! Press");
  lcd.setCursor (3, 1);
  lcd.print ("OK to start");

  lc.shutdown (0,false);
  lc.setIntensity (0, 8);
  lc.clearDisplay (0);

  resetValues();
 /* while ( address < 23 )
  {
    EEPROM.write(address,0);
    address++;
  }*/
}

void resetValues ()
{
  beginG = 0;
  int max = 0;
  address = 0;
  while (address < 23 && EEPROM.read (address))
  {
    max = max + EEPROM.read (address);
    address ++;
  }
  if (score > max)
  {
     address = 0;
     while (address < 23)
     {
       if (score > 255)
       {
         EEPROM.write (address, 255);
         score = score - 255;
       }
       else
       {
          EEPROM.write (address, score);
          score = 0;
       }
       address++;
      }  
   }
  score = 0;
  invert = 1;
  iMove = 0;
  jMove = 0;
  bonus = 0;
  gameMode = 0;
  pointer = 1;
  xApple = 1;
  yApple = -1;
  n = 1;
  hearts = 3;
}

void setValues()
{
  jMove = 0; 
  iMove = 0;
  do{
    iValue = analogRead (JOY_X);
    jValue = analogRead (JOY_Y);
                                                               // iMove, jMove = valori care se adauga pozitiei anterioare pentru a se modifica punctul
    if (iValue < 500) 
      iMove = 1 * invert;
    else if (iValue > 530) 
      iMove = -1 * invert;
    if (jValue < 500) 
      jMove = -1 * invert;
    else if (jValue > 520) 
      jMove = 1 * invert;
    if ( n != 1)                                              // Pentru  a continui clipirea marului
      fadeApple();
    } while (iMove == 0 && jMove == 0  && n != 1);           // Pentru a nu memora  pozitii noi in momentele in care Joystick-ul stagneaza pe valoarea 0
}

void setValues1()
{
  char prejMove = jMove; 
  char preiMove = iMove;
  iMove = 0;
  jMove = 0;
    iValue = analogRead (JOY_X);
    jValue = analogRead (JOY_Y);
                                              
    if (iValue < 500) 
      iMove = 1 * invert;
    else if (iValue > 530) 
      iMove = -1 * invert;
    if (jValue < 500) 
      jMove = -1 * invert;
    else if (jValue > 520) 
      jMove = 1 * invert;
    if (iMove==0 && jMove==0)                               // In cazul in care Joystick-ul nu introduce o noua directie, sarpele isi va continua directia anterioara
    {
      iMove = preiMove;
      jMove = prejMove;
    }
}

void setCoords()
{
  if (gameMode == 1)                                        // Nivelul easy
    setValues();
  else
    setValues1();                                            // Medium & hard
  
  p[n].i = p[n].i + iMove;                                   // Coordonatele capului se modifica in functie de iMove si jMove
  p[n].j = p[n].j + jMove;
  if (p[n].i > 7) 
    p[n].i = 0;
  else if (p[n].i<0) 
    p[n].i = 7;
  if (p[n].j > 7)                                            // In cazul in care punctul depaseste marginile, se va spauna in capatul simetric fata de centrul matricei (7->0, 0->7)
    p[n].j = 0;
  else if (p[n].j<0) 
    p[n].j = 7;
  lc.setLed (0, p[n].i, p[n].j, true);
}

void randomPoint()
{
  xFood = random (0,7);
  yFood = random (0,7);
  for (int k=1; k<=n; k++) 
    if (p[k].i == xFood && p[k].j == yFood)                    // Pentru ca punctul aleator sa nu se suprapuna cu sarpele
    {
      xFood = random (0,7);
      yFood = random (0,7);
      k = 1;
    }
  lc.setLed (0,xFood,yFood,true);
}

int eatDot()
{
  if (! (xFood == p[n].i && yFood == p[n].j))
    return 0;                                                   // Daca coordonatele capului sarpelui nu coincid cu cele ale mancarii, se returneaza valoarea 0
  return 1;                                                     // Altfel, se returneaza valoarea 1
}

void moveSnake()
{
  int k; 
  int aux,auy;
  aux = p[1].i; 
  auy = p[1].j;
  if (n == 1) 
    lc.setLed (0, aux, auy, false);                            // Cand sarpele este format dintr-un singur punct, mai intai se stinge punctul, dupa care se aprinde noua lui pozitie

  for (k = 1; k < n; k++)                                      // Primele n-1 puncte ale sarpelui preiau valoare urmatorului punct
  {
    p[k].i = p[k+1].i;
    p[k].j = p[k+1].j;
  }
  setCoords();                                                // Este setat noul cap al sarpelui
  lc.setLed (0, p[n].i, p[n].j, true);                        // Este aprinsa noua pozitie a capului sarpelui
  if (n > 1) 
    lc.setLed (0, aux, auy, false);                           // Cand sarpele este format din mai multe puncte, coada sarpelui se stinge dupa ce capul este aprins
}

void growSnake()                                              // Cand sarpele mananca un punct, lungimea acestuia creste, iar capul ia o noua valoare, mai intai initializata cu capul anterior
{
  n++;
  p[n].i = p[n-1].i;
  p[n].j = p[n-1].j;
  setCoords();
  lc.setLed (0, p[n].i, p[n].j, true);
  randomPoint();                                              // Se spauneaza un nou punct aleator
  score += (10 * gameMode);                                   // Scorul creste
}

int biteSnake()                                               // Functie care returneaza valoarea 1 in care sarpele s-a muscat si 0 in caz contrar
{
  for (int t = 1; t < n; t++)
    if (p[t].i == p[n].i && p[t].j == p[n].j)
      return 1;
  return 0;
}

void spawnApple()                                            // Functie care spauneaza marul
{
  int ok;
  do{
    ok = 0;
    xApple = random (0, 7);
    yApple = random (0, 7);
    for (int t=1; t<=n; t++)
      if (xApple == p[t].i && yApple == p[t].j)             // Pentru a nu se suprapune cu sarpele
        ok = 1;
    if (xApple == xFood && yApple == yFood)                 // Pentru a nu coincide cu mancarea sarpelui
      ok = 1;
    } while (ok == 1);
  lc.setLed (0, xApple, yApple, true);
}

int eatApple()
{
  if (! (xApple==p[n].i && yApple==p[n].j))               // Daca coordonatele capului sarpelui nu coincid cu cele ale marului, se returneaza valoarea 0
    return 0;
  xApple = -1; 
  yApple = 1;
  return 1;                                                 // Altfel, se returneaza 1, se genereaza un bonus aleator si coordonatele marului primesc valorile 1 si -1 ce nu pot fi afisate pe display
} 

void generateBonus()
{
  bonus = random (1, 10);                                 // Pentru bonus, se genereaza o valoare aleatoare intre 1 si 10 care va reprezenta tipul bonusului
  if (bonus <= 2)
    invert = -1;                                          // 1.Inversarea axelor
  else if (bonus <= 4) 
    wait *= 2;                                            // 2.Viteza sarpelui incetineste de doua ori
  else if (bonus <= 6)  
    wait /= 2;                                            // 3.Viteza sarpelui creste de doua ori
  else if (bonus <= 9)
    score = score + (random (1,5) * 10);                  // 4.Puncte bonus random
  else if (bonus == 10)
    hearts++;                                             // 5.Viata bonus
}

void resetBonus()
{
  if (bonus <= 2) 
    invert = 1;
  else if (bonus <= 4) 
    wait /= 2;
  else if (bonus <= 6) 
    wait *= 2;
  bonus = 0;
}

void fadeApple()
{
  if (xApple * yApple >= 0)                                       // Daca exista vreun mar pe harta, sa clipeasca
  {
    lc.setLed (0, xApple, yApple, false);
    delay (50);
    lc.setLed (0, xApple, yApple, true);
   }
}

void deleteApple()
{
  lc.setLed (0, xApple, yApple, false);
  xApple = -1; 
  yApple = 1;
}

void beginGame()
{
  for (int t = 1; t <= n; t++)
  {
    p[t].i = -2;
    p[t].j = -1;
  }
  n = 1;
  p[1].i = 2;
  p[1].j = 3;
  iMove = 0;
  jMove = 0;
  lc.setLed (0, p[1].i, p[1].j, true);
  randomPoint();
  beginG = 1;
}

void displayScore()
{
   lcd.clear();
   lcd.setCursor (1, 0);
   lcd.print ("Eat the most!");
   lcd.setCursor (0, 1);
   lcd.print ("S:");
   lcd.setCursor (3, 1);
   lcd.print (score);
   lcd.setCursor (12, 1);
   lcd.print ("L:");
   lcd.setCursor (15, 1);
   lcd.print (hearts);
}

void setDelay()
{
  if (gameMode == 1) 
      wait = 100;
  else if (gameMode == 2) 
      wait = 230;
  else if (gameMode == 3) 
      wait = 120;
}

int startMenu()
{
  int coordY;
  char pointer = 1;
  delay (500);
  while (digitalRead (BUTTON) == 0)
  {
    coordY = analogRead (JOY_Y);
    delay (500);
    if (coordY > 560 && pointer < 3) 
      pointer++;
    if (coordY < 450 && pointer > 1) 
      pointer --;
    
    lcd.clear();
    lcd.setCursor (0, 0);
    lcd.print ("Select Level:");
    lcd.setCursor (0, 1);
    if (pointer == 1)
      lcd.print (" Easy >>");
    else if (pointer == 2)
      lcd.print ("<< Medium >>");
    else if (pointer == 3)
      lcd.print ("<< Hard");
  }
  lcd.clear();
  return pointer;
}

void loseLife()
{
  hearts--;
  lc.clearDisplay (0);
  lcd.clear();
  lcd.setCursor (4, 0);
  lcd.print ("You bit");
  lcd.setCursor (3, 1);
  lcd.print ("yourself !");
  delay (3000);
  lcd.clear();
  lcd.setCursor (0, 0);
  lcd.print ("The game will");
  lcd.setCursor (0, 1);
  lcd.print ("start in 3 sec");
  delay (3000);
  beginG = 0;
}


void displayEnd()                        
{
  lc.clearDisplay (0);
  for (int t = 0; t <= 7; t++)                                // Afiseaza un X pe matrice
  {
    lc.setLed (0, t, t, true);
    lc.setLed (0, t, 7-t, true);
  }
  int sum = 0;
  address = 0;
  while (address < 23 )
  {
    sum += EEPROM.read (address);
    address++;
  }
  address=0;
   xApple = -1;
   yApple = 1;
   lcd.clear();
   lcd.setCursor (2, 0);
   lcd.print ("END OF GAME");
   lcd.setCursor (0, 1);                                   // Pe lcd se afiseaza un mesaj de incheiere si scorul final
   lcd.print ("S:");
   lcd.setCursor (3, 1);
   lcd.print (score);
   lcd.setCursor (8, 1);
   lcd.print ("HS:");
   lcd.setCursor (12, 1);
   lcd.print (sum);
   
   if (sum < score)
   {
    delay (2000);
    lcd.clear();
    lcd.setCursor (1,0);
    lcd.print ("NEW HIGHSCORE!");
    lcd.setCursor (4,1);
    lcd.print ("Hold OK");
    delay (2000);
   }
   
   delay (100);
   if (digitalRead (BUTTON)) 
   {  
      setup();
      delay (500);
   }
}


void loop() 
{
  if (gameMode == 0 && digitalRead (BUTTON))
  {
    gameMode = startMenu();
    setDelay();
  }
  
  else if (gameMode != 0)
  {
     if (beginG == 0) 
        beginGame();
     displayScore();
     time = millis();
     if (biteSnake() == 0)
     {
       if (time % 8000 <= 100 * (log(time) - 3) && xApple * yApple < 0)      // O data la aproxmativ 8 secunde, se spauneaza un mar daca nu exista deja pe harta        {
       {
         prevTime = time; 
         spawnApple();
       }
       if (time >= prevTime + 3000)                                          // Se sterge marul dupa 3 secunde
         deleteApple();
          
       if (eatApple())                      
       {
         bonusTime = time;
         generateBonus();
       }
       if (time >= bonusTime + 3000 && bonus != 0)
         resetBonus();
       if (eatDot())                                    
         growSnake();
       else
         moveSnake();
       delay (wait);
       fadeApple();
      }
    
      else if (biteSnake() == 1) 
      {
        if (hearts > 1)
          loseLife();
        else
          displayEnd();  
      }
  }
}


#include <LiquidCrystal.h>

#define PIN_BUTTON 4

#define SPRITE_RUN1              1
#define SPRITE_RUN2              2
#define SPRITE_JUMP              3
#define SPRITE_JUMP_UPPER        '.'    // little dot for head
#define SPRITE_JUMP_LOWER        4
#define SPRITE_TERRAIN_EMPTY     ' '
#define SPRITE_TERRAIN_SOLID     5
#define SPRITE_TERRAIN_SOLID_R   6
#define SPRITE_TERRAIN_SOLID_L   7

#define HERO_X      1   // fixed horizontal spot for the Dino
#define W           16  // width of the LCD

// terrain types
#define TERRAIN_EMPTY       0
#define TERRAIN_LOWER       1
#define TERRAIN_UPPER       2

// hero states
enum HeroPos {
  OFF,
  RUN1, RUN2,
  J1, J2, J3, J4, J5, J6, J7, J8,
  RUN_UP1, RUN_UP2
};

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
char terrainU[W+1], terrainL[W+1];

void initChars(){
  static byte gfx[] = {
    // RUN1
    B01100,B01100,B00000,B01110,B11100,B01100,B11010,B10011,
    // RUN2
    B01100,B01100,B00000,B01100,B01100,B01100,B01100,B01110,
    // JUMP frame
    B01100,B01100,B00000,B11110,B01101,B11111,B10000,B00000,
    // JUMP lower
    B11110,B01101,B11111,B10000,B00000,B00000,B00000,B00000,
    // GROUND full
    B11111,B11111,B11111,B11111,B11111,B11111,B11111,B11111,
    // GROUND right
    B00011,B00011,B00011,B00011,B00011,B00011,B00011,B00011,
    // GROUND left
    B11000,B11000,B11000,B11000,B11000,B11000,B11000,B11000
  };
  for(int i=0;i<7;i++){
    lcd.createChar(i+1, &gfx[i*8]);
  }
  for(int i=0;i<W;i++){
    terrainU[i]=SPRITE_TERRAIN_EMPTY;
    terrainL[i]=SPRITE_TERRAIN_EMPTY;
  }
}

// shift a line of terrain left, bringing in newTerrain at the right
void advanceLine(char *line, byte newTerrain){
  for(int i=0;i<W;i++){
    char cur = line[i];
    char nxt = (i==W-1)? newTerrain : line[i+1];
    switch(cur){
      case SPRITE_TERRAIN_EMPTY:
        line[i] = (nxt==SPRITE_TERRAIN_SOLID ? SPRITE_TERRAIN_SOLID_R : SPRITE_TERRAIN_EMPTY);
        break;
      case SPRITE_TERRAIN_SOLID:
        line[i] = (nxt==SPRITE_TERRAIN_EMPTY ? SPRITE_TERRAIN_SOLID_L : SPRITE_TERRAIN_SOLID);
        break;
      case SPRITE_TERRAIN_SOLID_R:
        line[i] = SPRITE_TERRAIN_SOLID;
        break;
      case SPRITE_TERRAIN_SOLID_L:
        line[i] = SPRITE_TERRAIN_EMPTY;
        break;
    }
  }
}

// draw both rows + score, return true if collision
bool drawHero(byte pos, char *U, char *L, unsigned int score){
  bool hit=false;
  char saveU = U[HERO_X], saveL = L[HERO_X];
  byte u,l;
  switch(pos){
    case OFF:                u=l=SPRITE_TERRAIN_EMPTY; break;
    case RUN1:               u=SPRITE_TERRAIN_EMPTY; l=SPRITE_RUN1; break;
    case RUN2:               u=SPRITE_TERRAIN_EMPTY; l=SPRITE_RUN2; break;
    case J1: case J8:        u=SPRITE_TERRAIN_EMPTY; l=SPRITE_JUMP; break;
    case J2: case J7:        u=SPRITE_JUMP_UPPER;    l=SPRITE_JUMP_LOWER; break;
    case J3: case J4: case J5: case J6:
                             u=SPRITE_JUMP;         l=SPRITE_TERRAIN_EMPTY; break;
    case RUN_UP1:            u=SPRITE_RUN1;         l=SPRITE_TERRAIN_EMPTY; break;
    case RUN_UP2:            u=SPRITE_RUN2;         l=SPRITE_TERRAIN_EMPTY; break;
  }
  if(u!=' '){ U[HERO_X]=u; hit |= (saveU!=SPRITE_TERRAIN_EMPTY); }
  if(l!=' '){ L[HERO_X]=l; hit |= (saveL!=SPRITE_TERRAIN_EMPTY); }

  // print
  U[W]='\0'; L[W]='\0';
  lcd.setCursor(0,0); lcd.print(U);
  lcd.setCursor(0,1); lcd.print(L);
  // score on top‐right
  byte digs = (score>9999)?5:(score>999)?4:(score>99)?3:(score>9)?2:1;
  lcd.setCursor(W-digs,0); lcd.print(score);

  U[HERO_X]=saveU; L[HERO_X]=saveL;
  return hit;
}

void setup(){
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  lcd.begin(16,2);
  initChars();
}

void loop(){
  static byte hero=RUN1;
  static byte terrainType=TERRAIN_EMPTY, terrainDur=1;
  static bool playing=false, blink=false;
  static unsigned int distance=0;

  // waiting state
  if(!playing){
    drawHero(blink?OFF:hero, terrainU,terrainL, distance>>3);
    if(blink){
      lcd.setCursor(0,0);
      lcd.print("Press Start     ");
    }
    delay(250);
    blink = !blink;
    if(digitalRead(PIN_BUTTON)==LOW){
      delay(50);
      while(digitalRead(PIN_BUTTON)==LOW);
      // reset
      hero=RUN1; playing=true; distance=0;
      initChars();
    }
    return;
  }

  // game running
  advanceLine(terrainL, terrainType==TERRAIN_LOWER?SPRITE_TERRAIN_SOLID:SPRITE_TERRAIN_EMPTY);
  advanceLine(terrainU, terrainType==TERRAIN_UPPER?SPRITE_TERRAIN_SOLID:SPRITE_TERRAIN_EMPTY);

  if(--terrainDur==0){
    if(terrainType==TERRAIN_EMPTY){
      terrainType = (random(3)==0)?TERRAIN_UPPER:TERRAIN_LOWER;
      terrainDur = 2 + random(10);
    } else {
      terrainType = TERRAIN_EMPTY;
      terrainDur = 10 + random(10);
    }
  }

  // jump input
  if(digitalRead(PIN_BUTTON)==LOW){
    delay(50);
    while(digitalRead(PIN_BUTTON)==LOW);
    if(hero<=RUN2) hero=J1;
  }

  // draw & collision
  if(drawHero(hero,terrainU,terrainL, distance>>3)){
    playing=false;
  } else {
    // advance hero animation
    if(hero==RUN2 || hero==J8)      hero=RUN1;
    else if(hero>=J3 && hero<=J5 && terrainL[HERO_X]!=SPRITE_TERRAIN_EMPTY)
                                     hero=RUN_UP1;
    else if(hero>=RUN_UP1 && terrainL[HERO_X]==SPRITE_TERRAIN_EMPTY)
                                     hero=J5;
    else if(hero==RUN_UP2)          hero=RUN_UP1;
    else                            hero++;
    distance++;
  }

  delay(50);
}

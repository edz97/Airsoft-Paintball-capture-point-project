#include <Arduino.h>
#include <kingOfTheHill.h> // Project specific
#include <armDisarm.h>     // Project specific
#include <menu.h>          // Project specific
#include <settings.h>      // Project specific
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h> // For I2C LCD
#include <TM1637Display.h>         // For 7-segment displays

#define INTERNAL_LED0_J101_R 8
#define INTERNAL_LED0_J101_G 9
#define INTERNAL_LED0_J101_B 10

#define INTERNAL_LED1_J102_R 11
#define INTERNAL_LED1_J102_G 12
#define INTERNAL_LED1_J102_B 13

#define EXTERNAL_LED_J103_R 4
#define EXTERNAL_LED_J103_G 5
#define EXTERNAL_LED_J103_B 6
#define EXTERNAL_LED_J103_W 7

#define TEAM_BUTTON_1 A0
#define TEAM_BUTTON_2 A1
#define TEAM_BUTTON_3 A2
#define TEAM_BUTTON_4 A3

#define STOP_BUTTON A4

#define SPI_J105_TX1 18
#define SPI_J105_RX1 19
#define SPI_J105_D1 22

#define SPI_J106_TX2 16
#define SPI_J106_RX2 17
#define SPI_J106_D2 23

#define SPI_J107_TX3 14
#define SPI_J107_RX3 15
#define SPI_J107_D3 24

#define LCD0_J113_1 26 // DIO display 1
#define LCD0_J113_2 27 // CLK display 1
#define LCD0_J113_3 28 // DIO display 2
#define LCD0_J113_4 29 // CLK display 2
#define LCD0_J113_5 30 // DIO display 3
#define LCD0_J113_6 31 // CLK display 3
#define LCD0_J113_7 32 // DIO display 4
#define LCD0_J113_8 33 // CLK display 4
#define LCD0_J113_9 34
#define LCD0_J113_10 35

#define LCD_1_DIO LCD0_J113_2 // DIO display 1
#define LCD_1_CLK LCD0_J113_1 // CLK display 1
#define LCD_2_DIO LCD0_J113_4 // DIO display 2
#define LCD_2_CLK LCD0_J113_3 // CLK display 2
#define LCD_3_DIO LCD0_J113_6 // DIO display 3
#define LCD_3_CLK LCD0_J113_5 // CLK display 3
#define LCD_4_DIO LCD0_J113_8 // DIO display 4
#define LCD_4_CLK LCD0_J113_7 // CLK display 4
TM1637Display display1(LCD_1_CLK, LCD_1_DIO);
TM1637Display display2(LCD_2_CLK, LCD_2_DIO);
TM1637Display display3(LCD_3_CLK, LCD_3_DIO);
TM1637Display display4(LCD_4_CLK, LCD_4_DIO);

#define LCD1_J114_1 36
#define LCD1_J114_2 37
#define LCD1_J114_3 38
#define LCD1_J114_4 39
#define LCD1_J114_5 40
#define LCD1_J114_6 41
#define LCD1_J114_7 42
#define LCD1_J114_8 43
#define LCD1_J114_9 44
#define LCD1_J114_10 45

#define SMOKE_PIN 25

game::KofH myGame;
armdisarm armDisarm;
menu myMenu;

// Dispaly stuff
LiquidCrystal_PCF8574 lcd(0x27);                     // set the LCD address to 0x27 for a 16 chars and 2 line display
byte dotOff[] = {0b00000, 0b01110, 0b10001, 0b10001, // Custom Character for dotOff
                 0b10001, 0b01110, 0b00000, 0b00000};
byte dotOn[] = {0b00000, 0b01110, 0b11111, 0b11111, // Custom Character for dot
                0b11111, 0b01110, 0b00000, 0b00000};

// Custom 7-segment display symbols/characters
uint8_t customSegment_ON[4] = {0b0000000, 0b0111111, 0b1010100, 0b0000000};
uint8_t customSegment_OFF[4] = {0b0000000, 0b0111111, 0b1110001, 0b1110001};
uint8_t customSegment_PrSS[4] = {0b1110011, 0b1010000, 0b1101101, 0b1101101};
uint8_t customSegment_UPP[4] = {0b0000000, 0b0111110, 0b1110011, 0b1110011};
uint8_t customSegment_Down[4] = {0b0111111, 0b1011100, 0b0011100, 0b1010100};
uint8_t customSegment_Ent[4] = {0b0000000, 0b1111001, 0b1010100, 0b1111000};
uint8_t customSegment_BACK[4] = {0b1111100, 0b1110111, 0b1011000, 0b1111001};

// functions
void setAllSegmentDisplaysNum(uint8_t value);
void setAllSegmentDisplaysSym(uint8_t *value);
bool TrueOnceASecond_main(void);

// Extern functions and variables
extern void settings_save();
extern void settings_read();
extern void settings_initiateDefault();

// Variables for game
int pointsTeam1 = 0;
int pointsTeam2 = 0;
int pointsTeam3 = 0;
int pointsTeam4 = 0;
int winningTeam = 0;

// Other Variables
bool FLAG_EnterMenu = false;

void setup()
{
  // put your setup code here, to run once:
  pinMode(INTERNAL_LED0_J101_R, OUTPUT);
  pinMode(INTERNAL_LED0_J101_G, OUTPUT);
  pinMode(INTERNAL_LED0_J101_B, OUTPUT);

  pinMode(INTERNAL_LED1_J102_R, OUTPUT);
  pinMode(INTERNAL_LED1_J102_G, OUTPUT);
  pinMode(INTERNAL_LED1_J102_B, OUTPUT);

  pinMode(EXTERNAL_LED_J103_R, OUTPUT);
  pinMode(EXTERNAL_LED_J103_G, OUTPUT);
  pinMode(EXTERNAL_LED_J103_B, OUTPUT);
  pinMode(EXTERNAL_LED_J103_W, OUTPUT);

  pinMode(TEAM_BUTTON_1, INPUT);
  pinMode(TEAM_BUTTON_2, INPUT);
  pinMode(TEAM_BUTTON_3, INPUT);
  pinMode(TEAM_BUTTON_4, INPUT);

  pinMode(SMOKE_PIN, OUTPUT);
  digitalWrite(SMOKE_PIN, HIGH);

  myMenu.MENU_BUTTON_ENTER = TEAM_BUTTON_2;
  myMenu.MENU_BUTTON_BACK = TEAM_BUTTON_4;
  myMenu.MENU_BUTTON_UP = TEAM_BUTTON_3;
  myMenu.MENU_BUTTON_DOWN = TEAM_BUTTON_1;
  myMenu.MENU_BUTTON_1 = TEAM_BUTTON_1;
  myMenu.MENU_BUTTON_2 = TEAM_BUTTON_2;
  myMenu.MENU_BUTTON_3 = TEAM_BUTTON_3;
  myMenu.MENU_BUTTON_4 = TEAM_BUTTON_4;

  // LCD setup
  lcd.begin(16, 2); // initialize the lcd

  lcd.createChar(1, dotOff);
  lcd.createChar(2, dotOn);
  // LCD setup end

  // Segment display setup
  // Display testing-code here
  display1.setBrightness(0x03);
  display1.showNumberDec(0001);
  display2.setBrightness(0x03);
  display2.showNumberDec(0002);
  display3.setBrightness(0x03);
  display3.showNumberDec(0003);
  display4.setBrightness(0x03);
  display4.showNumberDec(0004);
  // Segment display setup end

  Serial.begin(9600);

  settings_read();

  for (int i = 0; i < 5; i++)
  {
    char buffer[20];
    int cntDwn = 5 - i;
    sprintf(buffer, "Counting down %d\n\n", cntDwn);
    Serial.print(buffer);
    delay(500);
  }

  // If a button is pressed, set Flag high, and enter "menu-mode" for access to settings.
  if (digitalRead(myMenu.MENU_BUTTON_ENTER) || digitalRead(myMenu.MENU_BUTTON_UP) || digitalRead(myMenu.MENU_BUTTON_DOWN) || digitalRead(myMenu.MENU_BUTTON_BACK))
  {
    display1.setSegments(customSegment_UPP);
    display2.setSegments(customSegment_Ent);
    display3.setSegments(customSegment_Down);
    display4.setSegments(customSegment_BACK);
    FLAG_EnterMenu = true;
    settings_initiateDefault();
  }
  // initiateDefault();

  // Set display text
  lcd.setBacklight(1);
  lcd.home();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press 1 for KofH");
  lcd.setCursor(0, 1);
  lcd.print("2 for ArmDisarm");
}

void loop()
{
  game::digitalWrite_OFF_ToExtLED();
  // If any button is pressed at power-up, enter menu. Restart device to enter game-mode.
  if (FLAG_EnterMenu)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Entering menu...");
    delay(2000);

    myMenu.menuLoop(); // Enter infinite loop. Restart device to get out of it. (only to be entered at startup)
  }

  // Enter loop for a game-mode if apropiate button is pressed:

  // KofH
  if (digitalRead(TEAM_BUTTON_1))
  {
    // Initialize game settings
    myGame.pointsToWin = s_KofH_pointsToWin;
    myGame.numberOfTeams = s_KofH_numberOfTeams;
    myGame.activeTeams[0] = s_KofH_activeTeams[0];
    myGame.activeTeams[1] = s_KofH_activeTeams[1];
    myGame.activeTeams[2] = s_KofH_activeTeams[2];
    myGame.activeTeams[3] = s_KofH_activeTeams[3];
    myGame.gameDuration = s_KofH_gameDuration;

    for (;;)
    {
      // Option to manually end game by pressing all team-buttons at once
      /* if (digitalRead(TEAM_BUTTON_1) && digitalRead(TEAM_BUTTON_2) && digitalRead(TEAM_BUTTON_3) && digitalRead(TEAM_BUTTON_4)){
        myGame.FLAG_gameEnded = true;
      } */

      // Set white off, is currently not used at all (RGB LEDs are used instead)
      digitalWrite(EXTERNAL_LED_J103_W, HIGH);

      // Delayed start
      static int delayTime = s_KofH_delayedStartTime;
      if (delayTime > 0)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Game starts in:");
        lcd.setCursor(0, 1);
        lcd.print(delayTime);
        lcd.print("s");
        delayTime--;
        (delayTime % 2 == 0) ? game::analogWrite_Team_ToExtLED(myGame.teamColourRGBW, game::Neutral) : game::digitalWrite_OFF_ToExtLED();
        delay(500);
        (delayTime % 2 == 0) ? game::analogWrite_Team_ToExtLED(myGame.teamColourRGBW, game::Neutral) : game::digitalWrite_OFF_ToExtLED();
        delay(500);
      }
      // Game running after delayed start
      else
      {

        myGame.kingOfTheHill(); // must run continuously during a game to update state and stats (is not a loop)

        static uint32_t lastMillis = 0;
        if (millis() - lastMillis > 500)
        {
          lastMillis = millis(); // update lastMillis

          // Update segment displays
          myGame.activeTeams[0] ? display1.showNumberDec(myGame.pointsTeam1) : display1.setSegments(customSegment_OFF);
          myGame.activeTeams[1] ? display2.showNumberDec(myGame.pointsTeam2) : display2.setSegments(customSegment_OFF);
          myGame.activeTeams[2] ? display3.showNumberDec(myGame.pointsTeam3) : display3.setSegments(customSegment_OFF);
          myGame.activeTeams[3] ? display4.showNumberDec(myGame.pointsTeam4) : display4.setSegments(customSegment_OFF);

          /*    display1.showNumberDec(myGame.pointsTeam1);
             display2.showNumberDec(myGame.pointsTeam2);
             display3.showNumberDec(myGame.pointsTeam3);
             display4.showNumberDec(myGame.pointsTeam4); */

          // Update LCD
          if (myGame.FLAG_gameEnded)
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Game ended!");
            lcd.setCursor(0, 1);
            if (myGame.winningTeam == 0)
            {
              lcd.print("No winner!");
            }
            else
            {
              lcd.print("Team ");
              lcd.print(myGame.winningTeam);
              lcd.print(" won!");
            }
          }
          else
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            if (myGame.leadingTeam == 0)
            {
              lcd.print("No team leading");
            }
            else
            {
              lcd.print("Team ");
              lcd.print(myGame.leadingTeam);
              lcd.print(" leading!");
            }
            lcd.setCursor(0, 1);
            if (myGame.gameDuration - myGame.gameTimeElapsed > 60)
            {
              lcd.print("Time left: ");
              lcd.print((myGame.gameDuration - myGame.gameTimeElapsed) / 60);
              lcd.print("min");
            }
            else
            {
              lcd.print("Time left: ");
              lcd.print(myGame.gameDuration - myGame.gameTimeElapsed);
              lcd.print("s");
            }
          }
        }
      }
    }
  }

  if (digitalRead(TEAM_BUTTON_2))
  {
    // Initialize game settings
    armDisarm.gameDuration = s_armDisarm_gameDuration;
    armDisarm.defendTime = s_armDisarm_defendTime;
    armDisarm.defendTimeLeft = armDisarm.defendTime;
    armDisarm.disarmTime = s_armDisarm_disarmTime;
    armDisarm.armTime = s_armDisarm_disarmTime;

    for (;;)
    {
      // Option to manually end game by pressing all team-buttons at once
      /* if (digitalRead(TEAM_BUTTON_1) && digitalRead(TEAM_BUTTON_2) && digitalRead(TEAM_BUTTON_3) && digitalRead(TEAM_BUTTON_4)){
        myGame.FLAG_gameEnded = true;
      } */

      // Set white off, is currently not used at all (RGB LEDs are used instead)
      digitalWrite(EXTERNAL_LED_J103_W, HIGH);

      // Delayed start
      static int delayTime = s_armDisarm_delayedStartTime;
      if (delayTime > 0)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Game starts in:");
        lcd.setCursor(0, 1);
        lcd.print(delayTime);
        lcd.print("s");
        delayTime--;
        (delayTime % 2 == 0) ? game::analogWrite_Team_ToExtLED(myGame.teamColourRGBW, game::Neutral) : game::digitalWrite_OFF_ToExtLED();
        delay(500);
        (delayTime % 2 == 0) ? game::analogWrite_Team_ToExtLED(myGame.teamColourRGBW, game::Neutral) : game::digitalWrite_OFF_ToExtLED();
        delay(500);
      }
      // Game running after delayed start
      else
      {

        armDisarm.main_func(); // must run continuously during a game to update state and stats (is not a loop)

        static uint32_t lastMillis = 0;
        if (millis() - lastMillis > 500)
        {
          lastMillis = millis(); // update lastMillis

          // Update LCD
          if (armDisarm.FLAG_gameEnded)
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            if (armDisarm.winningTeam == 0)
            {
              if (armDisarm.gameState == armdisarm::disarmed)
              {
                lcd.print("BOMB DISARMED!");
                lcd.setCursor(0, 1);
                lcd.print("CT won the game.");
              }
              else
              {
                lcd.print("BOMB not planted");
                lcd.setCursor(0, 1);
                lcd.print("CT won the game.");
              }
            }
            else
            {
              lcd.print("BOMB EXPLODED!");
              lcd.setCursor(0, 1);
              lcd.print("Terrorists won.");
            }
          }
          else
          {
            static bool showRemTime = false;
            static int showRemGameTimeCnt = 0;
            if ((armDisarm.gameTimeElapsed%2) == 0)
            {
              showRemGameTimeCnt++;
              if (showRemGameTimeCnt == 6)
              {
                showRemTime = !showRemTime;
              }
              else if (showRemGameTimeCnt > 9)
              {
                showRemTime = !showRemTime;
                showRemGameTimeCnt = 0;
              }
            }
            lcd.clear();
            lcd.setCursor(0, 0);
            if (armDisarm.gameState == armdisarm::unarmed)
            {
              // Not armed.
              if (armDisarm.arming)
              {
                int armingCntDown = armDisarm.counter_armByTime;
                // arming in process
                lcd.print("Keep holding ");
                lcd.print(armingCntDown);
                lcd.print("s");
                lcd.setCursor(0, 1);
                lcd.print("to arm the bomb.");
                setAllSegmentDisplaysNum(armingCntDown);
              }
              else
              {
                if (showRemTime)
                {
                  // Show remaining game time in LCD display
                  lcd.print("-- NOT ARMED  --");
                  lcd.setCursor(0, 1);
                  unsigned int gameTimeLeft = armDisarm.gameDuration - armDisarm.gameTimeElapsed;
                  if (gameTimeLeft > 60)
                  {
                    lcd.print("Time left: ");
                    lcd.print(gameTimeLeft / 60);
                    lcd.print("min");
                    setAllSegmentDisplaysSym(customSegment_PrSS);
                  }
                  else
                  {
                    lcd.print("Time left: ");
                    lcd.print(gameTimeLeft);
                    lcd.print("s");
                    uint8_t secLeft = gameTimeLeft;  // cast to type uint8_t
                    setAllSegmentDisplaysNum(secLeft); // show on all 7-segment displays
                  }
                }
                else
                {
                  // Show arming info
                  lcd.print("Hold 4 buttons");
                  lcd.setCursor(0, 1);
                  lcd.print("to arm the bomb.");
                  setAllSegmentDisplaysSym(customSegment_PrSS);
                }
              }
            }
            else if (armDisarm.gameState == armdisarm::tobe_armed)
            {
              lcd.print("!!BOMB PLANTED!!");
              lcd.setCursor(0, 1);
              lcd.print(">Now defend it.<");
              setAllSegmentDisplaysSym(customSegment_ON);
            }
            else if (armDisarm.gameState == armdisarm::armed) // TS have planted and armed bomb.
            {
              int disarmingCntDown = armDisarm.counter_disarmByTime;
              if (armDisarm.disarming)
              {
                // disarming in process
                lcd.print("Keep holding ");
                lcd.print(disarmingCntDown);
                lcd.print("s");
                lcd.setCursor(0, 1);
                lcd.print("to disarm bomb.");
                setAllSegmentDisplaysNum(disarmingCntDown);
              }
              else
              {
                lcd.print("-- BOMB ARMED --");
                lcd.setCursor(0, 1);
                unsigned int defendTimeLeft = armDisarm.defendTimeLeft;
                if (showRemTime or (defendTimeLeft < 60))
                {
                  // Show remaining game time in LCD display
                  if (defendTimeLeft > 60)
                  {
                    lcd.print("Time left: ");
                    lcd.print(defendTimeLeft / 60);
                    lcd.print("min");
                    setAllSegmentDisplaysSym(customSegment_PrSS);
                  }
                  else
                  {
                    lcd.print("Time left: ");
                    lcd.print(defendTimeLeft);
                    lcd.print("s");
                    uint8_t secLeft = defendTimeLeft; // cast to type uint8_t
                    setAllSegmentDisplaysNum(secLeft);  // show on all 7-segment displays
                  }
                }
                else
                {
                  // Show disarming info
                  lcd.print("!Hold to disarm!");
                  setAllSegmentDisplaysSym(customSegment_PrSS);
                }
              }
            }
            else if (armDisarm.gameState == armdisarm::tobe_disarmed)
            {
              lcd.print("!BOMB DISARMED!");
              lcd.setCursor(0, 1);
              lcd.print("God work solider");
              setAllSegmentDisplaysSym(customSegment_OFF);
            }
            else if (armDisarm.gameState == armdisarm::tobe_exploded)
            {
              lcd.print("!BOMB Exploded!");
              lcd.setCursor(0, 1);
              lcd.print("   GAME OVER   ");
            }
            else if (armDisarm.gameState == armdisarm::tobe_timeout)
            {
              lcd.print("!BOMB TIMEOUT!");
              lcd.setCursor(0, 1);
              lcd.print("     CT won     ");
            }
          }
        }
      }
    }
  }
  /*
  else if (digitalRead(TEAM_BUTTON_2))
  {
    for (;;)
    {
      Serial.println("Button 2 pressed");
    }
  }
  else if (digitalRead(TEAM_BUTTON_3))
  {
    for (;;)
    {
      Serial.println("Button 3 pressed");
    }
  }
  else if (digitalRead(TEAM_BUTTON_4))
  {
    for (;;)
    {
      Serial.println("Button 4 pressed");
    }
  } */
}

// Set all 4 7-segment dispays to the same numeric value
void setAllSegmentDisplaysNum(uint8_t value)
{
  display1.showNumberDec(value);
  display2.showNumberDec(value);
  display3.showNumberDec(value);
  display4.showNumberDec(value);
}

// Set all 4 7-segment dispays to the same value
void setAllSegmentDisplaysSym(uint8_t *value)
{
  display1.setSegments(value);
  display2.setSegments(value);
  display3.setSegments(value);
  display4.setSegments(value);
}

// Returns true once every second
bool TrueOnceASecond_main(void)
{
    static unsigned long previousMillis = 0;
    static unsigned long currentMillis;
    currentMillis = millis();
    if (currentMillis - previousMillis >= 1000)
    {
        previousMillis = currentMillis;
        return true;
    }
    return false;
}
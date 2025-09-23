#include "Arduino.h"
#include <menu.h>
#include <settings.h>
#include <Wire.h> 
#include <LiquidCrystal_PCF8574.h> // For I2C LCD
#include <EEPROM.h>
#include <TM1637Display.h> // For 7-segment displays

/* #define MENU_BUTTON_ENTER TEAM_BUTTON_4
#define MENU_BUTTON_BACK TEAM_BUTTON_2
#define MENU_BUTTON_UP TEAM_BUTTON_1
#define MENU_BUTTON_DOWN TEAM_BUTTON_3 */
#define BUFFERLENGTH 100

// Display stuff
// extern LiquidCrystal_PCF8574 lcd(0x27); // set the LCD address to 0x27 for a 16 chars and 2 line display
extern LiquidCrystal_PCF8574 lcd;
extern TM1637Display display1;
extern TM1637Display display2;
extern TM1637Display display3;
extern TM1637Display display4;

// Varius variables for menu navigation
int menu_cursorPos = 1;
int last_cursorPos = 0;
uint8_t subMenu1 = 0;
uint8_t subMenu2 = 0;
bool FLAG_forMenu_enter = false;
bool FLAG_forMenu_back = false;
bool FLAG_shortFreeze = false;
char intermidiateBuffer[BUFFERLENGTH];

int last_subMenu0_cursorPos;
int last_subMenu1_cursorPos;
int last_subMenu1;
int last_subMenu2;
int menu_localCounter = 0; // To be used in a submenu if needed

extern int s_KofH_pointsToWin;
extern int s_KofH_numberOfTeams;
extern bool s_KofH_activeTeams[4];
extern int s_KofH_gameDuration;
extern int s_KofH_delayedStartTime;

// Custom 7-segment display symbols/characters
extern uint8_t customSegment_ON[4];
extern uint8_t customSegment_OFF[4];

void menu::menuLoop()
{
    for (;;)
    {
        // Updates menu navigation flags for "enter" and "back"
        menuEnterBackUpdate(FLAG_forMenu_enter, FLAG_forMenu_back, MENU_BUTTON_ENTER, MENU_BUTTON_BACK);
        menuCursorUpdate(menu_cursorPos, MENU_BUTTON_UP, MENU_BUTTON_DOWN); // Updates menu_cursorPos

        // Controls actions when "Enter" is pressed.
        if (FLAG_forMenu_enter) // "Enter"
        {
            FLAG_forMenu_enter = false;
            subMenu2 = ((subMenu1 != 0) && (subMenu2 == 0)) ? menu_cursorPos : subMenu2;
            subMenu1 = (subMenu1 == 0) ? menu_cursorPos : subMenu1;
            menu_cursorPos = 1;
            last_cursorPos = 1;
        }
        // Controls actions when "Back" is pressed.
        else if (FLAG_forMenu_back) // "Back"
        {
            FLAG_forMenu_back = false;
            subMenu1 = (subMenu2 == 0) ? 0 : subMenu1;
            subMenu2 = 0;
            last_cursorPos = 0;

            // Reset cursor position to last position in comming submenu
            if ((subMenu1 == 0) && (subMenu2 == 0))
            {
                menu_cursorPos = last_subMenu0_cursorPos;
            }
            else if ((subMenu1 != 0) && (subMenu2 == 0))
            {
                menu_cursorPos = last_subMenu1_cursorPos;
            }
        }

        if ((last_cursorPos != menu_cursorPos) || (last_subMenu1 != subMenu1) || (last_subMenu2 != subMenu2))
        {
            menuDisplay(menu_cursorPos, subMenu1, subMenu2);
            last_cursorPos = menu_cursorPos;
            last_subMenu1 = subMenu1;
            last_subMenu2 = subMenu2;
        }
    }
}

// Reads digital pin and sets flags for menu navigation. Flag must be reset outside of this function.
void menu::menuEnterBackUpdate(bool &FLAG_enter_true, bool &FLAG_back_true, uint8_t PIN_BUTTON_FOR_ENTER, uint8_t PIN_BUTTON_FOR_BACK)
{
    if (digitalRead(PIN_BUTTON_FOR_ENTER) == HIGH)
    {
        FLAG_enter_true = true;
        delay(300); // debounce
    }
    else if ((digitalRead(PIN_BUTTON_FOR_BACK) == HIGH))
    {
        FLAG_back_true = true;
        delay(300); // debounce
    }
}

// Reads digital pins and increments/decrements menu_cursorPos. Roll-over is not implemented.
void menu::menuCursorUpdate(int &cursorPos, uint8_t PIN_BUTTON_FOR_UP, uint8_t PIN_BUTTON_FOR_DOWN)
{
    if (digitalRead(PIN_BUTTON_FOR_UP) == HIGH)
    {
        last_cursorPos = cursorPos;
        cursorPos++;
        delay(300); // debounce

        // Keep track of last cursor position in each submenu
        if ((subMenu1 == 0) && (subMenu2 == 0))
        {
            last_subMenu0_cursorPos = cursorPos;
        }
        else if ((subMenu1 != 0) && (subMenu2 == 0))
        {
            last_subMenu1_cursorPos = cursorPos;
        }
    }
    else if (digitalRead(PIN_BUTTON_FOR_DOWN) == HIGH)
    {
        last_cursorPos = cursorPos;
        cursorPos--;
        delay(300); // debounce

        // Keep track of last cursor position in each submenu
        if ((subMenu1 == 0) && (subMenu2 == 0))
        {
            last_subMenu0_cursorPos = cursorPos;
        }
        else if ((subMenu1 != 0) && (subMenu2 == 0))
        {
            last_subMenu1_cursorPos = cursorPos;
        }
    }
}

// Indent and mark with a cursor ('>') the selected line in the menu
void menu::menuCursor(int cursorPos, int bufferLength, char *bufferLine1, char *bufferLine2)
{
    if ((bufferLine1[0] != '>') && (bufferLine2[0] != '>')) // Prevent cursor from being printed twice
    {
        switch (cursorPos)
        {
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
            snprintf(intermidiateBuffer, bufferLength, ">%s", bufferLine1);
            snprintf(bufferLine1, bufferLength, "%s", intermidiateBuffer);
            break;
        case 2:
        case 4:
        case 6:
        case 8:
        case 10:
            snprintf(intermidiateBuffer, bufferLength, ">%s", bufferLine2);
            snprintf(bufferLine2, bufferLength, "%s", intermidiateBuffer);
            break;

        default:
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////  MENU LAYOUT  ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Menu wish list:
- 1. Settings KofH
    - 1.1. Points to win
    - 1.2. Number of teams
    - 1.3. Game duration
    - 1.4. Delayed start time
- 2. Settings Bomb Defusal
    - 2.1. Defusal time
    - 2.2. Game duration
    - 2.3. Delayed start down time
- 3. Save */

// Menu switch structure. Also displays menu to LCD display
void menu::menuDisplay(int &cursorPos, int subM1, int subM2)
{
    static char bufferLine1[BUFFERLENGTH];
    static char bufferLine2[BUFFERLENGTH];

    if (subM1 == 0)
    {
        switch (cursorPos)
        {
        case 1:
        case 2:
            snprintf(bufferLine1, BUFFERLENGTH, "1. Settings KofH");
            snprintf(bufferLine2, BUFFERLENGTH, "2. Settings Bomb");
            break;
        case 3:
        case 4:
            snprintf(bufferLine1, BUFFERLENGTH, "3. Save changes");
            snprintf(bufferLine2, BUFFERLENGTH, "4. Set default");
            break;
        default:
            uint8_t maxlength = 4;
            cursorPos = (cursorPos > maxlength) ? maxlength : 1;
            break;
        }
        menuCursor(cursorPos, BUFFERLENGTH, bufferLine1, bufferLine2);
    }

    // ----------------- Menu 1.x --------------------------------
    else if ((subM1 == 1) && (subM2 == 0))
    {
        switch (cursorPos)
        {
        case 1:
        case 2:
            snprintf(bufferLine1, BUFFERLENGTH, "1.1 PointsToWin");
            snprintf(bufferLine2, BUFFERLENGTH, "1.2 Teams");
            menu_localCounter = s_KofH_numberOfTeams;

            break;
        case 3:
        case 4:
            snprintf(bufferLine1, BUFFERLENGTH, "1.3 Duration");
            snprintf(bufferLine2, BUFFERLENGTH, "1.4 DelayToStart");
            break;
        case 5:
        case 6:
            snprintf(bufferLine1, BUFFERLENGTH, "1.5 -----");
            snprintf(bufferLine2, BUFFERLENGTH, "1.6 -----");
            break;
        default:
            uint8_t maxlength = 6;
            cursorPos = (cursorPos > maxlength) ? maxlength : 1;
            break;
        }
        menuCursor(cursorPos, BUFFERLENGTH, bufferLine1, bufferLine2);
    }
    // ---------- Menu 1.x actions:
    else if ((subM1 == 1) && (subM2 == 1))
    {
        if (last_cursorPos != cursorPos)
        {
            s_KofH_pointsToWin = s_KofH_pointsToWin + (cursorPos > last_cursorPos ? -20 : 20);
        }
        snprintf(bufferLine1, BUFFERLENGTH, "1.1 PointsToWin");
        snprintf(bufferLine2, BUFFERLENGTH, "  > %d", s_KofH_pointsToWin);
    }
    else if ((subM1 == 1) && (subM2 == 2))
    {
        if (last_cursorPos != cursorPos)
        {
            if (menu_localCounter == 2)
            {
                menu_localCounter = 11;
            }
            else if (menu_localCounter == 3)
            {
                menu_localCounter = 17;
            }
            else if (menu_localCounter == 4)
            {
                menu_localCounter = 21;
            }

            menu_localCounter = menu_localCounter + (cursorPos > last_cursorPos ? -1 : 1);

            if (menu_localCounter < 11)
            {
                menu_localCounter = 11; // Minimum 2 teams
                cursorPos--;
            }
            else if (s_KofH_numberOfTeams > 21)
            {
                menu_localCounter = 21; // Maximum 4 teams
                cursorPos++;
            }
            // Set number of teams
            if (menu_localCounter < 17)
            {
                s_KofH_numberOfTeams = 2;
            }
            else if (menu_localCounter >= 17 && menu_localCounter < 21)
            {
                s_KofH_numberOfTeams = 3;
            }
            else if (menu_localCounter >= 21)
            {
                s_KofH_numberOfTeams = 4;
            }

            // Handle active teams
            switch (menu_localCounter)
            {
            case 11:
                s_KofH_activeTeams[0] = true;
                s_KofH_activeTeams[1] = true;
                s_KofH_activeTeams[2] = false;
                s_KofH_activeTeams[3] = false;
                s_KofH_numberOfTeams = 2;
                break;
            case 12:
                s_KofH_activeTeams[0] = true;
                s_KofH_activeTeams[1] = false;
                s_KofH_activeTeams[2] = true;
                s_KofH_activeTeams[3] = false;
                s_KofH_numberOfTeams = 2;
                break;
            case 13:
                s_KofH_activeTeams[0] = true;
                s_KofH_activeTeams[1] = false;
                s_KofH_activeTeams[2] = false;
                s_KofH_activeTeams[3] = true;
                s_KofH_numberOfTeams = 2;
                break;
            case 14:
                s_KofH_activeTeams[0] = false;
                s_KofH_activeTeams[1] = true;
                s_KofH_activeTeams[2] = true;
                s_KofH_activeTeams[3] = false;
                s_KofH_numberOfTeams = 2;
                break;
            case 15:
                s_KofH_activeTeams[0] = false;
                s_KofH_activeTeams[1] = true;
                s_KofH_activeTeams[2] = false;
                s_KofH_activeTeams[3] = true;
                s_KofH_numberOfTeams = 2;
                break;
            case 16:
                s_KofH_activeTeams[0] = false;
                s_KofH_activeTeams[1] = false;
                s_KofH_activeTeams[2] = true;
                s_KofH_activeTeams[3] = true;
                s_KofH_numberOfTeams = 2;
                break;
            case 17:
                s_KofH_activeTeams[0] = true;
                s_KofH_activeTeams[1] = true;
                s_KofH_activeTeams[2] = true;
                s_KofH_activeTeams[3] = false;
                s_KofH_numberOfTeams = 3;
                break;
            case 18:
                s_KofH_activeTeams[0] = true;
                s_KofH_activeTeams[1] = true;
                s_KofH_activeTeams[2] = false;
                s_KofH_activeTeams[3] = true;
                s_KofH_numberOfTeams = 3;
                break;
            case 19:
                s_KofH_activeTeams[0] = true;
                s_KofH_activeTeams[1] = false;
                s_KofH_activeTeams[2] = true;
                s_KofH_activeTeams[3] = true;
                s_KofH_numberOfTeams = 3;
                break;
            case 20:
                s_KofH_activeTeams[0] = false;
                s_KofH_activeTeams[1] = true;
                s_KofH_activeTeams[2] = true;
                s_KofH_activeTeams[3] = true;
                s_KofH_numberOfTeams = 3;
                break;
            case 21:
                s_KofH_activeTeams[0] = true;
                s_KofH_activeTeams[1] = true;
                s_KofH_activeTeams[2] = true;
                s_KofH_activeTeams[3] = true;
                s_KofH_numberOfTeams = 4;
                break;
            
            default:
                break;
            }
        }

        (s_KofH_activeTeams[0]) ? display1.setSegments(customSegment_ON, 4, 0) : display1.setSegments(customSegment_OFF, 4, 0);
        (s_KofH_activeTeams[1]) ? display2.setSegments(customSegment_ON, 4, 0) : display2.setSegments(customSegment_OFF, 4, 0);
        (s_KofH_activeTeams[2]) ? display3.setSegments(customSegment_ON, 4, 0) : display3.setSegments(customSegment_OFF, 4, 0);
        (s_KofH_activeTeams[3]) ? display4.setSegments(customSegment_ON, 4, 0) : display4.setSegments(customSegment_OFF, 4, 0);

        snprintf(bufferLine1, BUFFERLENGTH, "1.2 Teams");
        snprintf(bufferLine2, BUFFERLENGTH, "  > %d", s_KofH_numberOfTeams);
    }
    else if ((subM1 == 1) && (subM2 == 3))
    {
        if (last_cursorPos != cursorPos)
        {
            s_KofH_gameDuration = s_KofH_gameDuration + (cursorPos > last_cursorPos ? -30 : 30);
            if (s_KofH_gameDuration < 0)
            {
                s_KofH_gameDuration = 0; // No negative values
                last_cursorPos = cursorPos;
            }
        }
        snprintf(bufferLine1, BUFFERLENGTH, "1.3 Duration");
        snprintf(bufferLine2, BUFFERLENGTH, "  > %dmin %ds", s_KofH_gameDuration / 60, s_KofH_gameDuration % 60);
    }
    else if ((subM1 == 1) && (subM2 == 4))
    {
        if (last_cursorPos != cursorPos)
        {
            s_KofH_delayedStartTime = s_KofH_delayedStartTime + (cursorPos > last_cursorPos ? -10 : 10);
            if (s_KofH_delayedStartTime < 0)
            {
                s_KofH_delayedStartTime = 0; // No negative values
                last_cursorPos = cursorPos;
            }
        }
        snprintf(bufferLine1, BUFFERLENGTH, "1.4 DelayToStart");
        snprintf(bufferLine2, BUFFERLENGTH, "  > %ds", s_KofH_delayedStartTime);
    }

    // ----------------- Menu 2.x --------------------------------
    else if ((subM1 == 2) && (subM2 == 0))
    {
        switch (cursorPos)
        {
        case 1:
        case 2:
            snprintf(bufferLine1, BUFFERLENGTH, "2.1 GameDuration");
            snprintf(bufferLine2, BUFFERLENGTH, "2.2 BombTimer");
            break;
        case 3:
        case 4:
            snprintf(bufferLine1, BUFFERLENGTH, "2.3 DelayToStart");
            snprintf(bufferLine2, BUFFERLENGTH, "2.4 Arm/Disarm t");
            break;

        default:
            uint8_t maxlength = 4;
            cursorPos = (cursorPos > maxlength) ? maxlength : 1;
            break;
        }
        menuCursor(cursorPos, BUFFERLENGTH, bufferLine1, bufferLine2);
    }
    // ---------- Menu 2.x actions:
    else if ((subM1 == 2) && (subM2 == 1))
    {
        if (last_cursorPos != cursorPos)
        {
            s_armDisarm_gameDuration = s_armDisarm_gameDuration + (cursorPos > last_cursorPos ? -30 : 30);
            if (s_armDisarm_gameDuration < 0)
            {
                s_armDisarm_gameDuration = 0; // No negative values
                last_cursorPos = cursorPos;
            }
        }
        snprintf(bufferLine1, BUFFERLENGTH, "2.1 GameDuration");
        snprintf(bufferLine2, BUFFERLENGTH, "  > %dmin %ds", s_armDisarm_gameDuration / 60, s_armDisarm_gameDuration % 60);
    }
    else if ((subM1 == 2) && (subM2 == 2))
    {
        if (last_cursorPos != cursorPos)
        {
            s_armDisarm_defendTime = s_armDisarm_defendTime + (cursorPos > last_cursorPos ? -30 : 30);
            if (s_armDisarm_defendTime < 0)
            {
                s_armDisarm_defendTime = 0; // No negative values
                last_cursorPos = cursorPos;
            }
        }
        snprintf(bufferLine1, BUFFERLENGTH, "2.2 BombTimer");
        snprintf(bufferLine2, BUFFERLENGTH, "  > %dmin %ds", s_armDisarm_defendTime / 60, s_armDisarm_defendTime % 60);
    }
    else if ((subM1 == 2) && (subM2 == 3))
    {
        if (last_cursorPos != cursorPos)
        {
            s_armDisarm_delayedStartTime = s_armDisarm_delayedStartTime + (cursorPos > last_cursorPos ? -10 : 10);
            if (s_armDisarm_delayedStartTime < 0)
            {
                s_armDisarm_delayedStartTime = 0; // No negative values
                last_cursorPos = cursorPos;
            }
        }
        snprintf(bufferLine1, BUFFERLENGTH, "2.3 DelayToStart");
        snprintf(bufferLine2, BUFFERLENGTH, "  > %ds", s_armDisarm_delayedStartTime);
    }
    else if ((subM1 == 2) && (subM2 == 4))
    {
        if (last_cursorPos != cursorPos)
        {
            s_armDisarm_disarmTime = s_armDisarm_disarmTime + (cursorPos > last_cursorPos ? -1 : 1);
            if (s_armDisarm_disarmTime < 0)
            {
                s_armDisarm_disarmTime = 0; // No negative values
                last_cursorPos = cursorPos;
            }
        }
        snprintf(bufferLine1, BUFFERLENGTH, "Arm/Disarm time");
        snprintf(bufferLine2, BUFFERLENGTH, "  > %ds", s_armDisarm_disarmTime);
    }

    // ----------------- Menu 3.x --------------------------------
    else if ((subM1 == 3) && (subM2 == 0))
    {
        FLAG_forMenu_back = true;
        settings_save();

        snprintf(bufferLine1, BUFFERLENGTH, "Settings saved");
        snprintf(bufferLine2, BUFFERLENGTH, " ");
        FLAG_shortFreeze = true;
    }

    // ----------------- Menu 4.x --------------------------------
    else if ((subM1 == 4) && (subM2 == 0))
    {
        snprintf(bufferLine1, BUFFERLENGTH, "Set all default?"); // Header at line 1
        switch (cursorPos)
        {
        case 1:
            snprintf(bufferLine2, BUFFERLENGTH, "4.1 No"); // Line 2 (Y/N - line)
            break;
        case 2:
            snprintf(bufferLine2, BUFFERLENGTH, "4.2 Yes"); // Line 2 (Y/N - line)
            break;

        default:
            uint8_t maxlength = 2;
            cursorPos = (cursorPos > maxlength) ? maxlength : 1;
            break;
        }
        menuCursor(2, BUFFERLENGTH, bufferLine1, bufferLine2); // Force cursor to be on second line (Y/N - line)
    }

    // ---------- Menu 4.x actions:
    else if ((subM1 == 4) && (subM2 == 1))
    {
        FLAG_forMenu_back = true;
    }
    else if ((subM1 == 4) && (subM2 == 2))
    {
        FLAG_forMenu_back = true;
        EEPROM.write(adr_s_Settings_initiated, 0);
        settings_initiateDefault();

        snprintf(bufferLine1, BUFFERLENGTH, "Default settings");
        snprintf(bufferLine2, BUFFERLENGTH, "saved");
        FLAG_shortFreeze = true;
    }

    if (subM2 != 0)
    {
        // sdsd
    }

    // LCD print
    lcd.setBacklight(155);
    lcd.home();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(bufferLine1);
    lcd.setCursor(0, 1);
    lcd.print(bufferLine2);

    Serial.println(bufferLine1);
    Serial.println(bufferLine2);
    Serial.println("Cursor: " + String(cursorPos) + "Last Cursor: " + String(last_cursorPos) + " SubM1: " + String(subM1) + " SubM2: " + String(subM2));
    Serial.println("");
    if (FLAG_shortFreeze)
    {
        delay(2000);
        FLAG_shortFreeze = false;
    }
}

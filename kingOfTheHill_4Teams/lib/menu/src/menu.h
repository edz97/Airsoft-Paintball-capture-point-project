#ifndef menu_h
#define menu_h

#include "Arduino.h"
#include <menu.h>
#include <settings.h>

class menu
{
public:
    void menuLoop(void);

    // variables for menu navigation
    uint8_t MENU_BUTTON_1; // Corresponds to the "Team 1" button
    uint8_t MENU_BUTTON_2; // Corresponds to the "Team 2" button
    uint8_t MENU_BUTTON_3; // Corresponds to the "Team 3" button
    uint8_t MENU_BUTTON_4; // Corresponds to the "Team 4" button

    uint8_t MENU_BUTTON_ENTER;
    uint8_t MENU_BUTTON_BACK;
    uint8_t MENU_BUTTON_UP;
    uint8_t MENU_BUTTON_DOWN;


private:
    // Function declarations
    void menuEnterBackUpdate(bool &FLAG_enter_true, bool &FLAG_back_true, uint8_t PIN_BUTTON_FOR_ENTER, uint8_t PIN_BUTTON_FOR_BACK);
    void menuCursorUpdate(int &cursorPos, uint8_t PIN_BUTTON_FOR_UP, uint8_t PIN_BUTTON_FOR_DOWN);
    void menuDisplay(int &cursorPos, int subM1, int subM2);
    void menuCursor(int cursorPos, int bufferLength, char *bufferLine1, char *bufferLine2);
};

#endif

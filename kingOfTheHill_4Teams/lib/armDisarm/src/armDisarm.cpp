#include <Arduino.h>
#include <armDisarm.h>
#include <settings.h>
#include <kingOfTheHill.h>

#define PIN_TEAM1_BUTTON A0 // Assume pins 5-8 are set as input.
#define PIN_TEAM2_BUTTON A1
#define PIN_TEAM3_BUTTON A2
#define PIN_TEAM4_BUTTON A3

uint8_t teamColourRGBW[5][4] = {{255, 0, 255, 255}, {055, 165, 255, 255}, {100, 100, 100, 255}, {255, 255, 0, 255}, {0, 255, 255, 255}};
/*
        Neutral, // Green Colour
        Team1, // Yellow
        Team2, // White
        Team3, // Blue
        Team4  // Red
/*

posedo code:

Når gamet starter, skal LED være av.
Timer starter for total spilletid.
En random tallkombinasjon genereres (kode til disarm) med 4 siffer, tallene skal være mellom 1 og 4.

Når en knapp trykkes, skal det sjekkes om det er riktig tall. Hvis det er riktig, skal neste tall vises. Hvis det er feil, skjer ingen ting.
Hvis kombinasjon blir riktig innen 1min, starter bombenedtilling og LED skal blinke gult (pwrsave).

Hvis minst 2 av knappene trykkes inn samtidig, like lenge som "disarm" tiden, desarmeres bomben, og LED lyser blått.
Nedtellingen for bobmen stoppes så lenge knappene holdes inne.
Vis nedtelling på alle 7-segment display.


Settings needed:
int s_armDisarm_delayedStartTime;
unsigned long s_armDisarm_gameDuration;
unsigned long s_armDisarm_defendTime;
int s_armDisarm_disarmTime;

*/

// uint8_t teamColourRGBW[5][4] = {{255, 0, 255, 255}, {055, 165, 255, 255},{100, 100, 100, 255}, {255, 255, 0, 255}, {0, 255, 255, 255}};

// armdisarm::state gameState = armdisarm::unarmed; // Initial state

void armdisarm::reset_armDisarm(void)
{
    // Reset all Var's to start values
    winningTeam = CT;
    gameTimeElapsed = 0;
    FLAG_gameEnded = false;
    gameState = unarmed;
    return;
}

int armdisarm::main_func(void)
{
    FLAG_trueOnceASecond = game::TrueOnceASecond();

    if (FLAG_trueOnceASecond)
    {
        gameTimeElapsed++; // Increment game time elapsed by 1 second
        if (gameState == armed)
        { // Decrement defend time by 1 second if armed
            defendTimeLeft == 0 ? defendTimeLeft = 0 : defendTimeLeft--;
        }
    }

    switch (gameState)
    {
    case unarmed:
        /* Device yet not armed, beginning of game */
        winningTeam = CT;
        // Controll LED Beacon. White normaly (game::Team2), yellow when arming (game::Team1)
        arming ? game::analogWrite_Team_ToExtLED(teamColourRGBW, game::Team1) : game::setExtLed_TeamColour_pwrsave(teamColourRGBW, game::Team2);
        // Check if gametime is reached
        if (gameTimeElapsed >= gameDuration)
        {
            // Game duration reached
            gameState = timeout; // Change state to timeout
            break;
        }
        else
            armByTime(); // Run arming process. Takes care of state change to armed
        break;

    case tobe_armed:
        /* Device armed by TS, transfer-state "unarmed -> armed" */
        // Controll LED Beacon. blue when armed (game::Team3)
        game::analogWrite_Team_ToExtLED(teamColourRGBW, game::Team3);
        // Stay in state for 3 seconds
        static unsigned int delayTime_tobe_armed = 0;
        if (FLAG_trueOnceASecond && (delayTime_tobe_armed < 3))
        {
            delayTime_tobe_armed++;
        }
        else if (delayTime_tobe_armed >= 3)
        {
            // State change to armed
            gameState = armed;
            delayTime_tobe_armed = 0; // Reset delay time
        }
        break;

    case armed:
        /* Device armed by TS, nearing end of game */
        // Handle timeout if "defendTime" is reached
        winningTeam = TS;
        // Controll LED Beacon. blue when armed (game::Team3), yellow when disarming (game::Team1)
        disarming ? game::analogWrite_Team_ToExtLED(teamColourRGBW, game::Team1) : game::setExtLed_TeamColour_pwrsave(teamColourRGBW, game::Team3);
        if (defendTimeLeft == 0 && !disarming)
        {
            // Defend time reached
            gameState = tobe_exploded; // Change state to exploded
            break;
        }
        else
            disarmByTime(); // Run disarming process. Takes care of state change to disarmed
        break;

    case tobe_disarmed:
        /* Device disarmed by CT, transfer-state "armed -> disarmed" */
        // Controll LED Beacon. Green when disarmed (game::Natural)
        game::analogWrite_Team_ToExtLED(teamColourRGBW, game::Neutral);
        // Stay in state for 3 seconds
        static unsigned int delayTime_tobe_disarmed = 0;
        if (FLAG_trueOnceASecond && (delayTime_tobe_disarmed < 3))
        {
            delayTime_tobe_disarmed++;
        }
        else if (delayTime_tobe_disarmed >= 3)
        {
            // State change to armed
            gameState = disarmed;
            delayTime_tobe_disarmed = 0; // Reset delay time
        }
        break;

    case disarmed:
        /* Attackers have disarmed device, and won game */
        // Controll LED Beacon. Green when disarmed (game::Neutral).
        game::setExtLed_TeamColour_pwrsave(teamColourRGBW, game::Neutral);
        winningTeam = CT;
        FLAG_gameEnded = true;
        break;

    case tobe_exploded:
        /* defendTime enden, bomb exploded. Transfer-state "armed -> exploded" */
        // Controll LED Beacon. RED (game::Team4)
        game::analogWrite_Team_ToExtLED(teamColourRGBW, game::Team4);
        // Stay in state for 5 seconds
        static unsigned int delayTime_tobe_exploded = 0;
        if (FLAG_trueOnceASecond && (delayTime_tobe_exploded < 5))
        {
            delayTime_tobe_exploded++;
        }
        else if (delayTime_tobe_exploded >= 5)
        {
            // State change to armed
            gameState = exploded;
            delayTime_tobe_exploded = 0; // Reset delay time
        }
        break;

    case exploded:
        /* Device exploded, TS won */
        // Controll LED Beacon. Red when exploded (game::Team4).
        game::setExtLed_TeamColour_pwrsave(teamColourRGBW, game::Team4);
        winningTeam = TS;
        FLAG_gameEnded = true;
        break;

    case tobe_timeout:
        /* Device timeout, transfer-state "unarmed -> timeout" */
        // Controll LED Beacon. Green when timeout (game::Neutral).
        game::analogWrite_Team_ToExtLED(teamColourRGBW, game::Neutral);
        // Stay in state for 5 seconds
        static unsigned int delayTime_tobe_timeout = 0;
        if (FLAG_trueOnceASecond && (delayTime_tobe_timeout < 5))
        {
            delayTime_tobe_timeout++;
        }
        else if (delayTime_tobe_timeout >= 5)
        {
            // State change to timeout
            gameState = timeout;
            delayTime_tobe_timeout = 0; // Reset delay time
        }
        break;

    case timeout:
        /* Device timeout, CT won */
        // Controll LED Beacon. Green when timeout (game::Neutral).
        game::setExtLed_TeamColour_pwrsave(teamColourRGBW, game::Neutral);
        winningTeam = CT;
        FLAG_gameEnded = true;
        break;

    default:
        break;
    }

    if (FLAG_gameEnded)
        return 1;
    return 0;
}

// Arming process by pressing buttons for certain time
// If armed, func returns true and gameState is set to armed
int armdisarm::armByTime(void)
{
    static bool previosState = false;
    bool currentState = false;

    // Check if any of the buttons are pressed
    int buttonsPressed = digitalRead(PIN_TEAM1_BUTTON) ? 1 : 0;
    buttonsPressed += digitalRead(PIN_TEAM2_BUTTON) ? 1 : 0;
    buttonsPressed += digitalRead(PIN_TEAM3_BUTTON) ? 1 : 0;
    buttonsPressed += digitalRead(PIN_TEAM4_BUTTON) ? 1 : 0;

    if (buttonsPressed >= 2)
    {
        currentState = true; // Update current state
        arming = true;
    }
    else
        arming = false;

    if (gameState == unarmed)
    {
        if (currentState == true && previosState == false)
        {
            // Start timer
            if (FLAG_trueOnceASecond)
            {
                previosState = true;
            }
        }
        else if (currentState == true && previosState == true)
        {
            if (FLAG_trueOnceASecond)
            {
                counter_armByTime--;
            }
            if (counter_armByTime == 0)
            {
                // Buttons pressed for "disarmTime" seconds
                // Disarm the device
                gameState = tobe_armed;
                return true;
            }
        }
        else
        {
            // Less than 2 buttons pressed
            counter_armByTime = armdisarm::armTime;
            previosState = false;
        }
    }
    return false;
}

// Disarm the device if buttons are pressed for "disarmTime" seconds
// If disarmed, func returns true and gameState is set to disarmed
int armdisarm::disarmByTime(void)
{
    static bool previosState = false;
    bool currentState = false;

    // Check if any of the buttons are pressed
    int buttonsPressed = digitalRead(PIN_TEAM1_BUTTON) ? 1 : 0;
    buttonsPressed += digitalRead(PIN_TEAM2_BUTTON) ? 1 : 0;
    buttonsPressed += digitalRead(PIN_TEAM3_BUTTON) ? 1 : 0;
    buttonsPressed += digitalRead(PIN_TEAM4_BUTTON) ? 1 : 0;

    if (buttonsPressed >= 2)
    {
        currentState = true; // Update current state
        disarming = true;
    }
    else
        disarming = false;

    if (gameState == armed)
    {
        if (currentState == true && previosState == false)
        {
            // Start timer
            if (FLAG_trueOnceASecond)
            {
                previosState = true;
            }
        }
        else if (currentState == true && previosState == true)
        {
            if (FLAG_trueOnceASecond)
            {
                counter_disarmByTime--;
            }
            if (counter_disarmByTime == 0)
            {
                // Buttons pressed for "disarmTime" seconds
                // Disarm the device
                gameState = tobe_disarmed;
                return true;
            }
        }
        else
        {
            // Less than 2 buttons pressed
            counter_disarmByTime = armdisarm::disarmTime;
            previosState = false;
        }
    }
    return false;
}

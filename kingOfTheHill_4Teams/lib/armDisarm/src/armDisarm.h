#ifndef armDisarm_h
#define armDisarm_h

#include <Arduino.h>
#include <kingOfTheHill.h>

class armdisarm
{
private:
    int armByTime(void);
    int disarmByTime(void);
    bool FLAG_trueOnceASecond = false;

public:
    enum state
    {
        unarmed,
        tobe_armed,
        armed,
        tobe_disarmed,
        disarmed,
        tobe_exploded,
        exploded,
        tobe_timeout,
        timeout
    };
    enum Teams
    {
        CT, // Team that is attacking, aka trying to disarm the bomb
        TS  // Team that is defending, aka planting bomb and trying to prevent it from being disarmed
    };

    // put function declarations here:
    // static bool TrueOnceASecond(void);
    // static void analogWrite_Team_ToExtLED(uint8_t teamColour_RGBW[5][4], Teams leading_Team);
    // static void setExtLed_TeamColour_pwrsave(uint8_t teamColour_RGBW[5][4], Teams leading_Team);
    // static void digitalWrite_OFF_ToExtLED();
    state gameState = unarmed; // Initial state
    Teams winningTeam;                         // Team that won the game (only valid after game ended)
    unsigned long gameTimeElapsed;             // in seconds
    unsigned long gameDuration = 60;//900;          // in seconds
    unsigned long defendTime = 20;//300;            // seconds from arming to win
    unsigned long defendTimeLeft = defendTime; // in seconds
    unsigned int disarmTime = 5;
    unsigned int counter_disarmByTime = disarmTime;
    bool disarming = false; // true if disarming
    int armTime = 5;        // seconds to disarm
    unsigned int counter_armByTime = armTime;
    bool arming = false;    // true if arming
    bool FLAG_gameEnded = false;
    // settings

    void reset_armDisarm(void);
    int main_func(void);
};

#endif
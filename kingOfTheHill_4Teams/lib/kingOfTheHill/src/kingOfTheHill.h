/*
Some usefull text about this messy function
*/
#ifndef kingOfTheHill_h
#define kingOfTheHill_h

#include "Arduino.h"
#include <kingOfTheHill.h> 

class game
{
public:
    enum Teams
    {
        Neutral, // Green Colour
        Team1, // Yellow
        Team2, // White
        Team3, // Blue
        Team4  // Red
    };

    // put function declarations here:
    static bool TrueOnceASecond(void);
    static void analogWrite_Team_ToExtLED(uint8_t teamColour_RGBW[5][4], Teams leading_Team);
    static void setExtLed_TeamColour_pwrsave(uint8_t teamColour_RGBW[5][4], Teams leading_Team);
    static void digitalWrite_OFF_ToExtLED();

    class KofH
    {
    public:
        Teams winningTeam; // Team that won the game (only valid after game ended)
        Teams leadingTeam;
        Teams inControlOfPoint;        // Team curently gaining points
        unsigned long gameTimeElapsed; // in seconds
        int pointsTeam1;
        int pointsTeam2;
        int pointsTeam3;
        int pointsTeam4;
        bool FLAG_gameEnded = false;
        // settings
        unsigned long gameDuration = 900; // in seconds
        int pointsToWin = 600;            // one point per second
        uint8_t numberOfTeams = 4;
        bool activeTeams[4] = {true, true, true, true};
        // settings_KofH.TeamColourRGBW first index: Team(neut,1,2,3,4), second index: colour(R, G, B, W) with value 0-255
        uint8_t teamColourRGBW[5][4] = {{255, 0, 255, 255}, {055, 165, 255, 255},{100, 100, 100, 255}, {255, 255, 0, 255}, {0, 255, 255, 255}};
        void reset_KofH(void);
        int kingOfTheHill(void);
    };
};

#endif

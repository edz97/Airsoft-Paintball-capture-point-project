#ifndef settings_h
#define settings_h

#include "Arduino.h"
#include <settings.h>



    void settings_save();
    void settings_read();
    void settings_initiateDefault();

    // Variables for settings
    extern uint8_t s_Settings_initiated; // value 111 if settings initiated at least once.
    // KofH
    extern  int s_KofH_pointsToWin;
    extern int s_KofH_numberOfTeams;
    extern bool s_KofH_activeTeams[4];
    extern int s_KofH_gameDuration;
    extern int s_KofH_delayedStartTime;
    // ArmDisarm
    extern int s_armDisarm_delayedStartTime;
    extern unsigned long s_armDisarm_gameDuration;
    extern unsigned long s_armDisarm_defendTime;
    extern int s_armDisarm_disarmTime;


    // Adresses for EEPROM
    const int adr_s_Settings_initiated = 0;
    // (int has same size as 2 bytes, therefore 4 adresses are needed for each int)
    // KofH
    const int adr_s_KofH_pointsToWin = 1;
    const int adr_s_KofH_numberOfTeams = 3;
    const int adr_s_KofH_gameDuration = 5;
    const int adr_s_KofH_delayedStartTime = 7;
    const int adr_s_KofH_activeTeams = 9; // 4 bytes length, is array of 4 bools.
    // ArmDisarm
    const int adr_s_armDisarm_delayedStartTime = 13;
    const int adr_s_armDisarm_gameDuration = 15; // 4 bytes length, unsigned long
    const int adr_s_armDisarm_defendTime = 19;  // 4 bytes length, unsigned long
    const int adr_s_armDisarm_disarmTime = 23;


#endif
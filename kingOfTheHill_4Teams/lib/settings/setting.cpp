#include "Arduino.h"
#include <settings.h>
#include <EEPROM.h>

#define SETTINGS_INITIATED 111

// Variable definitions
uint8_t s_Settings_initiated;
// KofH
int s_KofH_pointsToWin;
int s_KofH_numberOfTeams;
bool s_KofH_activeTeams[4];
int s_KofH_gameDuration;
int s_KofH_delayedStartTime;
// ArmDisarm
int s_armDisarm_delayedStartTime;
unsigned long s_armDisarm_gameDuration;
unsigned long s_armDisarm_defendTime;
int s_armDisarm_disarmTime;

// Read setings from EEPROM
void settings_read()
{
    EEPROM.get(adr_s_KofH_delayedStartTime, s_KofH_delayedStartTime);
    EEPROM.get(adr_s_KofH_gameDuration, s_KofH_gameDuration);
    EEPROM.get(adr_s_KofH_numberOfTeams, s_KofH_numberOfTeams);
    EEPROM.get(adr_s_KofH_pointsToWin, s_KofH_pointsToWin);
    EEPROM.get(adr_s_KofH_activeTeams, s_KofH_activeTeams);
    
    // arm disarm settings
    EEPROM.get(adr_s_armDisarm_delayedStartTime, s_armDisarm_delayedStartTime);
    EEPROM.get(adr_s_armDisarm_gameDuration, s_armDisarm_gameDuration);
    EEPROM.get(adr_s_armDisarm_defendTime, s_armDisarm_defendTime);
    EEPROM.get(adr_s_armDisarm_disarmTime, s_armDisarm_disarmTime);
}

// Save settings to EEPROM
void settings_save()
{
    EEPROM.put(adr_s_KofH_delayedStartTime, s_KofH_delayedStartTime);
    EEPROM.put(adr_s_KofH_gameDuration, s_KofH_gameDuration);
    EEPROM.put(adr_s_KofH_numberOfTeams, s_KofH_numberOfTeams);
    EEPROM.put(adr_s_KofH_pointsToWin, s_KofH_pointsToWin);
    EEPROM.put(adr_s_KofH_activeTeams, s_KofH_activeTeams);

    // arm disarm settings
    EEPROM.put(adr_s_armDisarm_delayedStartTime, s_armDisarm_delayedStartTime);
    EEPROM.put(adr_s_armDisarm_gameDuration, s_armDisarm_gameDuration);
    EEPROM.put(adr_s_armDisarm_defendTime, s_armDisarm_defendTime);
    EEPROM.put(adr_s_armDisarm_disarmTime, s_armDisarm_disarmTime);
}

void settings_initiateDefault()
{
    if (EEPROM.read(adr_s_Settings_initiated) != SETTINGS_INITIATED)
    {
        // KofH settings
        s_KofH_delayedStartTime = 5;
        s_KofH_gameDuration = 2400;
        s_KofH_numberOfTeams = 4;
        s_KofH_activeTeams[0] = true;
        s_KofH_activeTeams[1] = true;
        s_KofH_activeTeams[2] = true;
        s_KofH_activeTeams[3] = true;
        s_KofH_pointsToWin = 2400;

        // arm disarm settings
        s_armDisarm_delayedStartTime = 5;
        s_armDisarm_gameDuration = 2400;
        s_armDisarm_defendTime = 300;
        s_armDisarm_disarmTime = 5;

        settings_save();
        EEPROM.write(adr_s_Settings_initiated, SETTINGS_INITIATED);
    }
}
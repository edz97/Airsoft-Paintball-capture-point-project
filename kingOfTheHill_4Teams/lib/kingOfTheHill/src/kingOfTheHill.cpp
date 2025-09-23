#include <Arduino.h>
#include <kingOfTheHill.h>
#include <settings.h>

#define PIN_EXT_LED_R 4 // Assume pins 1-4 are set as output.
#define PIN_EXT_LED_G 5
#define PIN_EXT_LED_B 6
#define PIN_EXT_LED_W 7

#define PIN_TEAM1_BUTTON A0 // Assume pins 5-8 are set as input.
#define PIN_TEAM2_BUTTON A1
#define PIN_TEAM3_BUTTON A2
#define PIN_TEAM4_BUTTON A3


/* extern uint8_t PIN_EXT_LED_R;
extern uint8_t PIN_EXT_LED_G;
extern uint8_t PIN_EXT_LED_B;
extern uint8_t PIN_EXT_LED_W;

extern uint8_t PIN_TEAM1_BUTTON;
extern uint8_t PIN_TEAM2_BUTTON;
extern uint8_t PIN_TEAM3_BUTTON;
extern uint8_t PIN_TEAM4_BUTTON;
extern uint8_t PIN_STOP_BUTTON; */

/*
Settings needed to be pased to game-function kingOgTheHill():

- Maximum game time before it ends unconditionaly:
- Maximum points needed to win game:
- Amount of teams (2/3 maybe more):

Return Values from function:

- Winner-Team:
- Points for each team (-1 if team did not participate?):
- Game duration:
*/

void game::KofH::reset_KofH()
{
    // Reset all Var's to start values
    winningTeam = Neutral;
    inControlOfPoint = Neutral;
    leadingTeam = Neutral;
    pointsTeam1 = 0;
    pointsTeam2 = 0;
    pointsTeam3 = 0;
    pointsTeam4 = 0;
    numberOfTeams = numberOfTeams < 2 ? 2 : numberOfTeams; // Restrain number of teams to minimum of 2
    numberOfTeams = numberOfTeams > 4 ? 4 : numberOfTeams; // Restrain number of teams to maximum of 4
    gameTimeElapsed = 0;
    FLAG_gameEnded = false;
}

int game::KofH::kingOfTheHill(void)
{

    // put your main code here, to run repeatedly:
    // update variable(flag), as to only use FLAG_trueOnceASecond() once for each loop
    bool FLAG_trueOnceASecond = TrueOnceASecond();

    /* if (Serial.available() > 0)
    {
        // read the incoming byte:
        KofH.inControlOfPoint = Serial.read() == 1 ? Team1 : Team2;
    } */

    if (!FLAG_gameEnded)
    {
        // Read Button inputs and set KofH.inControlOfPoint accordingly
        if ((digitalRead(PIN_TEAM1_BUTTON) == HIGH) && activeTeams[0])
        {
            inControlOfPoint = Team1;
            analogWrite_Team_ToExtLED(teamColourRGBW, inControlOfPoint);
            // Serial.println("Team1");
        }
        if ((digitalRead(PIN_TEAM2_BUTTON) == HIGH) && activeTeams[1])
        {
            inControlOfPoint = Team2;
            analogWrite_Team_ToExtLED(teamColourRGBW, inControlOfPoint);
            // Serial.println("Team2");
        }
        if ((digitalRead(PIN_TEAM3_BUTTON) == HIGH) && activeTeams[2])
        {
            inControlOfPoint = Team3;
            analogWrite_Team_ToExtLED(teamColourRGBW, inControlOfPoint);
            // Serial.println("Team3");
        }
        if ((digitalRead(PIN_TEAM4_BUTTON) == HIGH) && activeTeams[3])
        {
            inControlOfPoint = Team4;
            analogWrite_Team_ToExtLED(teamColourRGBW, inControlOfPoint);
            // Serial.println("Team4");
        }
    }

    // Set LED-strip to team colour
    setExtLed_TeamColour_pwrsave(teamColourRGBW, inControlOfPoint);

    if (!FLAG_gameEnded && FLAG_trueOnceASecond)
    {
        // Counting game duration:
        gameTimeElapsed++;
        // Counting points for team:
        switch (inControlOfPoint)
        {
        case Neutral:
            // Do nothing for now
            break;
        case Team1:
            pointsTeam1++;
            break;
        case Team2:
            pointsTeam2++;
            break;
        case Team3:
            pointsTeam3++;
            break;
        case Team4:
            pointsTeam4++;
        default:
            break;
        }
    }

    // Print game status to serial monitor
    if (FLAG_trueOnceASecond && !FLAG_gameEnded)
    {
        Serial.print("Leading: [");
        Serial.print(leadingTeam);
        Serial.print("] ");
        Serial.print("\tTeam1: ");
        Serial.print(pointsTeam1);
        Serial.print("\tTeam2: ");
        Serial.print(pointsTeam2);
        Serial.print("\tTeam3: ");
        Serial.print(pointsTeam3);
        Serial.print("\tTeam4: ");
        Serial.print(pointsTeam4);
        Serial.print("\tTime: ");
        Serial.print(gameTimeElapsed);
        Serial.print("s");
        Serial.print("   Max: ");
        Serial.print(gameDuration);
        Serial.print("s, ");
        Serial.print(pointsToWin);
        Serial.print("p");
        Serial.println();
    }

    // Update which team is in the lead:
    if ((pointsTeam1 > pointsTeam2) && (pointsTeam1 > pointsTeam3) && (pointsTeam1 > pointsTeam4))
        leadingTeam = Team1;
    else if ((pointsTeam2 > pointsTeam1) && (pointsTeam2 > pointsTeam3) && (pointsTeam2 > pointsTeam4))
        leadingTeam = Team2;
    else if ((pointsTeam3 > pointsTeam1) && (pointsTeam3 > pointsTeam2) && (pointsTeam3 > pointsTeam4))
        leadingTeam = Team3;
    else if ((pointsTeam4 > pointsTeam1) && (pointsTeam4 > pointsTeam2) && (pointsTeam4 > pointsTeam3))
        leadingTeam = Team4;

    // Check if team has won (by points or time) and set FLAG_gameEnded to true:
    if (((pointsTeam1 >= pointsToWin) || (pointsTeam2 >= pointsToWin) || gameTimeElapsed > gameDuration) && !FLAG_gameEnded)
    {
        winningTeam = leadingTeam;
        // Print winning team to serial monitor
        char buffer[100];
        if (winningTeam != Neutral)
            snprintf(buffer, 100, "Congratulations to Team %s!! You won the round.", (winningTeam == Team1) ? "1" : (winningTeam == Team2) ? "2"
                                                                                                                : (winningTeam == Team3)   ? "3"
                                                                                                                                           : "4");
        else
            snprintf(buffer, 100, "Too bad, no winning team this time");
        Serial.println(buffer);

        analogWrite_Team_ToExtLED(teamColourRGBW, winningTeam);
        // Set KofH.FLAG_gameEnded to true
        FLAG_gameEnded = 1;
    }
    if (FLAG_gameEnded)
        return 1;

    return 0;
}
// --------------------------------------------------------------------------------------------
// put function definitions here:

// Returns true once every second
bool game::TrueOnceASecond(void)
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

// Sets colour of LED-strip to team colour
void game::setExtLed_TeamColour_pwrsave(uint8_t teamColour_RGBW[5][4], Teams leading_Team)
{
    static unsigned long previousMillis = 0;
    unsigned long currentMillis;
    currentMillis = millis();
    static bool FLAG1 = 0;
    static bool FLAG2 = 0;
    static bool FLAG3 = 0;
    static bool FLAG4 = 0;
    if (currentMillis - previousMillis == 0 && !FLAG1)
    {
        // Durration 300ms
        analogWrite_Team_ToExtLED(teamColour_RGBW, leading_Team);
        FLAG1 = 1;
    }
    if (currentMillis - previousMillis >= 300 && !FLAG2)
    {
        // Durrations 300ms
        digitalWrite_OFF_ToExtLED();
        FLAG2 = 1;
    }
    if (currentMillis - previousMillis >= 600 && !FLAG3)
    {
        // Durration 300ms
        analogWrite_Team_ToExtLED(teamColour_RGBW, leading_Team);
        FLAG3 = 1;
    }
    if (currentMillis - previousMillis >= 900 && !FLAG4)
    {
        // Durration 600ms
        digitalWrite_OFF_ToExtLED();
        FLAG4 = 1;
    }
    if (currentMillis - previousMillis >= 1500)
    {
        previousMillis = currentMillis;
        FLAG1 = 0;
        FLAG2 = 0;
        FLAG3 = 0;
        FLAG4 = 0;
    }
}

// Set LED-strip colour to team colour
void game::analogWrite_Team_ToExtLED(uint8_t teamColour_RGBW[5][4], Teams team)
{

    /* digitalWrite(PIN_EXT_LED_R, teamColour_RGBW[team][0]); // Red
    digitalWrite(PIN_EXT_LED_G, teamColour_RGBW[team][1]); // Green
    digitalWrite(PIN_EXT_LED_B, teamColour_RGBW[team][2]); // Blue
    digitalWrite(PIN_EXT_LED_W, teamColour_RGBW[team][3]); // White */

    analogWrite(PIN_EXT_LED_R, teamColour_RGBW[team][0]); // Red
    analogWrite(PIN_EXT_LED_G, teamColour_RGBW[team][1]); // Green
    analogWrite(PIN_EXT_LED_B, teamColour_RGBW[team][2]); // Blue
                                                          // analogWrite(PIN_EXT_LED_W, teamColour_RGBW[team][3]); // White
    return;
}

// Set LED-strip off
void game::digitalWrite_OFF_ToExtLED()
{
    digitalWrite(PIN_EXT_LED_R, HIGH); // Red
    digitalWrite(PIN_EXT_LED_G, HIGH); // Green
    digitalWrite(PIN_EXT_LED_B, HIGH); // Blue
    digitalWrite(PIN_EXT_LED_W, HIGH); // White
    return;
}

// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------
// Legacy code below

/* void game::setExtLed_TeamColour_pwrsave(uint8_t teamColour_RGBW[5][4], Teams leading_Team)
{
    static unsigned long previousMillis = 0;
    unsigned long currentMillis;
    currentMillis = millis();
    if (currentMillis - previousMillis >= 0)
    {
        // Durration 200ms
        analogWrite_Team_ToExtLED(teamColour_RGBW, leading_Team);
    }
    if (currentMillis - previousMillis >= 200)
    {
        // Durrations 200ms
        digitalWrite_OFF_ToExtLED();
    }
    if (currentMillis - previousMillis >= 400)
    {
        // Durration 200ms
        analogWrite_Team_ToExtLED(teamColour_RGBW, leading_Team);
    }
    if (currentMillis - previousMillis >= 600)
    {
        // Durration 400ms
        digitalWrite_OFF_ToExtLED();
    }
    if (currentMillis - previousMillis >= 1000)
    {
        previousMillis = currentMillis;
    }
} */
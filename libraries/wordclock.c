#include <libraries/F28069M.h>
#include <libraries/wordclock.h>

/*    char *words_text[] = { "It ", "is ", "five ", "ten ", "quarter ", "twenty ",
*                           "five ", "half ", "minutes ", "past ", "to ", "one ",
*                           "two ", "three ", "four ", "five ", "six ", "seven ",
*                           "eight ", "nine ", "ten ", "eleven ", "twelve ",
*                           "o'clock" };
*/

void WordClock_Init()
{
    int i = 0;
    for (i = 0; i <= 23; ++i)
    {
        digitalWrite(word_pinouts[i], HIGH); // Set all LED's to HIGH (Pins are sinking)
        pinMode(word_pinouts[i], OUTPUT);    // Set all LED's to OUTPUT mode
    }
}

void setAllPinsState(int state, int delay)
{
    int i = 0;
    for (i = 0; i <= 23; ++i)
    {
        digitalWrite(word_pinouts[i], state);
        DELAY_US(delay);
    }
}

void WordClock_CheckLEDS()
{   // Short LED check (~0.5 seconds)
    setAllPinsState(LOW, 30000);
    setAllPinsState(HIGH, 30000);
    DELAY_US(100000);
    setAllPinsState(LOW, 0);
    DELAY_US(200000);
    setAllPinsState(HIGH, 0);
    DELAY_US(200000);
    setAllPinsState(LOW, 0);
    DELAY_US(200000);
    setAllPinsState(HIGH, 0);
}

void WordClock_CheckTime(int seconds)
{
    int hour = 1;
    for (hour = 1; hour <= 12; ++hour)
    {
        int min = 0;
        for (min = 0; min <= 55; min += 5)
        {
            WordClock_setWordPins(hour, min, 0);
            DELAY_US(1000000 * seconds);
        }
    }
}

void setPins(int pinStates[])
{
    setAllPins(HIGH, 0);
    int i = 0;
    for (i = 0; i <= 23; ++i)
    {
        digitalWrite(word_pinouts[i], pinStates[i]);
    }
}

void WordClock_setWordPins(int hour, int minute, int second)
{
    int words[] = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                    1, 1, 1, 0 };

    int minutes_arr[] = { 0, 2, 3, 4, 5, 5, 7, 5, 5, 4, 3, 2 };

    // Divide minutes by 5, as the only changes happen in increments of 5.
    minute = minute / 5;
    if (minute == 0 || minute == 3 || minute == 6 || minute == 9)
    {

    }
    else
    { // If minute does not equal 0, 15, 30, or 45, turn on the word "minutes"
        words[MINUTES] = 0;
    }

    // If minute equals zero, set the hour.
    if (minute == 0)
        words[PAST + hour] = 0;
    else if (minute <= 6)
    { // If minute is less than or equal 30, turn on the word "past" and set the hour
        words[PAST] = 0;
        words[PAST + hour] = 0;
    }
    else if (minute > 6)
    { // If minute is more than 30, turn on word "to"
        words[TO] = 0;
        if (hour == 12){
            words[ONE] = 0; // If time is 12, set "ONE" to on
        } else {
            words[PAST + hour + 1] = 0; // Set hour + 1
        }
    }

    if (minute != 0){
        words[minutes_arr[minute]] = 0; // Set word for the current minute
    }

    if (minute == 5 || minute == 7)
    { // If minute is 25 or 35
        words[FIVE2] = 0;
    }
    setPins(words);
}

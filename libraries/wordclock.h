/*
 * wordclock.h
 *
 *  Created on: Nov 2, 2022
 *      Author: julian
 */

#ifndef LIBRARIES_WORDCLOCK_H_
#define LIBRARIES_WORDCLOCK_H_

typedef enum
{
    LED_IT = 50,
    LED_IS = 49,
    LED_FIVE = 48,
    LED_TEN = 47,
    LED_QUARTER = 45,
    LED_TWENTY = 10,
    LED_FIVE2 = 9,
    LED_HALF = 8,
    LED_MINUTES = 5,
    LED_TO = 18,
    LED_PAST = 19,
    LED_ONE = 75,
    LED_TWO = 76,
    LED_THREE = 77,
    LED_FOUR = 78,
    LED_FIVE3 = 79,
    LED_SIX = 80,
    LED_SEVEN = 59,
    LED_EIGHT = 11,
    LED_NINE = 12,
    LED_TEN2 = 14,
    LED_ELEVEN = 34,
    LED_TWELVE = 38,
    LED_OCLOCK = 40
} LEDS;


static int word_pinouts[] =
{
    LED_IT,
    LED_IS,
    LED_FIVE,
    LED_TEN,
    LED_QUARTER,
    LED_TWENTY,
    LED_FIVE2,
    LED_HALF,
    LED_MINUTES,
    LED_TO,
    LED_PAST,
    LED_ONE,
    LED_TWO,
    LED_THREE,
    LED_FOUR,
    LED_FIVE3,
    LED_SIX,
    LED_SEVEN,
    LED_EIGHT,
    LED_NINE,
    LED_TEN2,
    LED_ELEVEN,
    LED_TWELVE,
    LED_OCLOCK
};

typedef enum
{
    IT = 0,
    IS = 1,
    FIVE = 2,
    TEN = 3,
    QUARTER = 4,
    TWENTY = 5,
    FIVE2 = 6,
    HALF = 7,
    MINUTES = 8,
    TO = 9,
    PAST = 10,
    ONE = 11,
    TWO = 12,
    THREE = 13,
    FOUR = 14,
    FIVE3 = 15,
    SIX = 16,
    SEVEN = 17,
    EIGHT = 18,
    NINE = 19,
    TEN2 = 20,
    ELEVEN = 21,
    TWELVE = 22,
    OCLOCK = 23
} WORDS;

void WordClock_Init();
void WordClock_CheckLEDS();
void WordClock_CheckTime(int seconds);

void WordClock_printTime(int h, int m, int s);
void WordClock_printWordTime(int h, int m, int s);

void WordClock_setWordPins(int h, int m, int s);

#endif /* WORDCLOCK_H_ */

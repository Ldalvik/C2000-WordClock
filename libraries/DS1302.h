#ifndef LIBRARIES_DS1302_H_
#define LIBRARIES_DS1302_H_

#include "includes.h"

/*
 * Day of the week map for the DAY register values
 */
typedef enum
{
    Sunday = 1,
    Monday = 2,
    Tuesday = 3,
    Wednesday = 4,
    Thursday = 5,
    Friday = 6,
    Saturday = 7,

    RamSize = 32
} constants;


typedef enum
{
    SecondsReg = 0,
    MinutesReg = 1,
    HoursReg = 2,
    DateReg = 3,
    MonthReg = 4,
    DayReg = 5,
    YearReg = 6,
    WriteProtectReg = 7,
    TrickleChargerReg = 8,
    RamAddress0 = 32
} registers;

typedef enum
{
    ClockBurstRead = 0xBF,
    ClockBurstWrite = 0xBE,
    RamBurstRead = 0xFF,
    RamBurstWrite = 0xFE,
    EnableClockHalt = 0x40,
} commands;

/*
 * void DS1302_Init();
 *      Function where user can put initialization code, specifically
 *      setting the Chip Enable GPIO pin.
 */
void DS1302_Init();

/*
 * void DS1302_writeProtect(int);
 *      Set or clear the RTC Write Protect bit. When set to true,
 *      write operations will not be allowed (besides to this register).
 *
 *      Set this to false before doing any other write operations to the chip.
 */
void DS1302_writeProtect(int state);

/*
 * void DS1302_clockHalt(int);
 *      Set or clear the RTC Clock Halt bit. When set to true, the clock will not oscillate.
 *
 *      Set this to false to start the clock.
 *
 *
 *      returns: void
 */
void DS1302_clockHalt(int state);

/*
 * void DS1302_setTrickleCharger(Uint8);
 *      Set the trickle charger power mode. These are the modes and their value:
 *              "" = 0xa6
 *              "" = 0x
 *              "" = 0x
 *              "" = 0x
 *
 *
 *      returns: void
 */
void DS1302_setTrickleCharger();

/*
 * void DS1302_setTime(Uint16, Uint8, Uint8, Uint8, Uint8, Uint8, Uint8);
 *      Set the time for the RTC to use. Once both the clock halt bit and
 *      all 7 time registers are set (seconds, minutes, hour, date, month, day, year),
 *      it will begin to keep time.
 *
 *
 *      returns: void
 */
void DS1302_setTime(const uint16_t hour, const uint16_t minute, const uint16_t second,
             const uint16_t date,
             const uint16_t month, const uint16_t day, const uint16_t year);

/*
 * int * DS1302_getTime();
 *      Get the current time that is being kept on the RTC. An array of integers
 *      is returned so you can access them via their index.
 *          0 = seconds
 *          1 = minutes
 *          2 = hour
 *          3 = date
 *          4 = month
 *          5 = day
 *          6 = year
 *
 *
 *      returns: int*
 */
int * DS1302_getTime();


void writeRam(const uint16_t address, const uint16_t data);
uint16_t readRam(const uint16_t address);
void writeRamBulk(const uint16_t *const data);
uint16_t * readRamBulk(int length);
void clearRam();

#endif  // DS1302_H_

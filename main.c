#include <libraries/DS1302.h>
#include <libraries/F28069M.h>
#include <libraries/wordclock.h>
#include <includes.h>

int HOUR   = 3;
int MINUTE = 59;
int SECOND = 50;
int DAY    = Tuesday;
int MONTH  = 12;
int DATE   = 10;
int YEAR   = 2022;

uint32_t RTC_BAUDRATE = (uint32_t) ((LSPCLK_HZ / 500000) - 1); // RTC Baudrate

uint16_t second, minute, hour, date, month, day, year = 0;

//int * words = {0,0,0};

void setup(void)
{
   /*  Initialize the SPI peripheral with the configuration needed to communicate
       with the DS1302 RTC. */

   /*  Commands are 8 bits long */

   /*  The baudrate is determined by the speed of the clock and the speed of the crystal.
    *  The F28069M has a low speed clock of 22,500,000 HZ. The crystal used for the DS1302 runs
    *  at 500,000 HZ while at the minimum voltage of 2v. 22,500,000 / 500,000 = 45.
    *  The baud rate must be set to a minimum of 45 - 1 = 44.
    */

    /* It receives data on the rising edge, and sends data back on the falling edge,
       so the clock cycle will be RISING EDGE WITH DELAY. */

    /* Data must be transmitted in MASTER_MODE */

    /* The DS1302 uses 3-wire SPI (chip select, data, clock) to send and
     * receive data between itself and the controller, so enabling TRIWIRE
     * mode will save you an extra pin.
     */

    SpiA_Init(8,              // Char length = 8
              RTC_BAUDRATE,   // Baudrate = (22,500,000 / 500,000) - 1 = 44
              RISING_EDGE,    // Rising edge
              W_DELAY,        // Delayed
              MASTER_MODE,    // Master mode
              TRIWIRE_MODE);  // Tri-wire mode

    DS1302_Init();

    pinMode(13, INPUT);

    if(digitalRead(13)){ // Set time ONLY when pin reads HIGH
        DS1302_setTime(HOUR, MINUTE, SECOND, DATE, MONTH, DAY, YEAR);
    }

    WordClock_Init();


    WordClock_CheckLEDS();
    //WordClock_CheckTime(4);
}

void loop(void)
{

    int *time = DS1302_getTime();

    second = time[0];
    minute = time[1];
    hour   = time[2];
    date   = time[3];
    month  = time[4];
    day    = time[5];
    year   = time[6];

    WordClock_setWordPins(hour, minute, second);
}

void main(void)
{
    DeviceSetup(setup, loop);
}

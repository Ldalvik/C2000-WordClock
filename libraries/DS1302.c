/* DS1302 RTC Library made for the TI C2000 F2806x Microprocessor
 * Files needed:
 *      DS1302.c (current)
 *      DS1302.h
 *      F28069M.c
 *      F28069M.h
 */
#include <libraries/DS1302.h>
#include <libraries/F28069M.h>

// Set the GPIO pin that will control the DS1302 Chip Enable (CE) pin.
#define CE_ON  GpioDataRegs.GPASET.bit.GPIO1   = 1;
#define CE_OFF GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;

void DS1302_writeProtect(int state);
void DS1302_clockHalt(int state);

void DS1302_Init()
{
    EALLOW;
    GpioCtrlRegs.GPADIR.bit.GPIO1   = 1;  // Set GPIO 1 pin as OUTPUT
    GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;  // Set GPIO 1 pin LOW
    EDIS;

    DS1302_writeProtect(false);
    DS1302_clockHalt(false);
}

// Returns the binary-coded decimal of 'dec'. Inverse of bcdToDec.
uint16_t DecToBcd(const uint16_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

// Returns the decoded decimal value from a binary-coded decimal (BCD) byte.
uint16_t BcdToDec(const uint16_t bcd)
{
    return (10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

// Flips a binary number
uint16_t __flip(uint16_t bin)
{
    return (bin * 0x0202020202ULL & 0x010884422010ULL) % 1023;
}

// Returns the hour in 24-hour format from the hour register value.
uint16_t getHourFromValue(const uint16_t value)
{
    uint16_t adj = 10 * ((value & (32 + 16)) >> 4);     // 24 hour mode
    if (value & 128) adj = 12 * ((value & 32) >> 5);    // 12 hour mode
    return (value & 15) + adj;
}

void send(uint16_t command, uint16_t data)
{
    command = __flip(command);
    data    = __flip(data);

    CE_ON;
    SpiA_Write(command);  // Write command
    SpiA_Write(data);     // Write data
    CE_OFF;
}

void write(uint16_t data)
{
    data    = __flip(data);
    SpiA_Write(data);    // Write data
}


uint16_t read()
{
   return __flip(SpiA_Read());
}

uint16_t readRegister(uint16_t _register)
{
    uint16_t data = 0;

    CE_ON;
    write(0x81 | _register);   // Send a read register command
    data = read();             // Receive bits
    CE_OFF;
    return data;
}

void writeRegister(uint16_t _register, uint16_t data)
{   // Send a write register command and the corresponding data
    send(0x80 | (_register << 1), data);
}

void DS1302_clockHalt(int state)
{   // Set/clear clock halt bit
    writeRegister(SecondsReg, (state << 7));
}

void DS1302_writeProtect(int state)
{   // Set/clear write protect bit
    writeRegister(WriteProtectReg, (state << 7));
}

// DO NOT USE WITH NON-RECHARGEABLE BATTERIES!!! Un-comment to use
void DS1302_setTrickleCharger(uint16_t mode)
{   // Set trickle charger mode
    //writeRegister(TrickleChargerReg, mode);
}

void DS1302_setTime(const uint16_t hour, const uint16_t minute, const uint16_t second,
             const uint16_t date, const uint16_t month, const uint16_t day,
             const uint16_t year)
{
    CE_ON;
    write(ClockBurstWrite);        // Enable clock burst mode for write operations
    write(DecToBcd(second));       // Write to seconds register
    write(DecToBcd(minute));       // Write to minutes register
    write(DecToBcd(hour));         // Write to hour register
    write(DecToBcd(date));         // Write to date register
    write(DecToBcd(month));        // Write to month register
    write(DecToBcd(day));          // Write to day register
    write(DecToBcd(year - 2000));  // Write to year register
    write(0x00);                   // Clear write protection bit
    CE_OFF;
}

int * DS1302_getTime()
{
    static int clock[7];

    CE_ON;
    write(ClockBurstRead);                // Enable clock burst for read operations
    clock[0] = BcdToDec(read());          // Read seconds from register
    clock[1] = BcdToDec(read());          // Read minutes from register
    clock[2] = getHourFromValue(read());  // Read hour from register
    clock[3] = BcdToDec(read());          // Read date from register
    clock[4] = BcdToDec(read());          // Read month from register
    clock[5] = BcdToDec(read());          // Read day from register
    clock[6] = 2000 + BcdToDec(read());   // Read year from register
    CE_OFF;
    return clock;
}


// Work in progress //

void writeRam(const uint16_t address, const uint16_t data)
{
    if (address >= RamSize) return;
    writeRegister(RamAddress0 + address, data);
}

uint16_t readRam(const uint16_t address)
{
    if (address >= RamSize) return 0;
    return readRegister(RamAddress0 + address);
}

void writeRamBulk(const uint16_t *const data)
{
    int length = sizeof(data) - 1;

    if (length <= 0)      return;
    if (length > RamSize) length = RamSize;

    write(RamBurstWrite);
    int i;
    for (i = 0; i < length; ++i) write(data[i]);
}

uint16_t * readRamBulk(int length)
{
    uint16_t * data;
    if (length <= 0)      return 0;
    if (length > RamSize) length = RamSize;
    write(RamBurstRead);
    int i;
    for (i = 0; i < length; ++i) data[i] = read();
    return data;
}

void clearRam()
{
    int i;
    for (i = 0; i < 32; ++i) writeRam(i, 0x00);
}

void DS1302_Test() {
    // set time
    // check all times
    // wait 3 seconds
    // check if time = set time + 3 seconds
    // test complete
}


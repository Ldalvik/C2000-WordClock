#include <libraries/F28069M.h>

// Import Interrupt registers
extern volatile unsigned int IFR;
extern volatile unsigned int IER;

// Declare init functions
void PieCntlInit(void);
void PieVectTableInit(void);

// Import device flashing functions
void DeviceInit(Uint16 clock_source, Uint16 pllDiv);
void MemCopy(Uint16 *SourceAddr, Uint16 *SourceEndAddr, Uint16 *DestAddr);
void InitFlash();
extern Uint16 RamfuncsLoadStart, RamfuncsLoadEnd, RamfuncsRunStart;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  USEABLE GPIO PINS (Ordered by PIN)
 *
 * ~~~~~~~~ J1 ~~~~~~~~~
 *  PIN 1-4     N/A
 *  PIN 5       GPIO 12
 *  PIN 6       N/A
 *  PIN 7       GPIO 18
 *  PIN 8       GPIO 22
 *  PIN 9       GPIO 33
 *  PIN 10      GPIO 32
 * ~~~~~~~~~~~~~~~~~~~~~


 * ~~~~~~~~ J2 ~~~~~~~~~
 *  PIN 11      GPIO 55
 *  PIN 12      GPIO 51
 *  PIN 13      GPIO 50
 *  PIN 14      GPIO 17
 *  PIN 15      GPIO 16
 *  PIN 16-17   N/A
 *  PIN 18      GPIO 44
 *  PIN 19      GPIO 19
 *  PIN 20      N/A
 * ~~~~~~~~~~~~~~~~~~~~~


 * ~~~~~~~~ J3 ~~~~~~~~~
 *  PIN 21-30   N/A
 * ~~~~~~~~~~~~~~~~~~~~~


 * ~~~~~~~~ J4 ~~~~~~~~~
 *  PIN 31-33   N/A
 *  PIN 34      GPIO 13
 *  PIN 35      GPIO 5
 *  PIN 36      GPIO 4
 *  PIN 37      GPIO 3
 *  PIN 38      GPIO 2
 *  PIN 39      GPIO 1
 *  PIN 40      GPIO 0
 * ~~~~~~~~~~~~~~~~~~~~~


 * ~~~~~~~~ J5 ~~~~~~~~~
 *  PIN 41-44   N/A
 *  PIN 45      GPIO 20
 *  PIN 46      N/A
 *  PIN 47      GPIO 14
 *  PIN 48      GPIO 21
 *  PIN 49      GPIO 23
 *  PIN 50      GPIO 54
 * ~~~~~~~~~~~~~~~~~~~~~


 * ~~~~~~~~ J6 ~~~~~~~~~
 *  PIN 51      GPIO 56
 *  PIN 52      GPIO 53
 *  PIN 53      GPIO 52
 *  PIN 54      GPIO 25
 *  PIN 55      GPIO 24
 *  PIN 56-57   N/A
 *  PIN 58      GPIO 26
 *  PIN 59      GPIO 27
 *  PIN 60      N/A
 * ~~~~~~~~~~~~~~~~~~~~~


 * ~~~~~~~~ J7 ~~~~~~~~~
 *  PIN 61-70   N/A
 * ~~~~~~~~~~~~~~~~~~~~~


 * ~~~~~~~~ J8 ~~~~~~~~~
 *  PIN 71-74   N/A
 *  PIN 75      GPIO 11
 *  PIN 76      GPIO 10
 *  PIN 77      GPIO 9
 *  PIN 78      GPIO 8
 *  PIN 79      GPIO 7
 *  PIN 80      GPIO 6
 *  PIN 81      GPIO 34 (Red LED)
 *  PIN 82      GPIO 39 (Blue LED)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  USEABLE GPIO PINS (Ordered by GPIO)
 *
 *  GPIO 0       PIN 40
 *  GPIO 1       PIN 39
 *  GPIO 2       PIN 38
 *  GPIO 3       PIN 37
 *  GPIO 4       PIN 36
 *  GPIO 5       PIN 35
 *  GPIO 6       PIN 80
 *  GPIO 7       PIN 79
 *  GPIO 8       PIN 78
 *  GPIO 9       PIN 77
 *  GPIO 10      PIN 76
 *  GPIO 11      PIN 75
 *  GPIO 12      PIN 5
 *  GPIO 13      PIN 34
 *  GPIO 14      PIN 47
 *  GPIO 15      N/A
 *  GPIO 16      PIN 15
 *  GPIO 17      PIN 14
 *  GPIO 18      PIN 7
 *  GPIO 19      PIN 19
 *  GPIO 20      PIN 45
 *  GPIO 21      PIN 48
 *  GPIO 22      PIN 8
 *  GPIO 23      PIN 49
 *  GPIO 24      PIN 55
 *  GPIO 25      PIN 54
 *  GPIO 26      PIN 58
 *  GPIO 27      PIN 59
 *  GPIO 28-31   N/A
 *  GPIO 32      PIN 10
 *  GPIO 33      PIN 9
 *  GPIO 34      PIN 81
 *  GPIO 35-38   N/A
 *  GPIO 39      PIN 82
 *  GPIO 40-43   N/A
 *  GPIO 44      PIN 18
 *  GPIO 45-49   N/A
 *  GPIO 50      PIN 13
 *  GPIO 51      PIN 12
 *  GPIO 52      PIN 53
 *  GPIO 53      PIN 52
 *  GPIO 54      PIN 50
 *  GPIO 55      PIN 11
 *  GPIO 56      PIN 51
 *  GPIO 57-58   N/A
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * UN-USEABLE GPIO PINS
 * GPIO15  // UART
 * GPIO28  // UART
 * GPIO29  // UART
 * GPIO30  // CAN RX-A
 * GPIO31  // CAN TX-A
 * GPIO35  // TDI (JTAG Test Data Input)
 * GPIO36  // TMS (JTAG Test Mode Select)
 * GPIO37  // TDO (JTAG scan out, Test Data Output)
 * GPIO38  // TCK (JTAG Test Clock)
 * GPIO40  // PWM DAC1
 * GPIO41  // PWM DAC2
 * GPIO42  // PWM DAC3
 * GPIO43  // PWM DAC4
 * GPIO57  // SPISTE-A ???
 * GPIO58  // UART
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


// Device initialization
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Initialize and flash the device,
 * then run the setup and infinite loop.
 *
 * void setup(void) -> setup method, runs once
 * void loop(void)  -> loop function, runs infinitely
 */
void DeviceSetup(void setup(void), void loop(void))
{
    DeviceInit(PLL_SRC, PLL_DIV);
#ifdef _FLASH
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    InitFlash();
#endif

    setup();
    for (;;)
    {
        loop();
    }
}

// Interrupts
 /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Setup device for interrupts
 *
 * Initializes the needed functions and configurations
 * to use interrupts.
 *
 * no params
 */
void InitInterrupts(void)
{
    //DINT;        // Disable interrupts (This is done by PieCntlInit())
    IER = 0x0000;  // Clear interrupt enable register
    IFR = 0x0000;  // Clear interrupt flag register

    PieCntlInit();      // Initialize the PIE control registers to a known state
    PieVectTableInit(); // Initialize the PIE vector table

    EINT; // Clear flag to enable interrupts
    ERTM; // Enable real-time mode
    //PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}


/* Attach an interrupt to the board. Will automatically assign an interrupt based on the open CpuTimers.
 *
 * clearInterval(id) must be used in the interrupt to acknowledge it.
 * The ID will need to be the order in which the interrupts are activated,
 * so make sure to keep track of its count with a variable.
 *
 * void (*i)        -> the interrupt function
 * int TICKS_PER_MS -> Times the interrupt will be run each clock (1000 = runs every second)
 */
void attachInterrupt(void (*i), int TICKS_PER_MS)
{
    if (CpuTimer0Regs.TCR.bit.TIE == 0)
    {
        EALLOW;
        SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1;  // Enable CpuTimer0
        EDIS;

        CpuTimer0Regs.PRD.all = (uint32_t) (SYSCLK_HZ / (1000 * TICKS_PER_MS)); // Set clocking speed

        CpuTimer0Regs.TCR.bit.TIE = 1;     // Set CpuTimer0 interrupt enable flag
        PieCtrlRegs.PIEIER1.bit.INTx7 = 1; // Only CpuTimer0 uses this?

        PieCtrlRegs.PIEACK.bit.ACK1 = PIEACK_GROUP1; // Only CpuTimer0 uses the PIE and needs to be acknowledged

        EALLOW;
        PieVectTable.TINT0 = (PINT) i; // Set the address of the interrupt to the timer 0
        EDIS;
        IER |= M_INT1; // Set interrupt enable flag, CpuTimer0 only uses INT1-INT12

        printf("Timer0 Interrupt started\n");
    }
    else if (CpuTimer1Regs.TCR.bit.TIE == 0)
    {
        EALLOW;
        SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1;  // Enable CpuTimer1
        EDIS;
        CpuTimer1Regs.PRD.all = (uint32_t) (SYSCLK_HZ / (1000 * TICKS_PER_MS)); // Set clocking speed

        CpuTimer1Regs.TCR.bit.TIE = 1; // Set CpuTimer1 interrupt enable flag

        EALLOW;
        PieVectTable.TINT1 = (PINT) i; // Set the address of the interrupt to the timer 1
        EDIS;

        IER |= M_INT13; // Set the interrupt enable flag, CpuTimer1 uses INT13

        printf("Timer1 Interrupt started\n");
    }
    else if (CpuTimer2Regs.TCR.bit.TIE == 0)
    {
        EALLOW;
        SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1; // Enable CpuTimer2
        EDIS;
        CpuTimer2Regs.PRD.all = (uint32_t) (SYSCLK_HZ / (1000 * TICKS_PER_MS)); // Set clocking speed

        CpuTimer2Regs.TCR.bit.TIE = 1; // Set CpuTimer2 interrupt enable flag

        EALLOW;
        PieVectTable.TINT2 = (PINT) i; // Set address of interrupt to timer 2
        EDIS;
        IER |= M_INT14; // Set the interrupt enable flag, CpuTimer2 uses INT14

        printf("Timer2 Interrupt started\n");
    }
    else
    {
        printf("All CpuTimers are being used\n");
    }
}

/* Clear an interrupt. Used in an interrupt being passed to attachInterrupt, the ID must match the order in
 * which the interrupts are activated.
 *
 * int id -> ID of the CpuTimer the interrupt uses.
 */
void clearInterrupt(int id)
{
    switch (id)
    {
    case 0:
    {
        PieCtrlRegs.PIEACK.bit.ACK1 = PIEACK_GROUP1; // Only CpuTimer0 uses the PIE and needs to be acknowledged
        CpuTimer0Regs.TCR.bit.TIF = 1;               // Set the interrupt flag for CpuTimer0
        IER |= M_INT1;                               // Set the interrupt enable register
    }
        break;
    case 1:
    {
        CpuTimer1Regs.TCR.bit.TIF = 1; // Set the interrupt flag for CpuTimer1
        IER |= M_INT13;                // Set the interrupt enable register
    }
        break;
    case 2:
    {
        CpuTimer2Regs.TCR.bit.TIF = 1; // Set the interrupt flag for CpuTimer2
        IER |= M_INT14;                // Set the interrupt enable register
    }
        break;
    default:
        printf("Unknown ID: %d\n", id);
    }
}
/*
 * Stop an interrupt. The CpuTimer corresponding the the ID will be disabled.
 *
 * int id -> ID of the CpuTimer the interrupt uses.
 */

void stopInterrupt(int id)
{
    /* if (IER >> (M_INT1 - 1) == 1) IER ^= 1UL << M_INT1;
     * Works, but doesn't disable interrupt on a high enough level
     */
    switch (id)
    {
    case 0:
    {
        if (CpuTimer0Regs.TCR.bit.TIE)
        {
            CpuTimer0Regs.TCR.bit.TIE = 0; // Clear CpuTimer0 interrupt enable flag
            printf("Timer0 Interrupt stopped\n");
        }
        else
        {
            printf("No interrupt on Timer0\n");
        }
    }
        break;
    case 1:
    {
        if (CpuTimer1Regs.TCR.bit.TIE)
        {
            CpuTimer1Regs.TCR.bit.TIE = 0; // Clear CpuTimer1 interrupt enable flag
            printf("Timer1 Interrupt stopped\n");
        }
        else
        {
            printf("No interrupt on Timer1\n");
        }
    }
        break;
    case 2:
    {
        if (CpuTimer2Regs.TCR.bit.TIE)
        {
            CpuTimer2Regs.TCR.bit.TIE = 0; // Clear CpuTimer1 interrupt enable flag
            printf("Timer1 Interrupt stopped\n");
        }
        else
        {
            printf("No interrupt on Timer2\n");
        }
    }
        break;
    default:
        printf("Unknown ID: %d\n", id);
    }
}


// GPIO pins
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Toggle GPIO pins
 * Sets GPIO pins to INPUT or OUTPUT
 *
 * int pin -> number of the physical pins on the board
 */
void togglePin(int pin)
{
    EALLOW;
    switch (pin)
    {
    case 40:
        GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1;
        break;
    case 39:
        GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1;
        break;
    case 38:
        GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1;
        break;
    case 37:
        GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1;
        break;
    case 36:
        GpioDataRegs.GPATOGGLE.bit.GPIO4 = 1;
        break;
    case 35:
        GpioDataRegs.GPATOGGLE.bit.GPIO5 = 1;
        break;
    case 80:
        GpioDataRegs.GPATOGGLE.bit.GPIO6 = 1;
        break;
    case 79:
        GpioDataRegs.GPATOGGLE.bit.GPIO7 = 1;
        break;
    case 78:
        GpioDataRegs.GPATOGGLE.bit.GPIO8 = 1;
        break;
    case 77:
        GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;
        break;
    case 76:
        GpioDataRegs.GPATOGGLE.bit.GPIO10 = 1;
        break;
    case 75:
        GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;
        break;
    case 5:
        GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;
        break;
    case 34:
        GpioDataRegs.GPATOGGLE.bit.GPIO13 = 1;
        break;
    case 47:
        GpioDataRegs.GPATOGGLE.bit.GPIO14 = 1;
        break;
    case 15:
        GpioDataRegs.GPATOGGLE.bit.GPIO16 = 1;
        break;

    case 14:
        GpioDataRegs.GPATOGGLE.bit.GPIO17 = 1;
        break;

    case 7:
        GpioDataRegs.GPATOGGLE.bit.GPIO18 = 1;
        break;

    case 19:
        GpioDataRegs.GPATOGGLE.bit.GPIO19 = 1;
        break;

    case 45:
        GpioDataRegs.GPATOGGLE.bit.GPIO20 = 1;
        break;

    case 48:
        GpioDataRegs.GPATOGGLE.bit.GPIO21 = 1;
        break;

    case 8:
        GpioDataRegs.GPATOGGLE.bit.GPIO22 = 1;
        break;

    case 49:
        GpioDataRegs.GPATOGGLE.bit.GPIO23 = 1;
        break;

    case 55:
        GpioDataRegs.GPATOGGLE.bit.GPIO24 = 1;
        break;

    case 54:
        GpioDataRegs.GPATOGGLE.bit.GPIO25 = 1;
        break;

    case 58:
        GpioDataRegs.GPATOGGLE.bit.GPIO26 = 1;
        break;

    case 59:
        GpioDataRegs.GPATOGGLE.bit.GPIO27 = 1;
        break;
    case 10:
        GpioDataRegs.GPBTOGGLE.bit.GPIO32 = 1;
        break;
    case 9:
        GpioDataRegs.GPBTOGGLE.bit.GPIO33 = 1;
        break;
    case 81:
        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
        break;
    case 82:
        GpioDataRegs.GPBTOGGLE.bit.GPIO39 = 1;
        break;
    case 18:
        GpioDataRegs.GPBTOGGLE.bit.GPIO44 = 1;
        break;
    case 13:
        GpioDataRegs.GPBTOGGLE.bit.GPIO50 = 1;
        break;
    case 12:
        GpioDataRegs.GPBTOGGLE.bit.GPIO51 = 1;
        break;
    case 53:
        GpioDataRegs.GPBTOGGLE.bit.GPIO52 = 1;
        break;
    case 52:
        GpioDataRegs.GPBTOGGLE.bit.GPIO53 = 1;
        break;
    case 50:
        GpioDataRegs.GPBTOGGLE.bit.GPIO54 = 1;
        break;
    case 11:
        GpioDataRegs.GPBTOGGLE.bit.GPIO55 = 1;
        break;
    case 51:
        GpioDataRegs.GPBTOGGLE.bit.GPIO56 = 1;
        break;
    }
    EDIS;
}

/* Write digital value to GPIO pin
 * Drives the pin logic to 0 (LOW) or 1 (HIGH)
 *
 * int pin   -> number of the physical pin on the board
 * int state -> pin logic, write 0 for LOW and 1 for HIGH
 */
void digitalWrite(int pin, int state)
{
    EALLOW;
    if (state)
    {
        switch (pin)
        {
        case 40:
            GpioDataRegs.GPASET.bit.GPIO0 = 1;
            break;
        case 39:
            GpioDataRegs.GPASET.bit.GPIO1 = 1;
            break;
        case 38:
            GpioDataRegs.GPASET.bit.GPIO2 = 1;
            break;
        case 37:
            GpioDataRegs.GPASET.bit.GPIO3 = 1;
            break;
        case 36:
            GpioDataRegs.GPASET.bit.GPIO4 = 1;
            break;
        case 35:
            GpioDataRegs.GPASET.bit.GPIO5 = 1;
            break;
        case 80:
            GpioDataRegs.GPASET.bit.GPIO6 = 1;
            break;
        case 79:
            GpioDataRegs.GPASET.bit.GPIO7 = 1;
            break;
        case 78:
            GpioDataRegs.GPASET.bit.GPIO8 = 1;
            break;
        case 77:
            GpioDataRegs.GPASET.bit.GPIO9 = 1;
            break;
        case 76:
            GpioDataRegs.GPASET.bit.GPIO10 = 1;
            break;
        case 75:
            GpioDataRegs.GPASET.bit.GPIO11 = 1;
            break;
        case 5:
            GpioDataRegs.GPASET.bit.GPIO12 = 1;
            break;
        case 34:
            GpioDataRegs.GPASET.bit.GPIO13 = 1;
            break;
        case 47:
            GpioDataRegs.GPASET.bit.GPIO14 = 1;
            break;
        case 15:
            GpioDataRegs.GPASET.bit.GPIO16 = 1;
            break;
        case 14:
            GpioDataRegs.GPASET.bit.GPIO17 = 1;
            break;
        case 7:
            GpioDataRegs.GPASET.bit.GPIO18 = 1;
            break;
        case 19:
            GpioDataRegs.GPASET.bit.GPIO19 = 1;
            break;
        case 45:
            GpioDataRegs.GPASET.bit.GPIO20 = 1;
            break;
        case 48:
            GpioDataRegs.GPASET.bit.GPIO21 = 1;
            break;
        case 8:
            GpioDataRegs.GPASET.bit.GPIO22 = 1;
            break;
        case 49:
            GpioDataRegs.GPASET.bit.GPIO23 = 1;
            break;
        case 55:
            GpioDataRegs.GPASET.bit.GPIO24 = 1;
            break;
        case 54:
            GpioDataRegs.GPASET.bit.GPIO25 = 1;
            break;
        case 58:
            GpioDataRegs.GPASET.bit.GPIO26 = 1;
            break;
        case 59:
            GpioDataRegs.GPASET.bit.GPIO27 = 1;
            break;
        case 10:
            GpioDataRegs.GPBSET.bit.GPIO32 = 1;
            break;
        case 9:
            GpioDataRegs.GPBSET.bit.GPIO33 = 1;
            break;
        case 81:
            GpioDataRegs.GPBSET.bit.GPIO34 = 1;
            break;
        case 82:
            GpioDataRegs.GPBSET.bit.GPIO39 = 1;
            break;
        case 18:
            GpioDataRegs.GPBSET.bit.GPIO44 = 1;
            break;
        case 13:
            GpioDataRegs.GPBSET.bit.GPIO50 = 1;
            break;
        case 12:
            GpioDataRegs.GPBSET.bit.GPIO51 = 1;
            break;
        case 53:
            GpioDataRegs.GPBSET.bit.GPIO52 = 1;
            break;
        case 52:
            GpioDataRegs.GPBSET.bit.GPIO53 = 1;
            break;
        case 50:
            GpioDataRegs.GPBSET.bit.GPIO54 = 1;
            break;
        case 11:
            GpioDataRegs.GPBSET.bit.GPIO55 = 1;
            break;
        case 51:
            GpioDataRegs.GPBSET.bit.GPIO56 = 1;
            break;
        }
    }
    else
    {
        switch (pin)
        {
        case 40:
            GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
            break;
        case 39:
            GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
            break;
        case 38:
            GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
            break;
        case 37:
            GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
            break;
        case 36:
            GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
            break;
        case 35:
            GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
            break;
        case 80:
            GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
            break;
        case 79:
            GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
            break;
        case 78:
            GpioDataRegs.GPACLEAR.bit.GPIO8 = 1;
            break;
        case 77:
            GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;
            break;
        case 76:
            GpioDataRegs.GPACLEAR.bit.GPIO10 = 1;
            break;
        case 75:
            GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;
            break;
        case 5:
            GpioDataRegs.GPACLEAR.bit.GPIO12 = 1;
            break;
        case 34:
            GpioDataRegs.GPACLEAR.bit.GPIO13 = 1;
            break;
        case 47:
            GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;
            break;
        case 15:
            GpioDataRegs.GPACLEAR.bit.GPIO16 = 1;
            break;
        case 14:
            GpioDataRegs.GPACLEAR.bit.GPIO17 = 1;
            break;
        case 7:
            GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;
            break;
        case 19:
            GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;
            break;
        case 45:
            GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;
            break;
        case 48:
            GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;
            break;
        case 8:
            GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;
            break;
        case 49:
            GpioDataRegs.GPACLEAR.bit.GPIO23 = 1;
            break;
        case 55:
            GpioDataRegs.GPACLEAR.bit.GPIO24 = 1;
            break;
        case 54:
            GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;
            break;
        case 58:
            GpioDataRegs.GPACLEAR.bit.GPIO26 = 1;
            break;
        case 59:
            GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;
            break;
        case 10:
            GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;
            break;
        case 9:
            GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;
            break;
        case 81:
            GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
            break;
        case 82:
            GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;
            break;
        case 18:
            GpioDataRegs.GPBCLEAR.bit.GPIO44 = 1;
            break;
        case 13:
            GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
            break;
        case 12:
            GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;
            break;
        case 53:
            GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;
            break;
        case 52:
            GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;
            break;
        case 50:
            GpioDataRegs.GPBCLEAR.bit.GPIO54 = 1;
            break;
        case 11:
            GpioDataRegs.GPBCLEAR.bit.GPIO55 = 1;
            break;
        case 51:
            GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;
            break;
        }
    }
    EDIS;
}

/* Read digital value of GPIO pin
 *
 * Returns the current logic value of the pin.
 *
 * int pin -> number of the physical pin on the board
 * returns: logic state of pin
 */
int digitalRead(int pin)
{
    int value;
    switch (pin)
    {
    case 40:
        value = GpioDataRegs.GPADAT.bit.GPIO0;
        break;
    case 39:
        value = GpioDataRegs.GPADAT.bit.GPIO1;
        break;
    case 38:
        value = GpioDataRegs.GPADAT.bit.GPIO2;
        break;
    case 37:
        value = GpioDataRegs.GPADAT.bit.GPIO3;
        break;
    case 36:
        value = GpioDataRegs.GPADAT.bit.GPIO4;
        break;
    case 35:
        value = GpioDataRegs.GPADAT.bit.GPIO5;
        break;
    case 80:
        value = GpioDataRegs.GPADAT.bit.GPIO6;
        break;
    case 79:
        value = GpioDataRegs.GPADAT.bit.GPIO7;
        break;
    case 78:
        value = GpioDataRegs.GPADAT.bit.GPIO8;
        break;
    case 77:
        value = GpioDataRegs.GPADAT.bit.GPIO9;
        break;
    case 76:
        value = GpioDataRegs.GPADAT.bit.GPIO10;
        break;
    case 75:
        value = GpioDataRegs.GPADAT.bit.GPIO11;
        break;
    case 5:
        value = GpioDataRegs.GPADAT.bit.GPIO12;
        break;
    case 34:
        value = GpioDataRegs.GPADAT.bit.GPIO13;
        break;
    case 47:
        value = GpioDataRegs.GPADAT.bit.GPIO14;
        break;
    case 15:
        value = GpioDataRegs.GPADAT.bit.GPIO16;
        break;
    case 14:
        value = GpioDataRegs.GPADAT.bit.GPIO17;
        break;
    case 7:
        value = GpioDataRegs.GPADAT.bit.GPIO18;
        break;
    case 19:
        value = GpioDataRegs.GPADAT.bit.GPIO19;
        break;
    case 45:
        value = GpioDataRegs.GPADAT.bit.GPIO20;
        break;
    case 48:
        value = GpioDataRegs.GPADAT.bit.GPIO21;
        break;
    case 8:
        value = GpioDataRegs.GPADAT.bit.GPIO22;
        break;
    case 49:
        value = GpioDataRegs.GPADAT.bit.GPIO23;
        break;
    case 55:
        value = GpioDataRegs.GPADAT.bit.GPIO24;
        break;
    case 54:
        value = GpioDataRegs.GPADAT.bit.GPIO25;
        break;
    case 58:
        value = GpioDataRegs.GPADAT.bit.GPIO26;
        break;
    case 59:
        value = GpioDataRegs.GPADAT.bit.GPIO27;
        break;
    case 10:
        value = GpioDataRegs.GPBDAT.bit.GPIO32;
        break;
    case 9:
        value = GpioDataRegs.GPBDAT.bit.GPIO33;
        break;
    case 82:
        value = GpioDataRegs.GPBDAT.bit.GPIO39;
        break;
    case 18:
        value = GpioDataRegs.GPBDAT.bit.GPIO44;
        break;
    case 13:
        value = GpioDataRegs.GPBDAT.bit.GPIO50;
        break;
    case 12:
        value = GpioDataRegs.GPBDAT.bit.GPIO51;
        break;
    case 53:
        value = GpioDataRegs.GPBDAT.bit.GPIO52;
        break;
    case 52:
        value = GpioDataRegs.GPBDAT.bit.GPIO53;
        break;
    case 50:
        value = GpioDataRegs.GPBDAT.bit.GPIO54;
        break;
    case 11:
        value = GpioDataRegs.GPBDAT.bit.GPIO55;
        break;
    case 51:
        value = GpioDataRegs.GPBDAT.bit.GPIO56;
        break;
    }
    return value;
}

/* Change GPIO pin mode
 *
 * Sets the GPIO pin as an INPUT or OUTPUT
 *
 * int pin  -> number of the physical pin on the board
 * int mode -> pin mode, 0 for INPUT and 1 for OUTPUT
 */
void pinMode(int pin, int mode)
{
    EALLOW;
    switch (pin)
    {
    case 40:
        GpioCtrlRegs.GPADIR.bit.GPIO0 = mode;
        break;
    case 39:
        GpioCtrlRegs.GPADIR.bit.GPIO1 = mode;
        break;
    case 38:
        GpioCtrlRegs.GPADIR.bit.GPIO2 = mode;
        break;
    case 37:
        GpioCtrlRegs.GPADIR.bit.GPIO3 = mode;
        break;
    case 36:
        GpioCtrlRegs.GPADIR.bit.GPIO4 = mode;
        break;
    case 35:
        GpioCtrlRegs.GPADIR.bit.GPIO5 = mode;
        break;
    case 80:
        GpioCtrlRegs.GPADIR.bit.GPIO6 = mode;
        break;
    case 79:
        GpioCtrlRegs.GPADIR.bit.GPIO7 = mode;
        break;
    case 78:
        GpioCtrlRegs.GPADIR.bit.GPIO8 = mode;
        break;
    case 77:
        GpioCtrlRegs.GPADIR.bit.GPIO9 = mode;
        break;
    case 76:
        GpioCtrlRegs.GPADIR.bit.GPIO10 = mode;
        break;
    case 75:
        GpioCtrlRegs.GPADIR.bit.GPIO11 = mode;
        break;
    case 5:
        GpioCtrlRegs.GPADIR.bit.GPIO12 = mode;
        break;
    case 34:
        GpioCtrlRegs.GPADIR.bit.GPIO13 = mode;
        break;
    case 47:
        GpioCtrlRegs.GPADIR.bit.GPIO14 = mode;
        break;
    case 15:
        GpioCtrlRegs.GPADIR.bit.GPIO16 = mode;
        break;
    case 14:
        GpioCtrlRegs.GPADIR.bit.GPIO17 = mode;
        break;
    case 7:
        GpioCtrlRegs.GPADIR.bit.GPIO18 = mode;
        break;
    case 19:
        GpioCtrlRegs.GPADIR.bit.GPIO19 = mode;
        break;
    case 45:
        GpioCtrlRegs.GPADIR.bit.GPIO20 = mode;
        break;
    case 48:
        GpioCtrlRegs.GPADIR.bit.GPIO21 = mode;
        break;
    case 8:
        GpioCtrlRegs.GPADIR.bit.GPIO22 = mode;
        break;
    case 49:
        GpioCtrlRegs.GPADIR.bit.GPIO23 = mode;
        break;
    case 55:
        GpioCtrlRegs.GPADIR.bit.GPIO24 = mode;
        break;
    case 54:
        GpioCtrlRegs.GPADIR.bit.GPIO25 = mode;
        break;
    case 58:
        GpioCtrlRegs.GPADIR.bit.GPIO26 = mode;
        break;
    case 59:
        GpioCtrlRegs.GPADIR.bit.GPIO27 = mode;
        break;
    case 10:
        GpioCtrlRegs.GPBDIR.bit.GPIO32 = mode;
        break;
    case 9:
        GpioCtrlRegs.GPBDIR.bit.GPIO33 = mode;
        break;
    case 81:
        GpioCtrlRegs.GPBDIR.bit.GPIO34 = mode;
        break;
    case 82:
        GpioCtrlRegs.GPBDIR.bit.GPIO39 = mode;
        break;
    case 18:
        GpioCtrlRegs.GPBDIR.bit.GPIO44 = mode;
        break;
    case 13:
        GpioCtrlRegs.GPBDIR.bit.GPIO50 = mode;
        break;
    case 12:
        GpioCtrlRegs.GPBDIR.bit.GPIO51 = mode;
        break;
    case 53:
        GpioCtrlRegs.GPBDIR.bit.GPIO52 = mode;
        break;
    case 52:
        GpioCtrlRegs.GPBDIR.bit.GPIO53 = mode;
        break;
    case 50:
        GpioCtrlRegs.GPBDIR.bit.GPIO54 = mode;
        break;
    case 11:
        GpioCtrlRegs.GPBDIR.bit.GPIO55 = mode;
        break;
    case 51:
        GpioCtrlRegs.GPBDIR.bit.GPIO56 = mode;
        break;
    }
    EDIS;
}

/* Change GPIO MUX mode
 *
 * Sets the GPIO pins multiplex mode.
 *
 * int pin  -> number of the physical pin on the board
 * int mode -> multiplex mode, can only be 0 (default), 1, 2, or 3
 */
void setMux(int pin, int mode)
{
    EALLOW;
    switch (pin)
    {
    case 40:
        GpioCtrlRegs.GPAMUX1.bit.GPIO0 = mode;
        break;
    case 39:
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = mode;
        break;
    case 38:
        GpioCtrlRegs.GPAMUX1.bit.GPIO2 = mode;
        break;
    case 37:
        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = mode;
        break;
    case 36:
        GpioCtrlRegs.GPAMUX1.bit.GPIO4 = mode;
        break;
    case 35:
        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = mode;
        break;
    case 80:
        GpioCtrlRegs.GPAMUX1.bit.GPIO6 = mode;
        break;
    case 79:
        GpioCtrlRegs.GPAMUX1.bit.GPIO7 = mode;
        break;
    case 78:
        GpioCtrlRegs.GPAMUX1.bit.GPIO8 = mode;
        break;
    case 77:
        GpioCtrlRegs.GPAMUX1.bit.GPIO9 = mode;
        break;
    case 76:
        GpioCtrlRegs.GPAMUX1.bit.GPIO10 = mode;
        break;
    case 75:
        GpioCtrlRegs.GPAMUX1.bit.GPIO11 = mode;
        break;
    case 5:
        GpioCtrlRegs.GPAMUX1.bit.GPIO12 = mode;
        break;
    case 34:
        GpioCtrlRegs.GPAMUX1.bit.GPIO13 = mode;
        break;
    case 47:
        GpioCtrlRegs.GPAMUX1.bit.GPIO14 = mode;
        break;
    case 15:
        GpioCtrlRegs.GPAMUX2.bit.GPIO16 = mode;
        break;
    case 14:
        GpioCtrlRegs.GPAMUX2.bit.GPIO17 = mode;
        break;
    case 7:
        GpioCtrlRegs.GPAMUX2.bit.GPIO18 = mode;
        break;
    case 19:
        GpioCtrlRegs.GPAMUX2.bit.GPIO19 = mode;
        break;
    case 45:
        GpioCtrlRegs.GPAMUX2.bit.GPIO20 = mode;
        break;
    case 48:
        GpioCtrlRegs.GPAMUX2.bit.GPIO21 = mode;
        break;
    case 8:
        GpioCtrlRegs.GPAMUX2.bit.GPIO22 = mode;
        break;
    case 49:
        GpioCtrlRegs.GPAMUX2.bit.GPIO23 = mode;
        break;
    case 55:
        GpioCtrlRegs.GPAMUX2.bit.GPIO24 = mode;
        break;
    case 54:
        GpioCtrlRegs.GPAMUX2.bit.GPIO25 = mode;
        break;
    case 58:
        GpioCtrlRegs.GPAMUX2.bit.GPIO26 = mode;
        break;
    case 59:
        GpioCtrlRegs.GPAMUX2.bit.GPIO27 = mode;
        break;
    case 10:
        GpioCtrlRegs.GPBMUX1.bit.GPIO32 = mode;
        break;
    case 9:
        GpioCtrlRegs.GPBMUX1.bit.GPIO33 = mode;
        break;
    case 81:
        GpioCtrlRegs.GPBMUX1.bit.GPIO34 = mode;
        break;
    case 82:
        GpioCtrlRegs.GPBMUX1.bit.GPIO39 = mode;
        break;
    case 18:
        GpioCtrlRegs.GPBMUX1.bit.GPIO44 = mode;
        break;
    case 13:
        GpioCtrlRegs.GPBMUX2.bit.GPIO50 = mode;
        break;
    case 12:
        GpioCtrlRegs.GPBMUX2.bit.GPIO51 = mode;
        break;
    case 53:
        GpioCtrlRegs.GPBMUX2.bit.GPIO52 = mode;
        break;
    case 52:
        GpioCtrlRegs.GPBMUX2.bit.GPIO53 = mode;
        break;
    case 50:
        GpioCtrlRegs.GPBMUX2.bit.GPIO54 = mode;
        break;
    case 11:
        GpioCtrlRegs.GPBMUX2.bit.GPIO55 = mode;
        break;
    case 51:
        GpioCtrlRegs.GPBMUX2.bit.GPIO56 = mode;
        break;
    }
    EDIS;
}

// SPI Peripheral
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
 *
 */


/* Initialize the SPI-A bus.
 *
 * Initializes the SPI-A bus with specified settings
 *
 * int charLength  ->
 * int baudRate    ->
 * int clockParity ->
 * int clockPhase  ->
 * int spiMode     ->
 * int isTriWire   ->
 *
 */
void SpiA_Init(int charLength, int baudRate, int clockPolarity, int clockPhase, int spiMode, int isTriWire)
{
    /* SPI BUS-A SETUP
     * GPIO pins 16, 18, and 19 (pins 15, 7, 19)
     */
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK       = 1;                // Enable SPI-A peripheral clock
    GpioCtrlRegs.GPAMUX2.bit.GPIO16         = 1;                // Set GPIO16 (pin 15) to SPISIMO-A
    GpioCtrlRegs.GPAMUX2.bit.GPIO18         = 1;                // Set GPIO18 (pin 7)  to SPICLK-A
    //GpioCtrlRegs.GPAMUX2.bit.GPIO19         = 1;                // Set GPIO19 (pin 19) to SPISTE-A

    GpioCtrlRegs.GPAPUD.bit.GPIO16          = 0;                // Enable pull-up on SPISIMO-A
    GpioCtrlRegs.GPAPUD.bit.GPIO18          = 0;                // Enable pull-up on SPICLK-A
    GpioCtrlRegs.GPAPUD.bit.GPIO19          = 0;                // Enable pull-up on SPISTE-A

    GpioCtrlRegs.GPAQSEL2.bit.GPIO16        = 3;                // Enable asynchronous input on SPISIMO-A
    GpioCtrlRegs.GPAQSEL2.bit.GPIO18        = 3;                // Enable asynchronous input on SPICLK-A
    GpioCtrlRegs.GPAQSEL2.bit.GPIO19        = 3;                // Enable asynchronous input on SPISTE-A
    {
        SpiaRegs.SPICCR.bit.SPISWRESET      = 0;                // Put SPI config into reset mode
        SpiaRegs.SPICCR.bit.CLKPOLARITY     = 0;    // Set clock polarity to be falling or rising edge
        SpiaRegs.SPICCR.bit.SPICHAR         = (charLength - 1);   // Set char length - 1
        SpiaRegs.SPICTL.bit.MASTER_SLAVE    = spiMode;          // Set master or slave mode
        SpiaRegs.SPICTL.bit.CLK_PHASE       = clockPhase;       // Set clock phase to have a delay or no delay
        SpiaRegs.SPIPRI.bit.TRIWIRE         = isTriWire;        // Set SPI to be used in 3-Wire mode
//      SpiaRegs.SPIPRI.bit.FREE            = isFree;           // Don't stop at breakpoints
//      SpiaRegs.SPIPRI.bit.PRIORITY        = isPriority;       // Set SPI as priority
//      SpiaRegs.SPIPRI.bit.SOFT            = isSoft;           //
//      SpiaRegs.SPIPRI.bit.STEINV          = invertChipSelect; // Invert SPISTE (Signal becomes active high, only works with the MASTER_SLAVE bit set to 0)
        SpiaRegs.SPISTS.all                 = 0x00e0;           // Clear all status bits (1110000)
        SpiaRegs.SPIBRR                     = baudRate;         // Set SPI baud rate
        SpiaRegs.SPICCR.bit.SPISWRESET      = 1;                // Take SPI out of reset mode
    }
    EDIS;
}


void SpiA_Write(uint16_t data) {
    volatile uint16_t dummy;
    while (SpiaRegs.SPISTS.bit.BUFFULL_FLAG == 1){}    // Wait until buffer is cleared
    SpiaRegs.SPICTL.bit.TALK = 1;                      // Set transmit path to out
    SpiaRegs.SPITXBUF = (data << 8);                   // Send data
    while (SpiaRegs.SPISTS.bit.INT_FLAG != 1){}        // Wait for data to be received
    dummy = SpiaRegs.SPIRXBUF;                         // Clear the buffer
}

uint16_t SpiA_Read() {
    uint16_t rdata, dummy = 0;
    SpiaRegs.SPICTL.bit.TALK = 0;                // Disable transmit path. This makes it so data isn't received on the SPISOMIA pin
    SpiaRegs.SPITXBUF = dummy;                   // Send dummy data to initiate the TX
    while (SpiaRegs.SPISTS.bit.INT_FLAG != 1){}  // Wait until data is received
    rdata = SpiaRegs.SPIRXBUF;                   // Receive data
    return rdata;
}

// Other
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
 *
 */

void Serial_print(char * str) {
    printf(str);
    fflush(stdout);
}

void Serial_println(char * str) {
    printf("%s\n", str);
    fflush(stdout);
}

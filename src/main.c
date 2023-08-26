/// \file
/// \brief Example that uses driverlib to flash some leds
///        Specifically works on the EKTM4C123GXL LaunchPad dev board
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/qei.h"
#include "driverlib/pin_map.h"

#define VEL_INT_FREQ 10000 // Macro to store the Interrupt frequency of QEI1
#define QEI1_PPR 257125    // Macro to store the PPR of the QEI1

/* -----------------------      Global Variables        --------------------- */
volatile uint32_t ui32Qei1Vel; // Variable to store the velocity of QEI1
volatile uint32_t ui32Qei1Pos; // Variable to store the position of QEI1
volatile int32_t i32Qei1Dir;   // Variable to store the direction of QEI1
volatile uint16_t ui16Qei1Rpm; // Variable to store the RPM of QEI1

int main(void)
{
    // Setup clock frequency to 80MHz
    SysCtlClockSet(
        SYSCTL_OSC_MAIN     // Use the main oscillator
        | SYSCTL_XTAL_16MHZ // which is a 16 MHz crystal
        | SYSCTL_USE_PLL    // Feed PLL which results in 400 MHz
        | SYSCTL_SYSDIV_2_5 // (400 MHz /2) /2.5) = 80 MHz
    );

    // Enable Port D pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
        ; // do nothing, wait for port to be ready
        // TODO: Should this fail, maybe implement WDT to make sure we can reset HW
    }
    // Enable the port F pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
        ; // do nothing, wait for port to be ready
          // TODO: Should this fail, maybe implement WDT to make sure we can reset HW
    }
    // set led pins as outputs

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    // Enable QEI Module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_QEI0))
    {
        ; // do nothing, wait for port to be ready
          // TODO: Should this fail, maybe implement WDT to make sure we can reset HW
    }

    GPIOPinTypeQEI(GPIO_PORTD_BASE, GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7); // Make Pin PD3, PD6, PD7 used for index and channel pins
    GPIOPinConfigure(GPIO_PD3_IDX0);
    // GPIOPinConfigure(GPIO_PORTD_BASE | GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PD6_PHA0);
    GPIOPinConfigure(GPIO_PD7_PHB0);

    QEIConfigure(QEI0_BASE, QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_RESET_IDX | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP, 1855);

    // Enable QEI
    QEIFilterEnable(QEI0_BASE);
    QEIEnable(QEI0_BASE);

    // Velocity configure
    QEIVelocityConfigure(QEI1_BASE, QEI_VELDIV_1, 32); // 32 ms period
    QEIVelocityEnable(QEI1_BASE);

    // enable interrupts
    IntMasterEnable();

    for (;;)
    {
        ui32Qei1Pos = QEIPositionGet(QEI0_BASE);
        ui32Qei1Vel = QEIVelocityGet(QEI0_BASE);
        i32Qei1Dir = QEIDirectionGet(QEI0_BASE);
    }
}

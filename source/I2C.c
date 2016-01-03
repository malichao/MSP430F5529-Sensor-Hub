///////////////////////////////////////////////////////////////////////////////
//   Malcolm Ma
//   UT Dallas
//   Dec 2015
//   Built with CCSv6.1.1
//
//	 Version: v0.1
//
//  Description: 
//  Basic I2C driver for TI's MSP430F5529
//  ACLK = n/a, MCLK = SMCLK = BRCLK = 4MHz
//
//
//                                /|\  /|\
//                MSP430F5529     10k  10k      MSP430F5529
//                   slave         |    |         master
//             -----------------   |    |   -----------------
//           -|XIN  P3.0/UCB0SDA|<-|----+->|P3.0/UCB0SDA  XIN|-
//            |                 |  |       |                 |
//           -|XOUT             |  |       |             XOUT|-
//            |     P3.1/UCB0SCL|<-+------>|P3.1/UCB0SCL     |
//            |                 |          |                 |
///////////////////////////////////////////////////////////////////////////////

#include <msp430.h>
#include "I2C.h"

unsigned char *PTxData;                     // Pointer to TX data
unsigned char TXByteCtr;
unsigned char sending;
unsigned char *PRxData;                     // Pointer to RX data
unsigned char RXByteCtr;
unsigned char RxBuffer[10];       // Allocate 128 byte of RAM
unsigned char reading;
unsigned char stop=0;

inline void clearI2CPort()
{
  P3DIR|=0x03;
  P3OUT|=0x03;
  P3OUT&=0x00;
  P3OUT|=0x03;
  P3DIR&=0x00;
}

inline void setI2CAddress(unsigned char addr)
{
  UCB0I2CSA=addr;
}
inline void initI2C()
{
  P1OUT &= ~0x01;                           // P1.0 = 0
  P1DIR |= 0x01;                            // P1.0 output

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  P3SEL |= 0x03;                            // Assign I2C pins to USCI_B0
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  UCB0IE |= UCTXIE + UCRXIE;                         // Enable TX interrupt
}
//  '_stop' option:
//    #define STOP    1
//    #define NO_STOP 0
//    a simple send command usually end with a stop,but for a combined protocal, 
//    which contains both send and receive command,usually has no stop in between.
//    a general combined protocal format:
//    Start-SlaveAddress-W-A-RegisterAddress-A-Start-SlaveAddress-R-A-Data-Stop
void sendI2C(unsigned char* data,unsigned char length,unsigned char _stop)
{
  if(_stop==STOP)
    stop=1;
  else
    stop=0;
  P1OUT |= 0x00;                        // P1.0 = 1
    //for(i=0;i<10;i++);                      // Delay required between transaction
    PTxData = data;               // TX array start address
                                            // Place breakpoint here to see each
                                            // transmit operation.
    //TXByteCtr = sizeof TxData;              // Load TX byte counter
    TXByteCtr = length;                 // Load TX byte counter
    sending=1;

    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition

    __bis_SR_register(GIE);     // Enter LPM0, enable interrupts
    while(sending);
    P1OUT |= 0x01;                        // P1.0 = 1
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
}

void sendI2CBytes(unsigned char* data,unsigned char length,unsigned char _stop)
{
  if(_stop==STOP)
    stop=1;
  else
    stop=0;
  P1OUT |= 0x00;                        // P1.0 = 1
    //for(i=0;i<10;i++);                      // Delay required between transaction
    PTxData = data;               // TX array start address
                                            // Place breakpoint here to see each
                                            // transmit operation.
    //TXByteCtr = sizeof TxData;              // Load TX byte counter
    TXByteCtr = length;                 // Load TX byte counter
    sending=1;

    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR + UCTXSTT;             // I2C TX, start condition

    __bis_SR_register(GIE);     // Enter LPM0, enable interrupts
    while(sending);
    P1OUT |= 0x01;                        // P1.0 = 1
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
}

unsigned char readI2C()
{
	PRxData = (unsigned char *)RxBuffer;    // Start of RX buffer
	RXByteCtr = 1;                          // Load RX byte counter
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
    UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
    while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
    while(RXByteCtr);
    return RxBuffer[0];
}
unsigned char WORD_LSB=0;

unsigned int readI2CWord()
{
  unsigned int temp;
  RxBuffer[0]=0;
  RxBuffer[1]=0;
  	  PRxData = (unsigned char *)RxBuffer;    // Start of RX buffer
      RXByteCtr = 2;                          // Load RX byte counter
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
    while(RXByteCtr);
    temp=((unsigned int)RxBuffer[0])<<8;
    temp|=((unsigned int)RxBuffer[1]);

    return temp;
}

void readI2CBytes(unsigned char length,unsigned char* pointer)
{
	int i;
	PRxData = (unsigned char *)RxBuffer;    // Start of RX buffer
	RXByteCtr = length;                      // Load RX byte counter
	UCB0CTL1 &= ~UCTR;
	UCB0CTL1 |= UCTXSTT;                    // I2C start condition
	while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
	while(RXByteCtr);
	for(i=0;i<length;i++,pointer++)
		*pointer=RxBuffer[i];
	__no_operation();
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4:
  case  6: break;                           // Vector  6: STTIFG
  case  8: break;                           // Vector  8: STPIFG
  case 10:                                 // Vector 10: RXIFG
			RXByteCtr--;                            // Decrement RX byte counter
			if (RXByteCtr)
			{
			  *PRxData++ = UCB0RXBUF;               // Move RX data to address PRxData
			  if (RXByteCtr == 1)                   // Only one byte left?
				UCB0CTL1 |= UCTXSTP;                // Generate I2C stop condition
			}
			else
			{
			  *PRxData = UCB0RXBUF;                 // Move final RX data to PRxData
			  //__bic_SR_register_on_exit(LPM0_bits); // Exit active CPU
			}
  case 12:                                  // Vector 12: TXIFG
      if (TXByteCtr)                          // Check TX byte counter
      {
        UCB0TXBUF = *PTxData++;               // Load TX buffer
        TXByteCtr--;                          // Decrement TX byte counter
      }
      else
      {
        if(stop)
        UCB0CTL1 |= UCTXSTP;                  // I2C stop condition
        UCB0IFG &= ~UCTXIFG;                  // Clear USCI_B0 TX int flag
        sending=0;
      }
      break;
  default: break;
  }
}

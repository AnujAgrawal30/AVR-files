/*
 * File:   ADC4_UART_Transmitter.c
 * Author: Anuj
 *
 * Recieving data through UART and storing as 10 bit ADC values
 * The data sent through UART is of the format:
 * 1 start bit
 * 
 * 8 data bit
 * 7:6 - selecting which ADCs value is it sending
 * 5th bit for selecting which part of data is sent
 * 4:0 is 5 bit data out of 10 bit ADC value
 * 
 *  - 2 stop bits
 * One ADCs data is sent through two transmission packets
 * 
 * Created on 12 March, 2019, 10:59 PM
 */


#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>

uint8_t ADC_select;
int main(void) {

    // Setting up the UART communication
    int UBBR_value = 25;                            // for 2400 baud rate
    UBRR0H |= (unsigned int) (UBBR_value >> 8);     // removing 8 least significant digits
    UBRR0L |= (unsigned int) UBBR_value;            // Taking those 8 least digits
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);          // Enabling RX and TX
    UCSR0C |= (1 << USBS0);                         // Setting 2 stop bits
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);        // Setting 8 bit data frame
    
    uint16_t ADC_values[4];
    while (1) {
        while (!(UCSR0A & (1 << RXC0)));            // Wait while there is no data to be written
        uint8_t data = UDR0;                        // Write data to data
        ADC_values[data >> 6] = ((data & 0b00011111) << (5 * data & 0b00100000)); // Magic
        
        // Do whatever you want to do to the data
        // Else it will be updated automatically after 8 cycles (4 * 2)
    }
}

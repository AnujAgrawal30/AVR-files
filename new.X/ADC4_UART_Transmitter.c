/*
 * File:   ADC4_UART_Transmitter.c
 * Author: Anuj
 *
 * Collecting data through 4 ADCs and send it through UART
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
    //Setting up the ADC
    ADCSRA |= 1 << ADEN;    // Enabling the ADC
    ADCSRA |= 1 << ADPS2;   //Selecting the pre-scaler to 16
    ADCSRA |= 1 << ADIE;    // To enable ADC interrupt on completion
    sei();                  // Setting up the global interrupts
    ADCSRA |= 1 << ADSC;    // To start first conversion
    ADMUX = 0;              // Starting with first ADC
    /* Note here that sei is called on first to beforehand set up the interrupt
     before starting the first conversion to make the system ready for 
     subsequent interrupt calls when sampling completes */
    
    //ADMUX |= 1 << ADLAR;    // Uncomment if wanting to get left adjusted data

    // Setting up the UART communication
    int UBBR_value = 25;                            // for 2400 baud rate
    UBRR0H |= (unsigned int) (UBBR_value >> 8);     // removing 8 least significant digits
    UBRR0L |= (unsigned int) UBBR_value;            // Taking those 8 least digits
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);          // Enabling RX and TX
    UCSR0C |= (1 << USBS0);                         // Setting 2 stop bits
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);        // Setting 8 bit data frame
    
    while (1) {
        // Nothing here since we are using interrupt to ample ADC
    }
}

ISR(ADC_vect){
    uint16_t ADC_val;           // The variable to store ADC value
    uint8_t temp = ADCL;        // Since ADCL has to be read first
    ADC_val = ADCH << 2 | temp; // Then ADCH
    /* To move to next ADC input
    Note here that we have selected ADC3..0 pins for sampling */
    
    /* Send data through UART 
     * 
     * Setting first part of the value */
    uint8_t data = 0;
    data |= (ADC_select << 6);    // Sending which ADCs value is it
    data |= (0 << 5);                   // Sending first part of the data (Not needed)
    data |= (ADC_val >> 5);             // Sending most 5 significant digits
    /* First part of data ready to be sent */
    while (!(UCSR0A & (1 << UDRE0)));  // Wait while data is ready to be sent
    UDR0 = data;                        // Writing data
    /* Setting second part of data */
    data |= (ADC_select << 6);          // Sending which ADCs value is it
    data |= (1 << 5);                   // Sending second part of the data
    data |= (ADC_val & 0b0000011111);   // Setting last 5 digits of the ADC value
    /* Second part of data ready to be sent */
    while (!(UCSR0A & (1 << UDRE0)));  // Wait while data is ready to be sent
    UDR0 = data;                        // Writing data
    /* Data Sent */
    ADC_select++;                       // Setting next ADC to be read
    ADC_select = ADC_select%4;
    ADMUX |= ADC_select;
    ADCSRA |= 1 << ADSC;    // To start next conversion
}

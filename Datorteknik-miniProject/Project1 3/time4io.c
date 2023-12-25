#include <stdint.h> // Standard library
#include <pic32mx.h> // the chip-kit library
#include "mipslab.h"  // Mipslab library

// This function is for the switches to work and send data, This function will be called later 
int getsw( void ){     // getting all the Switches 1-4 
   int y = PORTD >>8;  // shifting portD 8 to right
    y &= 0x0f;         // Masking with 00001111. 
    return(y);         // Returning back 
}
// This function is for buttons 2,3,4 this function will make those buttons work and send data, This function will be 
// called later
int getbtns(void){     
    int x = PORTD >> 5;  // Shifting PrtD 5 to right
    x &= 0x07;           // Masking with 0111
    return(x);           // Returning back 
}

// This function is in order to enable button 1 and send data, this function will be called later
int btn(void){
    int x = PORTF >> 1;   // Shifting PortD to right 
    x &= 0x1;             // Masking with 0001
    return(x);            // returning back 
} 
// Basic application for logging temperature data on an SD-card. 
// Requires the libraries FATFileSystem, SDFileSystem. 
// The library 4DGL-uLCD-SE is optional. 

#include "mbed.h" 
#include "uLCD_4DGL.h" 
#include "SDFileSystem.h" 

DigitalIn badd(p29);                                // Buttons' inputs go on Pins #29 and #30. 
DigitalIn bsub(p30);                                
DigitalOut dout(p18);                               // Transistor controller goes on Pin #18. 
DigitalOut led1(LED1);                              // Button indicators go on LEDs #1 and #4. 
DigitalOut led4(LED4);                              
AnalogIn ain(p15);                                  // Thermistor input goes on Pin #15. 
SDFileSystem sd(p5, p6, p7, p8, "sd");              // SD-card reader on DI = Pin #5, DO = #6, SCK = #7, CS = #8. 

int main() 
{    
    FILE * file; 
    float voltage = 0, temperature = 0; 
    
    // This code acts as the temperature logger. Connect the SD-card reader breakout board accordingly. 
    
    file = fopen("/sd/Temperature.txt", "w"); 
    if (file == NULL) 
    {
        return -1;
    }
    
    for(int i = 1; i <= 20; i++) 
    {
        voltage     = ((ain*3.3)); 
        temperature = ((voltage-0.5)*100.0); 
        fprintf(file, "%d.\t\tV = %2.3f V\t\tT = %3.2f 'C\n\r", i, voltage, temperature); 
        
        wait(1.0); 
    }
    fclose(file);
}

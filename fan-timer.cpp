// Fan-timer controller with LCD output, temperature measurements, & serial-over-USB capabilities. 
// Requires the library 4DGL-uLCD-SE. 
// The libraries FATFileSystem, SDFileSystem are optional. 

#include "mbed.h" 
#include "uLCD_4DGL.h" 
#include "SDFileSystem.h" 

DigitalIn badd(p29);                                // Buttons' inputs go on Pins #29 and #30. 
DigitalIn bsub(p30);                                
DigitalOut dout(p18);                               // Transistor controller goes on Pin #18. 
DigitalOut led1(LED1);                              // Button indicators go on LEDs #1 and #4. 
DigitalOut led4(LED4);                              
AnalogIn ain(p15);                                  // Thermistor input goes on Pin #15. 
uLCD_4DGL uLCD(p9, p10, p11);                       // LCD connectors: RX = Pin #9, TX = #10, RES = #11. 
Serial serial(USBTX, USBRX);                        // Serial connection over the USB-mini port. 

int main() 
{    
    // These are the default timer settings every time the device is first powered on. 
    int period = 60, ontime = 18, button = 6; 
    float temperature = 0; 
    
    // Initialize the Fan-timer buttons. 
    badd.mode(PullUp); 
    bsub.mode(PullUp); 
    
    // Initialize output on the LCD-screen & serial output. 
    uLCD.baudrate(115200); 
    uLCD.background_color(WHITE); 
    uLCD.textbackground_color(WHITE); 
    uLCD.cls(); 
    uLCD.color(BLUE); 
    uLCD.locate(4, 2); 
    uLCD.printf("TEMPERATURE"); 
    serial.printf("TEMPERATURE\n\r"); 
    
    // Use the buttons (with LED indicators) to increase/decrease the time that the fan is ON. 
    while(1) 
    {   
        for (int i = 0; i < period; i++)
        {
            led1 = 0; led4 = 0; 
            if (i < ontime)
            {
                dout = 1; 
            }
            else
            {
                dout = 0; 
            }
            
            if (badd == 0 && ontime < (period-button)) 
            {
                led1 = 1; 
                ontime += button; 
            }
            if (bsub == 0 && ontime > 0) 
            {
                led4 = 1; 
                ontime -= button; 
            } 
            
            temperature = ((ain * 3.3) - 0.64) * 100.0; 
            serial.printf("%2.1f 'C\n\r", temperature); 
            uLCD.locate(6, 3); uLCD.printf("%2.1f 'C", temperature);   
            
            wait(1); 
        }
    }
}

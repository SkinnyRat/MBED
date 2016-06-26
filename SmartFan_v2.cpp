// Smart-fan program to turn on/off a USB fan via remote or AI control. 

#include "mbed.h" 
#include "IRremote.h" 
#include "RHT03.h" 
#include "SDFileSystem.h" 

/*
Button codes: 
- On    = 27228 
- A     = 31224 
- B     = 30225 
- C     = 26229 
- Up    = 5250 
- Down  = 255 
- Left  = 8247 
- Right = 1254 
- Zero  = 4251 

Feedback table: 
1.  28'C, <80% H 
2.  28'C, 80-90% H 
3.  28'C, 91-95% H 
4.  28'C, >95% H 
5.  29'C, <80% H 
6.  29'C, 80-90% H 
7.  29'C, 91-95% H 
8.  29'C, >95% H 
9.  30'C, <80% H 
10. 30'C, 80-90% H 
11. 30'C, 91-95% H 
12. 30'C, >95% H 
13. 31'C, <80% H 
14. 31'C, 80-90% H 
15. 31'C, 91-95% H 
16. 31'C, >95% H 
*/

#define BASE 28 


int main()
{
    Serial PC(USBTX, USBRX); 
    IRremote        RC(p15); 
    RHT03      HumTemp(p24); 
    DigitalOut      TS(p21); 
    SDFileSystem    SD(p5, p6, p7, p8, "sd"); 
    
    // All the arrays' initialization stuff goes here. 
    float Temp[102], Hum[102]; 
    for (int i=0; i<102; i++)
    {
        Temp[i] = -400; 
        Hum[i]  = -400; 
    }
    
    int Feedback[16][50]; 
    for (int j=0; j<16; j++) 
        for (int k=0; k<50; k++) 
            Feedback[j][k] = -1; 
    
    FILE  *    file; 
    int   timer = 0; 
    int   cmd   = 0; 
    int   fan   = 1; 
    int   ctr   = 0; 
    int   old   = 0; 
    int   saved = 0; 
    int   click = 0; 
    
    const int eloop = 6000; 
    DigitalOut L1(LED1); DigitalOut L2(LED2); DigitalOut L3(LED3); DigitalOut L4(LED4); 
    
    // First read previously stored Remote-Controller feedback values. 
    file = fopen("/sd/RCValues.txt", "r"); 
    if (file != NULL)
    {
        L1 = L2 = L3 = L4 = 1; 
        for (int j=0; j<16; j++)
        {
            for (int k=0; k<50; k++)
            {
                fscanf(file, "%d ", &Feedback[j][k]); 
            }
        }    
        fclose(file); 
        wait(0.0400); 
        L1 = L2 = L3 = L4 = 0; 
    }
    
    while (1)
    {
        // Main event loop for controlling the fan operation. 
        for (int i=0; i<eloop; i++)
        {            
            // Turn fan on/off by fraction of total event loop time. 
            TS = 1; 
            if (i >= eloop*fan/10)
                TS = 0;                 
            
            // Current active buttons: P, A(--), B(1/0), C(++). 
            cmd = ((int)RC.readclear(2))*1000+(int)(RC.readclear(3));             
            switch(cmd)
            {
            case 27228:
                {
                    if (saved == 0)
                        L1 = L4 = 1; 
                    if (saved == 1)
                        L1 = 1; 
                    if (saved == 2)
                        L1 = L2 = 1; 
                    if (saved == 3)
                        L1 = L2 = L3 = 1; 
                    if (saved >= 4)
                        L1 = L2 = L3 = L4 = 1;  
                }
            break; 
            case 31224:
                if (fan > 1)
                {
                    fan--; 
                    click   = 1; 
                    L1 = L2 = 1; 
                } 
            break; 
            case 30225:
                {
                    if (fan > 0)
                    {
                        old = fan; 
                        fan = 0; 
                    }
                    else
                    {
                        fan = old; 
                        old = 0; 
                    }
                    click   = 1; 
                    L2 = L3 = 1; 
                }
            break; 
            case 26229:
                if (fan < 9)
                {
                    fan++; 
                    click   = 1; 
                    L3 = L4 = 1; 
                }                
            break; 
            }            
            wait(0.100); 
            L1 = L2 = L3 = L4 = 0; 
        }
        timer++; 
        
        // Once every 10 minutes, record the temperature/humidity. 
        int   rht   = 0; 
        bool done   = 0; 
        
        while(!done && rht < 10) 
        {
            wait(2.000); 
            if (HumTemp.readData() == RHT_ERROR_NONE) 
                done = true; 
            rht++; 
        } 
        if (ctr < 100)
        {
            Temp[ctr] = HumTemp.getTemperatureC(); 
            Hum[ctr]  = HumTemp.getHumidity(); 
            ctr++; 
        }
        
        // Also save the Remote-Controller feedback if the RC was clicked. 
        int idx = (int)(Temp[ctr-1]-BASE+0.5f)*4; 
        int sub = (int)(Hum[ctr-1]); 
        
        if (click)
        {
            if (idx < 0)
                idx = 0; 
            if (idx > 12)
                idx = 12; 
            if (sub >= 80 && sub < 90)
                idx += 1; 
            if (sub >= 90 && sub < 95)
                idx += 2; 
            if (sub >= 95)
                idx += 3; 
            
            for (int i=0; i<50; i++)
            {
                if (Feedback[idx][i] == -1)
                {
                    Feedback[idx][i] = fan; 
                    break; 
                }
            }
            click = 0; 
        }
        
        // Re-calculate the time that the fan should be turned on. 
        int sum = 0; 
        int ave = 0; 
        
        for (int j = 0; j<50; j++)
        {
            if (Feedback[idx][j] != -1)
            {
                sum += Feedback[idx][j]; 
                ave ++; 
            } 
        }
        fan += (int)(sum); 
        fan /= (int)(ave+1); 
        PC.printf("\r\nAverage = %d \r\n", fan); 
        
        // Once every 2 hour(s), write the records to SD-card. 
        if (timer == 12)
        {
            file = fopen("/sd/TempHum.txt", "w"); 
            if (file != NULL)
            {
                L1 = L2 = L3 = L4 = 1; 
                for (int i=0; i<100; i++)
                {
                    fprintf(file, "%d.\t\tTemp = %f 'C,\t\tHum = %f % \r\n", (i+1), Temp[i], Hum[i]); 
                }    
                fclose(file); 
                wait(0.0400); 
            }
            
            file = fopen("/sd/RCValues.txt", "w"); 
            if (file != NULL)
            {
                for (int j=0; j<16; j++)
                {
                    for (int k=0; k<50; k++)
                    {
                        fprintf(file, "%d ", Feedback[j][k]); 
                    }
                    fprintf(file, "\r\n"); 
                }    
                fclose(file); 
                L1 = L2 = L3 = L4 = 0; 
            }
            saved++; 
            timer = 0; 
        }
    }
}




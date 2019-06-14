#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "ili9341.h" //lcd library
#include "i2c_master_noint.h"
#include<stdio.h> //sprintf
#include<stdlib.h>

// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL =  ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // disable secondary osc
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // use slowest wdt
#pragma config WINDIS = OFF // wdt no window mode
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 00000000 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

//function to write single characters on the LCD
    
    //function to draw bars on the LCD
    void drawBarH(unsigned short x, unsigned short y, signed short n1o, unsigned short color_on, unsigned short color_off, unsigned int width, unsigned int length, signed short n1max){
       
        unsigned short ni1=0;
        unsigned short ni2=0;
        unsigned short yi=0;
        signed short length_max_v = length/2;
        signed short n1 = n1o/n1max;
        
        for(yi=0; yi<=width; yi++){ //loop over length of the bar
            
            if(n1>0){
                 for (ni2=0; ni2<=length_max_v ; ni2++){ //if acceleration is negative, turn off the positive side
                     LCD_drawPixel(x-ni2, y+yi, color_off);
                      }  
                if (n1<=length_max_v){
                    for (ni1=0; ni1<=n1 ; ni1++){ //color the bar up to the value of the acceleration
                     LCD_drawPixel(x+ni1, y+yi, color_on);
                    }
                     for (ni2=n1; ni2<=length_max_v ; ni2++){ //turn off the pixels that are greater than the acceleration
                     LCD_drawPixel(x+ni2, y+yi, color_off);
                      }     
                     }
                 
                else{
                     for (ni1=0; ni1<=length_max_v ; ni1++){ //turn on whole bar if acceleration is greater than the length
                     LCD_drawPixel(x+ni1, y+yi, color_on);
                    }
                    }
                
            }
            
           
            if(n1<0){
                for (ni2=0; ni2<=length_max_v ; ni2++){ //turn off the positive side of the bar 
                     LCD_drawPixel(x+ni2, y+yi, color_off);
                      } 
                if (-n1<=length_max_v){
                    for (ni1=0; ni1<=-n1 ; ni1++){ //turn on the part of the bar that matches the acceleration
                     LCD_drawPixel(x-ni1, y+yi, color_on);
                    }
                     for (ni2=-n1; ni2<=length_max_v ; ni2++){ //turn off the parts greater than the acceleration
                     LCD_drawPixel(x-ni2, y+yi, color_off);
                     }
                     }
                 
                else{
                     for (ni1=0; ni1<=length_max_v ; ni1++){ //loop over n1 values of n (color on of the bar)
                     LCD_drawPixel(x-ni1, y+yi, color_on);
                    }
                 }
            }
            
        
        
        }
    }
    
    void drawBarV(unsigned short x, unsigned short y, signed short n1o, unsigned short color_on, unsigned short color_off, unsigned int width, unsigned int length, signed short n1max){
       
        unsigned short ni1=0;
        unsigned short ni2=0;
        unsigned short xi=0;
        signed short length_max_v = length/2;
        signed short n1 = n1o/n1max;
        
        for(xi=0; xi<=width; xi++){ //loop over width of the bar
            
            if(n1>0){
                 for (ni2=0; ni2<=length_max_v ; ni2++){ //loop over n2 values of n (color off of the bar background)
                     LCD_drawPixel(x+xi, y-ni2, color_off);
                      }  
                if (n1<length_max_v){
                    for (ni1=0; ni1<=n1 ; ni1++){ //loop over n1 values of n (color on of the bar)
                     LCD_drawPixel(x+xi, y+ni1, color_on);
                    }
                     for (ni2=n1; ni2<=length_max_v ; ni2++){ //loop over n2 values of n (color off of the bar background)
                     LCD_drawPixel(x+xi, y+ni2, color_off);
                      }     
                     }
                 
                else{
                     for (ni1=0; ni1<=length_max_v ; ni1++){ //loop over n1 values of n (color on of the bar)
                     LCD_drawPixel(x+xi, y+ni1, color_on);
                    }
                    }
                
            }
            
           
            if(n1<0){
                for (ni2=0; ni2<=length_max_v ; ni2++){ //loop over n2 values of n (color off of the bar background)
                     LCD_drawPixel(x+xi, y+ni2, color_off);
                      } 
                if (-n1<=length_max_v){
                    for (ni1=0; ni1<=-n1 ; ni1++){ //loop over n1 values of n (color on of the bar)
                     LCD_drawPixel(x+xi, y-ni1, color_on);
                    }
                     for (ni2=-n1; ni2<=length_max_v ; ni2++){ //loop over n2 values of n (color off of the bar background)
                     LCD_drawPixel(x+xi, y-ni2, color_off);
                     }
                     }
                 
                else{
                     for (ni1=0; ni1<=length_max_v ; ni1++){ //loop over n1 values of n (color on of the bar)
                     LCD_drawPixel(x+xi, y-ni1, color_on);
                    }
                 }
            }
            
        
        
        }
    }
    

int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // set input and output pins
    TRISBbits.TRISB4=1; // B4 i an input
    TRISAbits.TRISA4=0; //A4 is an output
    LATAbits.LATA4=1; //A4 initially set to 3.3V
    SPI1_init();
    initExp();
    LCD_init();
    
    __builtin_enable_interrupts(); 
    
    
    
    unsigned short bg=0x780F; // background color
    unsigned short charcolor=0x0000; //color of the text
    unsigned short offcolor=0xffff; // color for text background to distinguish b/w off and on pixels
    unsigned short pg_bar_p=0x0000; //progress indicator
    unsigned short pg_bar_n=0x07ff; //progress remaining indicator
    
    unsigned char str [20];
    unsigned char str1 [20]; 
    unsigned char str2 [20];
    unsigned char str3 [20];
    unsigned char str4 [20];
    unsigned char str5 [20];
    
    
    
    unsigned char bar_number_x [4]; //array that is going to contain the number indicating the acc
    
    unsigned char data [30]; //array that stores the output data from the LSM
   
    LCD_clearScreen(ILI9341_BLACK);
    
    while(1) {
        
    
        
        _CP0_SET_COUNT(0);        
        //make LED connected to A4 blink every half second
         while(_CP0_GET_COUNT()<480000){ 
             LATAbits.LATA4=1;
         }
         _CP0_SET_COUNT(0);
                 
        while(_CP0_GET_COUNT()<480000){
             LATAbits.LATA4=0;
        }
        _CP0_SET_COUNT(0);
        
        sprintf(str, "WHOAMI %d" , read_i2c(0b1101011,0x0F)); //read whoami   
        
        LCD_drawstring(str,20,20,charcolor,offcolor);
       while(_CP0_GET_COUNT()<480000){ //read at a 20 Hz frequency
         
       read_multiple_i2c(0b1101011,0x20,data,14); //read the seven consecutive registers indicating temperature, acceleration in xyz, and angular velocity in xyz
        //reconstruct the signed short by shifting the high byte and ORing it with the low byte 
        //signed short temp = data[1]<< 8 | data [0]; //temperature is given by two 8 bits number, store the second one, then shift on by 8 bits and store the first one
        signed short gyro_x = data[3]<< 8 | data [2];
        signed short gyro_y = data[5]<< 8 | data [4];
        signed short acc_x = data[9]<< 8 | data [8];
        signed short acc_y = data[11]<< 8 | data [10];
         
          sprintf(str2, "Ax %d   " , acc_x); 
          LCD_drawstring(str2,20,30,charcolor,offcolor);  
         
        sprintf(str3, "Ay %d " , acc_y);    
          LCD_drawstring(str3,20,40,charcolor,offcolor);  
         
         sprintf(str4, "Gx %d    " , gyro_x);    
        
         LCD_drawstring(str4,20,50,charcolor,offcolor);  
         sprintf(str5, "Gy %d    " , gyro_y); 
        
         LCD_drawstring(str5,20,60,charcolor,offcolor);  
        drawBarV(100, 90, acc_y, ILI9341_YELLOW, ILI9341_BLUE, 5, 200, 250); //draw progress bar starting from pixel at x=13 y=80
         
        drawBarH(100, 90, acc_x, ILI9341_YELLOW, ILI9341_BLUE, 5, 200, 250); //draw progress bar starting from pixel at x=13 y=80
                
          
} 
        }
    }
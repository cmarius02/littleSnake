#include<avr/io.h>
#include <util/delay.h>

#include "Nokia5110.h"

#include "font6x8.h"

//
#define BIT(i)  (1<<i)  

#define SCLK_set   PORTD|=BIT(6)      //serial clock input
#define SCLK_clr   PORTD&=~BIT(6)  

#define SDIN_set  PORTD|=BIT(5)      //serial data input   
#define SDIN_clr  PORTD&=~BIT(5) 

#define LCD_DC_set   PORTD|=BIT(4)    //data/commande
#define LCD_DC_clr   PORTD&=~BIT(4)  

#define LCD_CE_set  PORTD|=BIT(3)      //chip enable
#define LCD_CE_clr  PORTD&=~BIT(3) 
 
#define LCD_RST_set  PORTD|=BIT(2)     //external reset input
#define LCD_RST_clr  PORTD&=~BIT(2) 


void delay_1us(void)                 //delay 1us
  {
     _delay_us(1);
  }

void delay_1ms(void)                 //delay 1ms
  {
    _delay_ms(1);
  }
  
/*-----------------------------------------------------------------------
LCD_write_byte    : write data to LCD

input parmenter£ºdata    £ºdata
          command £ºcommand

created date         £º2013-4-25
-----------------------------------------------------------------------*/
void LCD_write_byte(unsigned char dat, unsigned char command)
  {
    unsigned char i;
      LCD_CE_clr;
    
    if (command == 0)
       LCD_DC_clr;
    else
       LCD_DC_set;

		for(i=0;i<8;i++)
		{
			if(dat&0x80)
		     SDIN_set;
			else
		      SDIN_clr;
		    SCLK_clr;
			dat = dat << 1;
          	SCLK_set;
		}
       LCD_CE_set;
  }

void LCD_init(void)                 //LCD initialization
  {
    DDRD|=0xFF;    

     LCD_RST_clr;
    delay_1us();
     LCD_RST_set;
    
    LCD_CE_clr;

    delay_1us();
     LCD_CE_set;
  
    delay_1us();

    LCD_write_byte(0x21, 0);	// set LCD mode
    LCD_write_byte(0xc8, 0);	// set bias voltage
    LCD_write_byte(0x06, 0);	// temperature correction
    LCD_write_byte(0x13, 0);	// 1:48
    LCD_write_byte(0x20, 0);	// use bias command
    LCD_clear();	           // clear the LCD
    LCD_write_byte(0x0c, 0);	// set LCD mode,display normally
        
    LCD_CE_clr;    
  }


void LCD_clear(void)          // clear the LCD
  {
    unsigned int i;

    LCD_write_byte(0x0c, 0);			
    LCD_write_byte(0x80, 0);			

    for (i=0; i<504; i++)
	  {
       LCD_write_byte(0, 1);
	  }		
  }

/*-----------------------------------------------------------------------
LCD_set_XY              : Set the LCD coordinate functions

input parameter£ºX       £º0£­83
               Y        £º0£­5

created date           £º2013-4-25
-----------------------------------------------------------------------*/
void LCD_set_XY(unsigned char X, unsigned char Y)
  {
    LCD_write_byte(0x40 | Y, 0);		// column
    LCD_write_byte(0x80 | X, 0);          	// row
  }

/*-----------------------------------------------------------------------
LCD_write_char    : Display English characters

input parameter£ºc   :char to display

created date         £º2013-4-25
-----------------------------------------------------------------------*/
void LCD_write_char(unsigned char c)
  {
    unsigned char line;

    c -= 32;

    for (line=0; line<6; line++)
      LCD_write_byte(font6x8[c][line], 1);
  }

/*-----------------------------------------------------------------------
LCD_write_english_String  : Display English strings

input parameter£ºX       £º0£­83
                Y        £º0£­5

created date         £º2013-4-25	
-----------------------------------------------------------------------*/
void LCD_write_string(unsigned char X,unsigned char Y,char *s)
  {
    LCD_set_XY(X,Y);
    while (*s) 
      {
	 LCD_write_char(*s);
	 s++;
      }
  }

/*-----------------------------------------------------------------------
LCD_draw_map      : Bitmap drawing function

input parmenter£ºX¡¢Y    £ºstarting point X,Y£»
                 *map    £ºbitmap data
                 Pix_x   £ºhighth
                 Pix_y   £ºwidth

created date         £º2013-4-25
-----------------------------------------------------------------------*/
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                  unsigned char Pix_x,unsigned char Pix_y)
  {
    unsigned int i,n;
    unsigned char row;
    
    if (Pix_y%8==0) row=Pix_y/8;      //calculate how many line is needed
      else
        row=Pix_y/8+1;
    
    for (n=0;n<row;n++)
      {
      	LCD_set_XY(X,Y);
        for(i=0; i<Pix_x; i++)
          {
            LCD_write_byte(map[i+n*Pix_x], 1);
          }
        Y++;                         //chang line
      }      
  }
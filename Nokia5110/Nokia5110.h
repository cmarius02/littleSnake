#ifndef __Nokia5110_h__
#define __Nokia5110_h__ 

void LCD_init( void );
void LCD_clear( void );

void LCD_set_XY( unsigned char X, unsigned char Y );

void LCD_write_char( unsigned char c );
void LCD_write_string( unsigned char X, unsigned char Y, char *s );

void LCD_draw_bmp_pixel( unsigned char X, unsigned char Y, unsigned char *map, unsigned char Pix_x, unsigned char Pix_y );

#endif // __Nokia5110_h__
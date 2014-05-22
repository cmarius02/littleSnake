#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <math.h>  //include libm

#include "mpu6050/mpu6050.h"
#include "Nokia5110/Nokia5110.h"

#define LIMIT 0.2

#define LEFT  42
#define UP    43
#define RIGHT 44
#define DOWN  45


#define WIDTH  14
#define HEIGHT 6

#define MAX_LEN 100

#define SNAKE 'O'
#define HEAD  '0'
#define SPACE ' '
#define FOOD  'X'

typedef struct toast {
	int x;
	int y;
} toast;

char screen[HEIGHT][WIDTH];
toast snake[MAX_LEN];
toast food;

int snake_len, snake_tail;
int game = 1, score = 0;
int direction = RIGHT;

void put_food()
{
    	srand(42);
    	int x = (rand() + snake_len)  % HEIGHT;
    	int y = (rand() + snake_tail) % WIDTH;

	// Find available space if random fails
	if (screen[x][y] == SNAKE) {
       		for (x=0; x<HEIGHT; x++)
       		for (y=0; y<WIDTH; y++) {
			for (int z=0; z<MAX_LEN; z++) {
				if (snake[z].x == -1 || x == snake[z].x || y == snake[z].y) {
					screen[x][y] = FOOD;
					food.x = x; food.y = y;
					return;
				}
			}
		}

	} else {
		screen[x][y] = FOOD;
		food.x = x; food.y = y;
	}
}

void update_snake()
{
	int done;
       	for (int x=0; x<HEIGHT; x++)
       	for (int y=0; y<WIDTH; y++) {

		done = 0;
		for (int z=0; z<MAX_LEN; z++) {
			
			if (snake[z].x != -1 && x == snake[z].x && y == snake[z].y) {
				screen[x][y] = SNAKE;
				done = 1;
				break;
			}
		}

		if (done == 0) {
				if (screen[x][y] == FOOD) {
					screen[x][y] = FOOD;
				} else {
					screen[x][y] = SPACE;
				}
		}

	}
}

void change_dir()
{
	// Can eat?
	if (food.x == snake[snake_len-1].x && food.y == snake[snake_len-1].y) {
		snake[snake_len].x = food.x;
		snake[snake_len].y = food.y;
		if (++snake_len  == MAX_LEN) snake_len  = 0;

		screen[food.x][food.y] = SNAKE;
		score += 10; put_food();
	}

	switch(direction) {
	case UP:    {
		// Check for screen edge
		if (snake[snake_len-1].x == 0) {
			snake[snake_len].x = 5;
			snake[snake_len].y = snake[snake_len-1].y;
		} else {
			snake[snake_len].x = snake[snake_len-1].x-1;
			snake[snake_len].y = snake[snake_len-1].y;
		}

		break;
	}
	case RIGHT: {
		// Check for screen edge
		if (snake[snake_len-1].y == WIDTH-1) {
			snake[snake_len].x = snake[snake_len-1].x;
			snake[snake_len].y = 0;
		} else {
			snake[snake_len].x = snake[snake_len-1].x;
			snake[snake_len].y = snake[snake_len-1].y+1;
		}

		break;
	}
	case DOWN:  {
		// Check for screen edge
		if (snake[snake_len-1].x == HEIGHT-1) {
			snake[snake_len].x = 0;
			snake[snake_len].y = snake[snake_len-1].y;
		} else {
			snake[snake_len].x = snake[snake_len-1].x+1;
			snake[snake_len].y = snake[snake_len-1].y;
		}

		break;
	}
	case LEFT:  {
		// Check for screen edge
		if (snake[snake_len-1].y == 0) {
			snake[snake_len].x = snake[snake_len-1].x;
			snake[snake_len].y = WIDTH-1;
		} else {
			snake[snake_len].x = snake[snake_len-1].x;
			snake[snake_len].y = snake[snake_len-1].y-1;
		}

		break;
	}
	}

	// Wrong bite
	if (screen[snake[snake_len].x][snake[snake_len].y] == SNAKE) {

		// Score to string and screen
		char ss[] = "Points: ";
		char s[10];
		dtostrf(score, 3, 0, s);
		strcat(ss,s);

		LCD_clear();
		LCD_write_string(0,0,"  GAME  OVER ");
		LCD_write_string(0,2,ss);

		// End game
		game = 0; return;
	}



	// Mark slice as inactive
	snake[snake_tail].x = -1;

	// Keep inside array limit
	if (++snake_tail == MAX_LEN) snake_tail = 0;
	if (++snake_len  == MAX_LEN) snake_len  = 0;

	update_snake();
}

int main(void) 
{
	double axg = 0, ayg = 0, azg = 0;
	double gxds = 0, gyds = 0, gzds = 0;

	//init mpu6050
	mpu6050_init();
	_delay_ms(50);

	LCD_init();
	LCD_clear();

	put_food();

	// INtro
	while (1) {
		// Read current parameters
		mpu6050_getConvData(&axg, &ayg, &azg, &gxds, &gyds, &gzds);

		LCD_write_string(0,0," Little Snake ");
		LCD_write_string(0,3,"  >> Move << ");
		if (axg > LIMIT || axg < -LIMIT ||ayg < -LIMIT || ayg >  LIMIT)
			break;
	}

    	// Init snake, both on screen and array
	screen[3][5] = SNAKE;
	screen[3][6] = SNAKE;
	screen[3][7] = SNAKE;
	snake[0].x = 3; snake[0].y = 5;
	snake[1].x = 3; snake[1].y = 6;
	snake[2].x = 3; snake[2].y = 7;

	snake_tail = 0; snake_len = 3;
	for (int z=snake_len; z<MAX_LEN; z++) snake[z].x = -1;
	

	while(game) {

		// Read current parameters
		mpu6050_getConvData(&axg, &ayg, &azg, &gxds, &gyds, &gzds);

		// Take a decision
		if(axg >  LIMIT)      {if (direction !=    UP) direction = DOWN;}
		else if(axg < -LIMIT) {if (direction !=  DOWN) direction = UP;}
		if(ayg >  LIMIT)      {if (direction !=  LEFT) direction = RIGHT;}
		else if(ayg < -LIMIT) {if (direction != RIGHT) direction = LEFT;}

		// Make it happen
		change_dir();

		// Check end game
		if (!game) break;

		LCD_init();
     	   	LCD_clear();
	
		// Print entire screen
       		for (int x=0; x<HEIGHT; x++) {
       			for (int y=0; y<WIDTH; y++) {
				if (x == snake[snake_len-1].x && y == snake[snake_len-1].y) {
					LCD_write_char(HEAD);
				} else if (screen[x][y] == SNAKE) {
					LCD_write_char(SNAKE);
				} else if (screen[x][y] == FOOD) {
					LCD_write_char(FOOD);
				} else {
					LCD_write_char(SPACE);
				}
			}
        	}

		_delay_ms(250);
	}
}

/*******************************************************************************

Program: Rolling LED Display
Purpose: Enable message to be displayed to 6 x 24 LED grid
Author:  Darren Conley (and Kyle Hajekerou)
Change log:
Darren Conley - March 6, 2017
1. First version of program written

Darren Conley - March 8, 2017
1. Displays single letter

Darren Conley - March 9, 2017
1. Corrected issue where all rows were displaying on row 0.
2. Corrected count-from-zero issue preventing final column of message
   from displaying properly.
3. Wrote code to enable scrolling.
   
Darren Conley - March 10, 2017
1. Corrected SHIFTDELAY value from 65000 to 20 to facilitate proper 
   scroll speed.
2. Added code to parse message into 6x6 display values.

****************************** include libraries *****************************/

#include <hidef.h>            /* common defines and macros */
#include "derivative.h"       /* derivative-specific definitions */
#include <string.h>           /* library for string functions */

/****************************** define constants *****************************/

#define DECODEROP 0b00000111  // first 3 ports for 3 bit line decoder input
#define SERIALOP 0b00000001   // serial output to shift registers
#define CLOCKOP 0b0100        // clock output for shift registers 
#define CLEAROP 0b0001        // output to clear shift registers
#define DISPLAYDELAY 1000     // value to set refresh rate for display
#define SHIFTDELAY 20         // value to set LED shift rate
#define TOTCOLS 24            // total columns in LED display 
#define TOTROWS 6             // total rows in LED display
#define LTRWDTH 6             // width of each letter in pixels (LEDs)
#define MESSAGELENGTH 28      // number of characters in displayed message

/***************************** declare variables *****************************/

char message[MESSAGELENGTH] = "THANK GOD ITS FRIDAY";

//array to output to display initialized to all zeros
unsigned char display[6][24] = { {0 } }; 

int iteration = 0;           //counter for tracking shift iteration number
unsigned int shift_time = 0; //counter for shift timing

/******************************** letter array *******************************/

unsigned char letter[27][6][6] = 
{
  {
  {0,0,0,1,0,0},
  {0,0,1,0,1,0},
  {0,1,0,0,0,1},
  {0,1,1,1,1,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1}
  },
  {
  {0,1,1,1,1,0},
  {0,1,0,0,0,1},
  {0,1,1,1,1,0},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,1,1,1,0},
  },
  {
  {0,0,1,1,1,1},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,0,1,1,1,1},
  },
  {
  {0,1,1,1,1,0},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,1,1,1,0},
  },
  {
  {0,1,1,1,1,1},
  {0,1,0,0,0,0},
  {0,1,1,1,1,1},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,1,1,1,1},
  },
  {
  {0,1,1,1,1,1},
  {0,1,0,0,0,0},
  {0,1,1,1,1,0},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  },
  {
  {0,0,1,1,1,1},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,0,1,1,1},
  {0,1,0,0,0,1},
  {0,0,1,1,1,1},
  },
  {
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,1,1,1,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  },
  {
  {0,1,1,1,1,1},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  {0,1,1,1,1,1},
  },
  {
  {0,0,1,1,1,1},
  {0,0,0,0,1,0},
  {0,0,0,0,1,0},
  {0,0,0,0,1,0},
  {0,1,0,0,1,0},
  {0,0,1,1,0,0},
  },
  {
  {0,1,0,0,1,0},
  {0,1,0,1,0,0},
  {0,1,1,0,0,0},
  {0,1,0,1,0,0},
  {0,1,0,0,1,0},
  {0,1,0,0,0,1},
  },
  {
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,1,1,1,1},
  },
  {
  {0,1,0,0,0,1},
  {0,1,1,0,1,1},
  {0,1,0,1,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  },
  {
  {0,1,0,0,0,1},
  {0,1,1,0,0,1},
  {0,1,0,1,0,1},
  {0,1,0,0,1,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  },
  {
  {0,0,1,1,1,0},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,0,1,1,1,0},
  },
  {
  {0,1,1,1,1,1},
  {0,1,0,0,0,1},
  {0,1,1,1,1,1},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  {0,1,0,0,0,0},
  },
  {
  {0,0,1,1,1,0},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,1,0,1},
  {0,1,0,0,1,0},
  {0,0,1,1,0,1},
  },
  {
  {0,1,1,1,1,0},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,1,1,1,0},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  },
  {
  {0,0,1,1,1,1},
  {0,1,0,0,0,0},
  {0,1,1,1,1,0},
  {0,0,0,0,0,1},
  {0,0,0,0,0,1},
  {0,1,1,1,1,0},
  },
  {
  {0,1,1,1,1,1},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  },
  {
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,0,1,1,1,0},
  },
  {
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,0,1,0,1,0},
  {0,0,1,0,1,0},
  {0,0,1,0,1,0},
  {0,0,0,1,0,0},
  },
  {
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,1,0,1,0,1},
  {0,1,0,1,0,1},
  {0,0,1,0,1,0},
  },
  {
  {0,1,0,0,0,1},
  {0,0,1,0,1,0},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  {0,0,1,0,1,0},
  {0,1,0,0,0,1},
  },
  {
  {0,1,0,0,0,1},
  {0,1,0,0,0,1},
  {0,0,1,0,1,0},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  {0,0,0,1,0,0},
  },
  {
  {0,1,1,1,1,1},
  {0,0,0,0,0,1},
  {0,0,0,0,1,0},
  {0,0,1,1,0,0},
  {0,1,0,0,0,0},
  {0,1,1,1,1,1},
  },
  {
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  {0,0,0,0,0,0},
  },
};	                            

/**************************** prototype functions ****************************/

void pulse_clock (void);     // cycles the clock once
void reset_registers (void); // clears data in the registers
void run_delay (int count);  // delay function for setting LED refresh rate
void output_row (int row);   // outputs one row of serial data to shift reg
void display_rows (void);    // displays each row on LED display

// translates message string to int array of letter values
void parse_message(int parsed[25], int tot_ltrs);

// creates the 24x6 array for each cycle
void build_display (int iteration, int parsed[25], int tot_pixels); 
	                            
/******************************* main function *******************************/
void main(void) {
  /* put your own code here */

unsigned int count = 0;
unsigned int tot_ltrs = strlen(message);
unsigned int tot_pixels = tot_ltrs * LTRWDTH;

int parsed[MESSAGELENGTH];

parse_message(parsed, tot_ltrs);

// initialize ports
DDRA = DECODEROP; // PA on hardware, PORTA in code, output to decoder
DDRB = SERIALOP;  // PB on hardware, PORTB in code, output for serial data
DDRS = CLOCKOP;   // PS2 on hardware, PTS in code, output for clock pulse
DDRP = CLEAROP;   // PP on hardware, PTP in code, output to clear

// repeat forever
while (TRUE) {

  // loop to rebuild display[][] with shifted values
  for (iteration = 0; iteration < tot_pixels + TOTCOLS; iteration++) 
  {
    build_display(iteration, parsed, tot_pixels);

    // loop to display current iteration before shifting
    // repeats until time to shift one pixel over
    for (shift_time = 0; shift_time < SHIFTDELAY; shift_time++)
    {
      display_rows(); //send data to each row and display it
    }
    
    //end loop to display current iteration before shifting
  
  }

  // end loop to rebuild display[][] with shifted values
  
}

// end loop to repeat forever

	EnableInterrupts;

  for(;;) {
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}

/****************************** other functions ******************************/

/******************************** pulse clock ********************************/
void pulse_clock (void)
{
  PTS = 0b100;
  PTS = 0b000;
}

/****************************** reset_registers ******************************/
void reset_registers (void)
{
  PTP = 0b0000; //clear data in serial ports
  PTP = 0b0001; //shut off clear for serial ports (enable receive data)
}

/******************************* build_display *******************************/
void build_display (int iteration, int parsed[25], int tot_pixels)
{
  unsigned int row; //counter for rows 
  unsigned int col; //counter for columns
  
  for (row = 0; row < TOTROWS; row++)
  {
    for (col = 0; col < TOTCOLS; col++)
    {
      if (col < (TOTCOLS - 1)) //shift values left for all columns except final column
      {
        display[row][col] = display[row][col+1];
      } 
      else 
      {
        if (iteration < tot_pixels)
        {
          display[row][TOTCOLS - 1] = letter[parsed[iteration/6]][row][iteration%6];
        } 
        else 
        {
          display[row][TOTCOLS - 1] = 0;
        }
      }
    }
  }
}

/******************************** run_delay **********************************/
void run_delay (int count)
{
  int loop;
  for (loop = 0; loop < count; loop++);
}

/******************************** output_row *********************************/
void output_row (int row)
{
  int count = 0;
  
  for (count = 0; count < TOTCOLS; count++)
  {
    PORTB = display[row][(TOTCOLS - 1) - count];
    pulse_clock();
  }
  pulse_clock();
 
}

/******************************* display_rows ********************************/
void display_rows (void) 
{

  unsigned int count = 0;
  
  for (count = 0; count < TOTROWS; count++) 
    {
      reset_registers();

      output_row(count);

      PORTA = count;
    
      run_delay(DISPLAYDELAY);
    }
}

/****************************** parse_message ********************************/
void parse_message(int parsed[25], int tot_ltrs)
{
  int count = 0;
  
  for (count = 0; count < tot_ltrs; count++)
  {
  if (message[count] - 'A' < 0 || message[count] - 'A' > 25)
    {
      parsed[count] = 26;
    }
    else
    {
      parsed[count] = (message[count] - 'A');
    }
  }
}
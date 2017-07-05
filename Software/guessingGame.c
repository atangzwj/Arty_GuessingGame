/*
 * guessingGame.c
 *
 *  Created on: Jun 20, 2017
 *      Author: Arvin Tang
 */

#include "guessingGame.h"

// Initialize Arty Board inputs and outputs
void initIO() {
   // Initialize XGpio structs
   xgpio0 = (XGpio*) calloc(1, sizeof(XGpio));
   xgpio1 = (XGpio*) calloc(1, sizeof(XGpio));
   XGpio_Initialize(xgpio0, 0);
   XGpio_Initialize(xgpio1, 1);

   // Initialize XGpio_Config structs
   xgpio_cfg0 = XGpio_LookupConfig(0);
   xgpio_cfg1 = XGpio_LookupConfig(1);

   // Set directions
   XGpio_SetDataDirection(xgpio0, RGB_LEDS_CHANNEL, 0); // Outputs
   XGpio_SetDataDirection(xgpio0, LEDS_CHANNEL, 0);

   XGpio_SetDataDirection(xgpio1, BTN_CHANNEL, 1); // Inputs
   XGpio_SetDataDirection(xgpio1, SW_CHANNEL, 1);
}

// Begins a guessing game
// Returns 0 if user wants to stop playing, otherwise returns 1
int guessingGame() {
   int num = rand() % 16;
   int chances = 4;
   print("I'm thinking of a number between 0 and 15, what is it?\n\r");
   print("Use the switches to enter a number\n\r");
   print("Press BTN0 to submit your guess\n\r");
   print("Press BTN3 to stop playing\n\n\r");
   xil_printf("You have %d chances remaining\n\r", chances);

   int guess_result = 1;
   displayChances(chances);
   while (chances > 0 && guess_result) {
      int guess = getGuess();

      // End game if getGuess() returned -1
      if (guess < 0) {
         return endGame();
      }

      guess_result = checkGuess(num, guess);
      chances -= guess_result;
      displayChances(chances);
      if (guess_result) {
         xil_printf("Chances remaining = %d\n\r", chances);
      }
   }

   if (!chances) {
      print("\nYou used all 4 chances\n\r");
      xil_printf("The number was %d\n\n\n\r", num);
      blinkLEDS();
   }

   print("Would you like to keep playing?\n\r");
   print("Press BTN0 to keep playing\n\r");
   print("Press BTN3 to stop\n\n\n\r");
   int keep_playing = getGuess();
   if (keep_playing < 0) {
      return endGame();
   }
   return 1;
}

// Returns user's guess taken from the switches
int getGuess() {
   int guess = READ_SW;
   int btn = READ_BTN;
   while (btn) {
      btn = READ_BTN; // Wait until user releases btn from previous press
      displayGuess(guess);
   }

   int detected = 0;
   while (!(btn && detected)) {
      btn = READ_BTN;
      guess = READ_SW;
      displayGuess(guess);
      for (int i = 0; i < 1000 && btn; i++) {
         btn = READ_BTN;
         detected = i == 999; // Count input as detected after 1000 iterations
      }
   }
   if (btn & 0x8) {
      return -1; // BTN3 was pressed, user wants to stop playing
   }
   return guess;
}

// Checks if num == guess. If equal, tell user they were correct. If not equal,
// tell user whether they guessed too high or too low
// Return 0 if guess was correct, 1 if incorrect
int checkGuess(int num, int guess) {
   if (num == guess) {
      xil_printf("\nCorrect! The number was %d\n\n\n\r", num);
      return 0;
   }
   char *hi_low = guess > num ? "high" : "low";
   xil_printf("Incorrect, %d was too %s\n\r", guess, hi_low);
   return 1;
}

// Write the current value entered on SW to RGB LEDs in blue
void displayGuess(int guess) {
   u16 rgb_leds = 0x000;
   u16 mask = 0x001;
   for (int i = 0; i < 4; i++) {
      mask <<= 3 - i;
      rgb_leds <<= 2;
      rgb_leds += guess & mask;
      mask = 0x1;
   }
   WRITE_RGB_LEDS(rgb_leds);
   WRITE_RGB_LEDS(0x000); // Modulate RGB LED brightness
}

// Display the number of chances left by lighting up the corresponding LEDs
// LEDs turn off from the left as chances decrease
void displayChances(int chances) {
   u16 leds = 0x0;
   for (int i = 0; i < chances; i++) {
      leds <<= 1;
      leds++;
   }
   WRITE_LEDS(leds);
}

// Display ending message and turn off all LEDs
// Return 0 to indicate user wants to end game
int endGame() {
   print("Thank you for playing!\n\r");
   displayChances(0);
   displayGuess(0);
   return 0;
}

// Flash all LEDS (RGB LEDS in red) 3 times
void blinkLEDS() {
   for (int i = 0; i < 3; i ++) {
      WRITE_LEDS(0xF);
      for (int j = 0; j < 8000; j++) {
         WRITE_RGB_LEDS(0x924);
         for (int k = 0; k < 50; k++) {
            WRITE_RGB_LEDS(0x000); // Modulate RGB LED brightness
         }
      }

      WRITE_LEDS(0x0);
      for (int j = 0; j < 280000; j++) {
         WRITE_RGB_LEDS(0x000);
      }
   }
}

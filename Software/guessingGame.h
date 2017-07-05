/*
 * guessingGame.h
 *
 *  Created on: Jun 20, 2017
 *      Author: Arvin Tang
 */

#ifndef SRC_GUESSINGGAME_H_
#define SRC_GUESSINGGAME_H_

#include <stdlib.h>
#include <stdio.h>
#include "xgpio.h"

#define RGB_LEDS_CHANNEL 1
#define LEDS_CHANNEL 2
#define BTN_CHANNEL 1
#define SW_CHANNEL 2

#define WRITE_RGB_LEDS(rgb) XGpio_DiscreteWrite(xgpio0, RGB_LEDS_CHANNEL, rgb)
#define WRITE_LEDS(led) XGpio_DiscreteWrite(xgpio0, LEDS_CHANNEL, led)

#define READ_BTN XGpio_DiscreteRead(xgpio1, BTN_CHANNEL) & 0x9 // BTN3 and BTN0
#define READ_SW XGpio_DiscreteRead(xgpio1, SW_CHANNEL)

XGpio *xgpio0; // RGB LEDs and LEDs
XGpio *xgpio1; // Buttons and Switches
XGpio_Config *xgpio_cfg0;
XGpio_Config *xgpio_cfg1;

void initIO();

int guessingGame();

int getGuess();

int checkGuess(int, int);

void displayGuess(int);

void displayChances(int);

int endGame();

void blinkLEDS();

#endif /* SRC_GUESSINGGAME_H_ */

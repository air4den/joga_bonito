#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
#include "images/start1.h"
#include "images/start2.h"
#include "images/pitch.h"
#include "images/win.h"
#include "images/lose.h"
#include "images/ball.h"
#include "images/goat.h"

// Add any additional states you need for your app. You are not requried to use
// these specific provided states.
enum gba_state {
  START,
  PLAY,
  WIN,
  LOSE,
};

int main(void) {
  // Manipulate REG_DISPCNT here to set Mode 3. //
  REG_DISPCNT = MODE3 | BG2_ENABLE;
  
  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  enum gba_state state = START;
  enum gba_state previousState = START; 

  u16 * startScreenState = (u16 *) start2;

  struct player player;

  struct ball pelota;

  u32 vBlankStart = vBlankCounter; 
  u32 dtime;
  u32 gameClock;

  char savedTimeToWinText[30];

  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons
    
    // Manipulate the state machine below as needed //
    // NOTE: Call waitForVBlank() before you draw
    if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)){
      previousState = state;
      state = START;
    }

    dtime = (vBlankCounter - vBlankStart)/10;
    gameClock = (dtime/6);

    

    // Start Screen animates every second (every 60 frames)
    if ((vBlankCounter%60) == 0) {
          startScreenState = (u16 *)((startScreenState == start2) ? start1 : start2);
    } 

    waitForVBlank();    // waiting for VBlank before drawing
    switch (state) {
      case START:

        drawFullScreenImageDMA(startScreenState);
        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons) != 0) {
          previousState = START;
          state = PLAY;
          break;
        }
        break;

      case PLAY:
        if (previousState != PLAY) {
          vBlankStart = vBlankCounter;    // set starting time = vBlankCounter

          // initial player + pelota pos/vel
          player.posX = 240/2 -7;
          player.posY = 64;
          player.velX = 0;
          player.velY = 0;

          pelota.posX = 240-37;
          pelota.posY = 67;
          pelota.velX = 0;
          pelota.velY = 0;

          drawFullScreenImageDMA(pitch);
          previousState = PLAY;
        }

        // Game Clock
        
        if (gameClock > 20) {
          state = LOSE;     // time ran out!
          break;
        }

        // Input Handling
        if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
          undrawImageDMA(player.posY, player.posX, GOAT_WIDTH, GOAT_HEIGHT, pitch);
          if (player.posX - 2 >= 11) {
            player.posX -= 2;
          }
        } 
        if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
          undrawImageDMA(player.posY, player.posX, GOAT_WIDTH, GOAT_HEIGHT, pitch);
          if (player.posX + GOAT_WIDTH + 2 <= 228) {
            player.posX += 2;
          }
        }
        if (KEY_DOWN(BUTTON_UP, currentButtons)) {
          undrawImageDMA(player.posY, player.posX, GOAT_WIDTH, GOAT_HEIGHT, pitch);
          if (player.posY - 2 >= 8) {
            player.posY -= 2;
          }
        }
        if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
          undrawImageDMA(player.posY, player.posX, GOAT_WIDTH, GOAT_HEIGHT, pitch);
          if (player.posY + GOAT_HEIGHT + 2 <= 135) {
            player.posY += 2;
          }
        }

        // Check Collision
        int collision = checkCollision(player.posX, player.posY, GOAT_WIDTH, GOAT_HEIGHT, pelota.posX, pelota.posY, BALL_WIDTH, BALL_HEIGHT);
        if (collision != 0) {
          state = WIN;
          sprintf(savedTimeToWinText, "You scored in %d seconds!", gameClock);
          previousState = PLAY;
          break;
        }

        drawBall(pelota.posX, pelota.posY, BALL_HEIGHT, BALL_WIDTH, ball);    // draw the ball
        drawBall(player.posX, player.posY, GOAT_WIDTH, GOAT_HEIGHT, goat);    // draw my GOAT Lionel Andres Messi Cuccitini

        // Draw Clock
        char text[2];
        sprintf(text, "%d", gameClock);
        undrawImageDMA(145,10, 6*2, 8, pitch);
        drawString(145, 10, text, WHITE);

        break;

      case WIN:
      if (previousState != WIN) {
        drawFullScreenImageDMA(win);
        drawString(55, 50, savedTimeToWinText, WHITE);
        previousState = WIN;
      }
        break;
        
      case LOSE:
        drawFullScreenImageDMA(lose);
        break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
  }


  return 0;
}

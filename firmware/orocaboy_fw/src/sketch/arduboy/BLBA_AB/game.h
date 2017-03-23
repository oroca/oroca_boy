#ifndef GAME_H
#define GAME_H

#include <Arduino.h>
#include "globals.h"
#include "playfield.h"

void stateMenuPlay()
{
  scorePlayer = 0;
  blobNumbers = 0;
  blobFrame = 0;
  initPlayfield();                        // let's clean up the playfield and start with a fresh one
  removeFlag();
  fillBlobPit();                          // fill the pit with random blobs.
  createCurrentBlobs();
  giveExtraScore = false;
  canMoveBlobsDown = true;
  showCombo = false;
  chain = 0;
  elfState = ELF_NORMAL;
  elfStressedFrame = 0;
  elfPausedFrame = 0;
  thumbsUpFrame = 0;
  gameState = STATE_GAME_PLAYING;
};


void stateGamePlaying()
{
  if (arduboy.everyXFrames(30))dropBlobs();
  updateStage();
  if (arduboy.justPressed(RIGHT_BUTTON)) moveBlobsRight();
  if (arduboy.justPressed(LEFT_BUTTON)) moveBlobsLeft();
  if (arduboy.justPressed(DOWN_BUTTON)) dropBlobs();
  if (arduboy.justPressed(B_BUTTON)) rotateBlobsRight();
  if (arduboy.justPressed(A_BUTTON)) rotateBlobsLeft();
  if (arduboy.justPressed(UP_BUTTON))
  {
    gameState = STATE_GAME_PAUSE;
    elfState = ELF_PAUSED;
  }
};

void stateGamePause()
{
  elfState = ELF_PAUSED;
  updateStage();
  if (arduboy.justPressed(UP_BUTTON))
  {
    gameState = STATE_GAME_PLAYING;
    elfState = ELF_NORMAL;
  }
};

void stateGameOver()
{
  drawDitherBackground();
  drawStressedElf();
  arduboy.drawRect(0, 0, 51, 64, WHITE);
  sprites.drawSelfMasked(4, 16, youLose, 0);
  arduboy.fillRect(4, 32, 43, 9, WHITE);
  scoreDraw(6, 34);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON))
  {
    gameState = STATE_MENU_MAIN;
  }
};

#endif

#pragma once

// Move the mouse to specific coordinates on the screen
bool MoveMouse(int x, int y);
bool SetMouse(int x, int y);

int GetScreenWidth();
int GetScreenHeight();

bool PrimaryDown();
bool PrimaryUp();
// Issue click with the primary button
bool PrimaryClick();

bool SecondaryDown();
bool SecondaryUp();
// Issue click with the secondary button
bool SecondaryClick();

bool MiddleDown();
bool MiddleUp();
bool MiddleClick();

bool VerticalScroll(int scrollAmt);

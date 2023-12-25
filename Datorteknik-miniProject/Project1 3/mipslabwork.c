
// Everything is written by Ermia & Teoman

#include <stdint.h>   // Standard library
#include <pic32mx.h>  // library fot the kit
#include "mipslab.h"  // Header file for all labs
#include <stdbool.h>  // Boolean library
unsigned char displaypixel[32][128]; // display pixels 32 x 128, Each unsigned char one byte
unsigned char displayByte[512];      // 512 bytes for the display, Each unsigned char one byte
int startThegame = 0; // will be used to start the game later
int timeoutcount = 0; // global variabel for interupt and timer 
volatile int* portE = (volatile int*) 0xbf886110; // pointing at the address of PortE

// A function that already exist from lab3, used for interupts
void user_isr( void )  // void function 
{ 
  if( IFS(0) & 0x100 ){  // if the flag & 100 on 
    timeoutcount++;        // tim... + 1
  IFSCLR(0)= 0x100;      // flag back to zero
  if(timeoutcount == 10){  // if timecount = 10 
    timeoutcount = 0;      // tim... = 0 
}
 return;      
}
}
// The function used in lab 3, enbaling all the timer and interupts 
void labinit( void )       // Void function
{
  IECSET(0) = 0x100; //enable
  IPCSET(2) = 0x1f; //highest prio
  volatile int* trisE = (volatile int*) 0xbf886100;
  *trisE = *trisE & 0xff00;    // We use  trisE so that 0-7 becomes 0 (output)
  TRISF &= 0x2; // Making Port F with 0010 in order to get it as an input
  TRISD = TRISD | 0xfe0;     //set bits 5 - 11 to 1 (inputs)
  T2CON = 0x70;   //Prescaling 1:256 (bits 4-6 should be 111, so that the value becomes  112)
  PR2 = 31250;  // ((80000000/256) / 10)set Period-Register to 10 ms with prescaling
  TMR2 = 0;                 //set Timer-Value to 0, for precise measurement
  T2CONSET = 0x8000;    //START TIMER - we set bit 15 in T2CON to '1', there is placed ON
  IECSET(0) = 2048;     //  Enable INT2 based on page 51 on IEC0 family data sheet (elventh, 0x800)
  IPCSET(2) = 7936;     // INT2 based on page 51 on IPC2 family data sheet (eleventh bit, 0x1f00)
  enableInterrupt();       //enable interrupt 
  return;
}

// Start of display activation 
// this function will take forwrd the position of the pixcels that should be turned one. it is a function
// that doesn't return any value. It will take forward the poisition of the pixel( both x and y). The 
// lenght of both x,y of the pixcels that should turn on. 
void turnOnPixel(int positionXPixel, int positionYpixel, int lenghtXpixel, int lenghtYpixel) 
{
  int line = -1; // Line meaning the 32 rows, will start with -1 to make zero later
  int vertical = -1; // meaning the vertical pixcels 128, will start with -1 to make zero later
  while(line <= 31)  // While lines are less than 31
  { 
    for(vertical = 0; vertical <= 127; ++vertical) // a for loop in the while loop, vertical +1 directly 
  { 
    if (line >= positionYpixel)                  // if line bigger or equal the Y-pixel given 
      if(line <= (positionYpixel + lenghtYpixel))  // If the line is less or equal to the addition of lenght and pizel given
        if(vertical >= positionXPixel)            // if vertical bigger or equal the x-pixel given
          if(vertical <= (positionXPixel + lenghtXpixel)){   // if vertical less than addition of xpixel and pixelxlenght
            displaypixel[line][vertical] = 1;             // if all the requirments above are valid, turn on the pixel
            }
  }
  ++line;   // line + 1 directly
  }
}


// The function will show the rectangel that will pop up on the screen. 
void drawRectangel(void){
  turnOnPixel(0,0,128,0);// Fixing the uper side 
  turnOnPixel(0,31,127,1); //Fixing the downer side
  turnOnPixel(0,0,0,32); // Fixing the left side
  turnOnPixel(127,0,1,32); // Fixing the right side
}

// This function well translate the information of the pixels 32 x 128(2D arrat) to an array (1D) of unsigned char
// The array contains of the data which transfer information to the display and whihc pixels are going to get on 
// It should be noted that on the function pixelToByte, we got help from people and Internet espacially older students 
// We understand fully the implimentation and worte the function and the logic by the understanding that we got 
// PixeltoByte is written fully by our own understanding and words. 
// from different sources 
// 512 bytes meaning 4096 bits that are 1 or 0. 
void pixelToByte()
{
  int screenPart1 = -1;  // Part1 of the y-axis of the screen 
  int screenPart2 = 0; // part2 of the y-axis
  int screenXpart = 0; // x-part of the screen 
  
  unsigned char pixelOne = 1;  // one Byte, turn on the pixel
  unsigned char pixelZero = 0; // one Byte, turn off the pixel

  while(screenPart1 < 4)  // We divide the screen y-axis to 4 
  {
    for (screenXpart = 0; screenXpart < 128; screenXpart++) // We divide the x-axiz of the screen to 128 parts 
    {
      pixelOne = 1;      // Turn on pixel --> make it to one
      pixelZero = 0;     // Turn off pixel --> make it to zero
        for (screenPart2 = 0; screenPart2 < 8; screenPart2++) // Divide the x-screen that is already divided in 4. 4 * 8 = 32
      {
          if (displaypixel[screenPart2 + 8 * screenPart1][screenXpart]) // if the pixel is 1 
        {
            pixelZero =  pixelZero | pixelOne; // Pixel zero are going to be the ones that we and the with the one already on 
        }
        pixelOne = (pixelOne << 1);          // Pixelone are going to be all the pixel one whihc will be shifted to öeft once
      }

      displayByte[screenPart1 * 128 + screenXpart] = pixelZero; // Transfering the pixels 32 x 128 to 512 bytes data
    }
  ++screenPart1; // +1, go to the next part of the screen 
  }
}

// this function will turn off all the pixels in order to for the ball and padel move and the pixels that have already been 
// On will trun off,  This function will be used later on in various place,  clear all everything in the arry and caliing on 
// the function 
void dottedLine(){
  displaypixel[4][64] = 1; // Making the doted line in the middle
  displaypixel[9][64] = 1;  // Making the doted line in the middle
  displaypixel[14][64] = 1; // Making the doted line in the middle
  displaypixel[19][64] = 1; // Making the doted line in the middle
  displaypixel[24][64] = 1; // Making the doted line in the middle
  displaypixel[29][64] = 1; // Making the doted line in the middle
  pixelToByte();  // after turning off all the pixels, transfer the data to the 512 byte info 
}

// this function will turn off the pixels of the ball. This function will make the ball able to move. The ball will move and
// The parvios pixels will later be turned off. The division off turnoffPixel is due to the fact that we do not want the 
// Pixels to be turned off unneceasery
void turnOffPixelBall()
{
  int i = 1;  //1 in order to not turn off the rectangel everytime
  int j = 1; // 1 in order to not turn off the rectangel everytime
  for (i = 1; i < 31; i++) // Going through all the pixels on the y-axis from 1 to 31( Not 0-32 to not turn off rectangle)
  {
    for( j= 4; j < 124; j++) // Going through all the pixels on the x-axis from 4 to 124( Not 0-128 to not turn off rectangle and padel)
    {
      displaypixel[i][j] = 0; // Clear all the pixels on the array
      displaypixel[4][64] = 1; // Making the doted line in the middle
      displaypixel[9][64] = 1;  // Making the doted line in the middle
      displaypixel[14][64] = 1; // Making the doted line in the middle
      displaypixel[19][64] = 1; // Making the doted line in the middle
      displaypixel[24][64] = 1; // Making the doted line in the middle
      displaypixel[29][64] = 1; // Making the doted line in the middle
    }  
  }
  pixelToByte();  // after turning off all the pixels, transfer the data to the 512 byte info 
}

// This function will turn off all the pixels in order to show the winner at the end. This function will even turn off the  
// rectangel, This function will be used later on in various place,  clear all everything in the arry and caliing on 
// the function 
void turnOffPixel2()
{
  int i = 0;  //0 in order to turn off the rectangel everytime
  int j = 0; // 0 in order to  turn off the rectangel everytime
  for (i = 0; i <= 32; i++) // Going through all the pixels on the y-axis from 1 to 32
  {
    for( j= 0; j <= 128; j++) // Going through all the pixels on the x-axis from 1 to 128
      displaypixel[i][j] = 0; // Clear all the pixels on the array
    }
  pixelToByte();  // after turning off all the pixels, transfer the data to the 512 byte info 
  
}
// this function will turn off the pixels of the padels. This function will make the padels able to move. The ball will
// move and The parvios pixels will later be turned off. The division off turnoffPixel is due to the fact
 //that we do not want the  Pixels to be turned off unneceasery
void turnOffPixelPadel(){
  int i = 1;  //1 in order to not turn off the rectangel everytime
  int j = 1; // 1 in order to not turn off the rectangel everytime
  for (i = 1; i < 31; i++) // Going through all the pixels on the x-axis from 1 to 127( Not 0-128 to not turn off rectangle)
  {
    for( j= 1; j < 5; j++) // Going through all the pixels on the y-axis from 1 to 31( Not 0-32 to not turn off rectangle)
    {
      displaypixel[i][j] = 0; // Clear all the pixels on the array
      
    }  
  }
  for (i = 1; i < 31; i++) // Going through all the pixels on the x-axis from 1 to 127( Not 0-128 to not turn off rectangle)
  {
    for( j= 124; j < 127; j++) // Going through all the pixels on the y-axis from 1 to 31( Not 0-32 to not turn off rectangle)
    {
      displaypixel[i][j] = 0; // Clear all the pixels on the array  
    }  
  }
  pixelToByte();  // after turning off all the pixels, transfer the data to the 512 byte info 
}

// End of display activation 
// Start of the logic for game
// this function identifies all the movements of the padels. Padels are controled using the buttons 1-4. If the buttons are
// pushed, the padels will go up and down in y-direstion. 
int leftPadelX = 1; // Set the x-position of the left padel on pixel x = 1(Vertical line)
int leftPadelY = 13;  //Set the y-position of the left padel on pixel y = 13 (horizontal line)
int rightPadelX = 124; // Set the x-position of the right padel on pixel x = 124(Vertical line)
int rightPadelY = 13;  // Set the y-position of the right padel on pixel y = 13(hoeizontal line)
int sizeOfPadel = 2;  // The x-size of the Padel should be 2/ it will turn on 2 pixels after on the x-axis
int padelLength = 6;  // Padel-length y-direstion is 6. Turn on 6 pixels up and down
void movementOfPadles(){
// Start of the logic for game
  if(getbtns() & 4) // Calling on function getbtns, and if buttin 4 is pushed
  {
    if(leftPadelY == 1){      // if the padel position becomes 1, the begining of the screen
      leftPadelY = leftPadelY;  // Stay on the same position. It will say that do not move 
    }
    else{                      // if we are not in that posiion(Position 1)
      leftPadelY -= 2;         // Go up by 2. The speed will be faster if 2 is changed to 3, and slower if 2 is changed to 1
    }                           // by the speed, it is meant the speed of padel's movement
  }
  if(getbtns() & 2)             // Calling on function getbtns, and if buttin 3 is pushed
  {
    if(leftPadelY == 25){        // if the padel position becomes 25(23+6), the end of the screen
      leftPadelY = leftPadelY; // Stay on the same position. It will say that do not move 
    }
    else{
      leftPadelY += 2;          // if we are not in that posiion(Position 23)
    }                        // go down by 2. The speed will be faster if 2 is changed to 3, and slower if 2 is changed to 1
  }                          // by the speed, it is meant the speed of padel's movement
  if(btn() & 1)              // Calling on function btn, and if buttin 1 is pushed
  {
    if(rightPadelY == 25){    // if the padel position becomes 25(23+8), the end of the screen
      rightPadelY = rightPadelY;  // Stay on the same position. It will say that do not move 
    }
    else{
      rightPadelY = rightPadelY + 2;  // if we are not in that posiion(Position 23)
    }                      // go down by 2. The speed will be faster if 2 is changed to 3, and slower if 2 is changed to 1
  }                        // by the speed, it is meant the speed of padel's movement
  if(getbtns() & 1) // Calling on function getbtns, and if buttin 2 is pushed
  {
    if(rightPadelY == 1){  // if the padel position becomes 1, the begining of the screen
      rightPadelY = rightPadelY; // Stay on the same position. It will say that do not move
    }
    else{
      rightPadelY -= 2; // if we are not in that posiion(Position 23)
    }                   // go down by 2. The speed will be faster if 2 is changed to 3, and slower if 2 is changed to 1
  }                     // by the speed, it is meant the speed of padel's movement
  //turnOffPixel(); // Truning off all the pixels, basiclly updating with every clock-cycle
  turnOffPixelPadel();
  turnOnPixel(leftPadelX, leftPadelY, sizeOfPadel, padelLength); // Turn on the pixels for the left padel
  turnOnPixel(rightPadelX, rightPadelY, sizeOfPadel, padelLength); // Turn on the pixels for the right padel 
  pixelToByte();  // Change the information of the pixel array to bytes of the string 
  display_image(0, displayByte);  // Display it as an image on the screen 
}
int scoreplayer1 = 0; // The initilization of player1 score
int scoreplayer2 = 0; // The initilization of player2 score
int ballXPostion = 63;   // Ball position, try to have it as middle as possible 
int sizeOfball = 2;      // Size of the ball 2, will later be printed on the display as a square 2 x 2
int ballYPostion = 15;   // Ball position 15 in order to be in the middle 
float ballYspeed = 1;          // Speed of the ball moving in Y-direstion 
float ballXspeed = -1;      // Speed of the ball moving in X direction, at the start, it will go to the right by 

// This function determines all the bounces with the upper and belower boundries. The function determines the collision 
// with the padels and with what speed and angels should the ball be bounced. Function contains whatever the ball does
// The functiojn will limit the speed and how the ball will move in x/y directions. 
// All the numbers and speeds have been calculated using mathematics. Calculations are not available here becuase of not 
// being able to put mathemacal equations and the time it consumes. 
// if you wonder about the calculations, please reach me by my mail ermiaghaffari2002@gmail.com 
void ballMangement2(){
  ballXPostion += (int) ballXspeed ; // X position of the ball is always the addition of the speed and the its x position 
  ballYPostion += (int) ballYspeed; // Xyposition of the ball is always the addition of the speed and the its y position 
  // it should be noted that floats will be converted to int in order to be able change the pixels 
  
  if(((ballXPostion) >= (rightPadelX -  5 ) ) && ((ballXPostion ) <= (rightPadelX  + 1))) { // Collision with right padel x-axis
    if(( (ballYPostion + 1)  > (rightPadelY - 2)) && ((ballYPostion + 1) < (rightPadelY + 8)) ) { // Collison with padel y-axis 
      if(ballYspeed > 0){ // if the ball comes down and it will meet the padel 
        ballXspeed *= -1.2;  // Change the x direction 
        ballYspeed = 1.8 * ballYspeed; // bounce in y direction 
      }
      if(ballYspeed < 0){ // If the ball comes up 
        ballXspeed *= -1.2;  // Change the x 
        ballYspeed = -1.8 * ballYspeed; // Send y down 
      }
    }
  } 
  
  if((ballXPostion >= (leftPadelX + 2 ) ) && ((ballXPostion - 1) <= (leftPadelX + 4) ) ) {// Collision with the left padel x-axis
    if((ballYPostion  >= (leftPadelY - 1)) && (ballYPostion < (leftPadelY + 7)) ) {  // Collison with padel y-axis 
      if(ballYspeed > 0){  // if the ball comes down and it will meet the padel 
        ballXspeed *= -1.2; // Change the x direction
        ballYspeed = 1.8 * ballYspeed; // bounce in y direction 
         
        }
        if(ballYspeed < 0){ // If the ball comes up 
            ballYspeed = -1.8 * ballYspeed;  // Send y down 
            ballXspeed *= -1.2; //Change the x
          }
    }
  }
  if((ballYPostion >= 29 ) && ((ballYPostion) <= 32)) { // down boundries and bounce 
    ballYspeed *= -1.3; // Y bounce, change the y-direction 
    ballXspeed *= 1.4;  // X bounce 

  }
  if((ballYPostion >= 0 ) && ((ballYPostion) <= 3)) { // upp boundries and bounce 
    ballYspeed *= -1.3; // Y bounce, change the y-direction 
    ballXspeed *= 1.4; // X bounce 
  }
  if (ballXspeed > 4){ // if the x-speed of the ball gets too much, slow down 
      ballXspeed = 1; // speed will become 1 
  }
  if (ballXspeed < -4){  // if the x-speed of the ball gets too much, slow down
      ballXspeed = -1;  // speed will become -1 
  }
  if (ballYspeed > 2){ // if the y-speed of the ball gets too much, slow down
      ballYspeed = 1; // speed will become 1 
  }
  if (ballYspeed < -2){ // if the y-speed of the ball gets too much, slow down
      ballYspeed = -1; // speed will become -1 
  }
  if ((ballXspeed == 0 )){ // if the ball doesn't move in x-direction ( Ball gets still, change xspeed to 1)
      ballXspeed = 1;  // x- Speed will be one
  }
  if ((ballYPostion < -1 ) || (ballYPostion > 33 ) ){ // if the ball passes by the boundries , bug and reset the game 
      resetGame();
  }
  if (ballYspeed > -0.20 && ballYspeed <= 0)   // if the ball gets stock on the roof or floor 
    {
      ballYspeed = -0.25;   // Change the  Y speed so that it can move again 
    }
    else if (ballYspeed >= 0 && ballYspeed < 0.20) // if the ball gets stock on the roof or floor 
    {
      ballYspeed = 0.25;  // Change the  Y speed so that it can move again 
    }

  resetGame(); // Calling on the function reset the game 
  turnOffPixelBall(); // Trun off the pixels for the ball 
  turnOnPixel(ballXPostion, ballYPostion,sizeOfball, sizeOfball);  // turn on the pixels for the ball 
  pixelToByte();  // Change the information of the pixel array to bytes of the string 
  display_image(0, displayByte);  // Display it as an image on the screen 
}
// Reset game says that if the ball passes by two specific x coordinates, reset the game. Reset the ball position and the padels 
void resetGame(){
  if(ballXPostion > 137 || ballXPostion < -4) // If the ball has a x-position greater than 137 or less than -4 
  {
    ballXPostion = 63; // Ball x position is going to get reset and be in the middle 
    ballYPostion = 15; // Ball y position is going to get reset and be in the middle 
    ballXspeed = -1; // Ball x speed is going to get reset and go to left 
    ballYspeed = -1; // Ball y position is going to get reset and be in the middle 
  }
}

int restart = 1; // Global variable restart will be used in labwork, it will have diffrent values to reseted 
// End of the game logic
// This function is basically the entire lab. Detlet this function and see what happens? :) whatever happens, happens here
void labwork( void ) {
 
   if(getsw() & 1){  // If the switch is On, go forward
    clearString();    // Clear everything, taken from labfunc
    delay(4500);      // a delay of 4,5 sec
    startThegame = 2; // Start the game will become 1
  } 

  if(startThegame == 2){  // If the game is going to be started
    dottedLine();        // Draw the dotted line 
    drawRectangel();     // draw the rectangel   
    movementOfPadles();  // Movement of the padels 
    ballMangement2(); // Call on the function for the ball 
    pixelToByte();   // Translate it to the pixels on the display
    display_image(0, displayByte); // Put the image on the screen 
    }
  if(ballXPostion < -2){ // if the ball position is less than -2 meaning went outta display 
    startThegame = 3;   // StartThegmae will be 3, the game will not be played anymore 
    restart = 2;        // Restart will be 2 
    turnOffPixel2();    // Turn off whatever pixel that is on 
    
  }
  if(restart == 2){     // If restart is 2 
    display_string(2, "    Right Wins ");  // Right wins becuase the ball passed the display on the left
    display_update();        // Update the screen that right wins will be up the screeen 
    
  }
  if(restart == 3){     // if restart is 3 
    display_string(2, "    Left Wins "); // Left wins becuase the ball passed the display on the  right 
    display_update();  // / Update the screen that left wins will be up the screen 
  }
  if(ballXPostion > 130){ // if the ball passes by the right hand side of the display, the position will be more than 130 
    startThegame = 3; // start the game will be 1 
    restart = 3; // Restart will be equal to 3 
    turnOffPixel2(); // Turn off all the pixels 
  }  
  if(getsw() & 2){  // If the switch is On, go forward
    startThegame = 2; // start the game will be 2 
    restart = 1;  // Restart will be equal to  one 
    clearString(); // Clear all the strings 
    // This will basically reset the entire game 
  } 
}
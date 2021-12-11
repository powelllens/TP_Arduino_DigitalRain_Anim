/*
  DigitalRainAnim.h - Library for Digital Rain Animation for TFT_eSPI Using ESP32, ESP8266.
  Created by Eric Nam, November 08, 2021.
  Released into the public domain.
*/

#ifndef DigitalRainAnim_h
#define DigitalRainAnim_h

#include <vector>
#include <string>
#include "led-matrix.h"
#include <graphics.h>
#include <chrono>

#define FONT_SIZE 2              //set font size 2
#define LINE_WIDTH 5             //width for font size 2
#define LETTER_HEIGHT 7          //height for font size 2
#define KEY_RESET_TIME 60 * 1000 //1 Min reset time

using namespace rgb_matrix;
using namespace std;

class DigitalRainAnim
{
public:
	DigitalRainAnim(FrameCanvas *offscreen_) :anim_offscreencanvas(offscreen_)
	{

	}
	void init(FrameCanvas *offscreen, Font* font, bool alphabetOnly = false);          //initialization
	void init(FrameCanvas *offscreen, Font* font, int new_line_len_min, int new_line_len_max, int new_line_speed_min, int new_line_speed_max, int new_timeFrame, bool alphabetOnly = false); //initialization with parameters
	bool loop();                         //loop animation
	void pause();                        //pause animation 
	void resume();                       //resume animation
	string getKey(int key_length);  //generating new key
	void resetKey();
	void setHeadCharColor(uint8_t red, uint8_t green, uint8_t blue); //set Head Char Color
	void setTextColor(uint8_t red, uint8_t green, uint8_t blue); //set Text Color
	void setBGColor(uint8_t red, uint8_t green, uint8_t blue); //set BG Color
	
private:
	FrameCanvas *anim_offscreencanvas; //target matrix
	Font* anim_font;			   //font for rgb matrix
	int line_len_min;              //minimum length of characters 
	int line_len_max;              //maximum length of characters 
	int line_speed_min;            //minimum vertical move speed
	int line_speed_max;            //maximum vertical move speed
	int width, height;             //width, height of display
	int numOfline;                 //number of calculated row
	int timeFrame;                 //time frame for drawing
	uint8_t fontSize;              //default font size 2
	uint8_t lineWidth;             //default line width
	uint8_t letterHeight;          //default letter height
	bool isPlaying;                //boolean for play or pause
	bool isAlphabetOnly;           //boolean for showing Alphabet only
	uint32_t lastDrawTime;         //checking last drawing time
	uint32_t lastUpdatedKeyTime;   //checking last generating key time
	Color headCharColor;           //having a text color  
	Color textColor;               //having a text color  
	Color bgColor;                 //having a bg color  
	vector<int> line_length;  //dynamic array for each line of vertical length
	vector<int> line_pos;     //dynamic array for eacg line Y position
	vector<int> line_speed;   //dynamic array for eacg line speed
	string keyString;	      //storing generated key
	string SecretKeys[30];
	int max_brigthness;
	unsigned long lasttime;

	void prepareAnim();                 //the function is to prepare all lines at the beginning
	void lineUpdate(int lineNum);       //the function is to update each line after disappeared from the screen
	void lineAnimation(int lineNum);    //the function is to update each line
	char getASCIIChar();         //the function is to get the random ASCII char 
	char getAbcASCIIChar();      //the function is to get only alphabet lowercase and uppercase
	int setYPos(int lineLen);           //the function is to update the position of line
	int getRandomNum(int min, int max); //the function is to get the random number (including max)
	int randomgen(int min, int max);    //the function is to get the random number (including max)
	long GetMillis();					//get current milliseconds
	int map(int value, int min_in, int max_in, int min_out, int max_out);
	Color luminance(Color color, uint8_t luminance); //the function is to get a color with luminance
	void CheckTime();
};

#endif

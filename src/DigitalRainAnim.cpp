/*
  DigitalRainAnim.h - Library for Digital Rain Animation for TFT_eSPI Using ESP32, ESP8266.
  Created by Eric Nam, November 08, 2021.
  Released into the public domain.
*/

#include "DigitalRainAnim.h"
#include <string.h>
#include <time.h>

using namespace rgb_matrix;
using namespace std;


//initialze
void DigitalRainAnim::init(FrameCanvas *offscreen, Font* font, bool alphabetOnly)
{
	anim_offscreencanvas = offscreen;
	anim_font = font;
	line_len_min = 3;
	line_len_max = 15;
	line_speed_min = 2;
	line_speed_max = 8;
	max_brigthness = 150;
	timeFrame = 200;
	isAlphabetOnly = alphabetOnly;
	fontSize = FONT_SIZE;
	lineWidth = LINE_WIDTH;
	letterHeight = LETTER_HEIGHT;
	this->prepareAnim();
}

//initialze with params
void DigitalRainAnim::init(FrameCanvas *offscreen,Font* font, int new_line_len_min, int new_line_len_max, int new_line_speed_min, int new_line_speed_max, int new_timeFrame, bool alphabetOnly)
{
	anim_offscreencanvas = offscreen;
	anim_font = font;
	line_len_min = new_line_len_min;
	line_len_max = new_line_len_max;
	line_speed_min = new_line_speed_min;
	line_speed_max = new_line_speed_max;
	timeFrame = new_timeFrame;
	isAlphabetOnly = alphabetOnly;
	fontSize = FONT_SIZE;
	lineWidth = LINE_WIDTH;
	letterHeight = LETTER_HEIGHT;
	this->prepareAnim();
}

//checking how many lines it can draw from the width of the screen.
//the size of the array is determined by the number of lines.
void DigitalRainAnim::prepareAnim()
{

	this->setHeadCharColor(max_brigthness, max_brigthness, max_brigthness);
	this->setTextColor(0, max_brigthness, 0);
	this->setBGColor(0, 0, 0);
	lastDrawTime = this->GetMillis() - timeFrame;
	width = anim_offscreencanvas->width(); // tft_DigitalRainAnim->width();
	height = anim_offscreencanvas->height(); // tft_DigitalRainAnim->height();

	numOfline = width / lineWidth;

	for (int i = 0; i < numOfline; i++) {
		line_length.push_back(this->getRandomNum(line_len_min, line_len_max));
		line_pos.push_back(this->setYPos(line_length[i]) - letterHeight);
		line_speed.push_back(this->getRandomNum(line_speed_min, line_speed_max));
	}

	isPlaying = true;
	lastUpdatedKeyTime = this->GetMillis() - timeFrame;
	srand(time(NULL));

	SecretKeys[0] = "Secret Text 1";
	SecretKeys[1] = "Secret Text 2";
	SecretKeys[2] = "Secret Text 3";
	SecretKeys[3] = "Secret Text 4";
	SecretKeys[4] = "Secret Text 5";
	SecretKeys[5] = "Secret Text 6";
	SecretKeys[6] = "Secret Text 7";
	SecretKeys[7] = "Secret Text 8";
	SecretKeys[8] = "Secret Text 9";
	SecretKeys[9] = "Secret Text 10";
	SecretKeys[10] = "Secret Text 11";
	SecretKeys[11] = "Secret Text 12";
	SecretKeys[12] = "Secret Text 13";
	SecretKeys[13] = "Secret Text 14";

}

//updating each line with a new length, Y position, and speed.
void DigitalRainAnim::lineUpdate(int lineNum) {
	line_length[lineNum] = this->getRandomNum(line_len_min, line_len_max);
	line_pos[lineNum] = this->setYPos(line_length[lineNum]);
	line_speed[lineNum] = this->getRandomNum(line_speed_min, line_speed_max);
}

//while moving vertically, the color value changes and the character changes as well.
//if a random key is generated, switch to red.
void DigitalRainAnim::lineAnimation(int lineNum)
{
	const size_t arraySize = 2;  // Avoid magic numbers!
	char text_buffer[arraySize];
	memset(&(text_buffer[0]), 0, arraySize);
	int letter_spacing = 0;
	int startX = lineNum * lineWidth;
	int currentY = -letterHeight;
	
	bool isKeyMode = keyString.length() > 0;

	for (int i = 0; i < line_length[lineNum]; i++) {
		int colorVal = map(i, 0, line_length[lineNum], 10, max_brigthness);
		
		text_buffer[0] = (char)(isAlphabetOnly ? this->getAbcASCIIChar() : this->getASCIIChar());

		DrawText(anim_offscreencanvas, *anim_font, startX, line_pos[lineNum] + currentY,
			(isKeyMode ?  Color(colorVal,0,0) : this->luminance(textColor, colorVal)), NULL, text_buffer, letter_spacing);

		currentY = (i * letterHeight);
	}

	if (keyString.length() > lineNum) {
		char _char = keyString.at(lineNum);
		const char *keyChar = &_char;
		text_buffer[0] = *keyChar;
		DrawText(anim_offscreencanvas, *anim_font, startX, line_pos[lineNum] + currentY,
			headCharColor, NULL, text_buffer, letter_spacing);
	}
	else {
		text_buffer[0] = (isAlphabetOnly ? this->getAbcASCIIChar() : this->getASCIIChar());
		DrawText(anim_offscreencanvas, *anim_font, startX, line_pos[lineNum] + currentY,
			textColor, NULL, text_buffer, letter_spacing);
	}

	line_pos[lineNum] += line_speed[lineNum];

	if (line_pos[lineNum] >= height) {
		this->lineUpdate(lineNum);
	}
}

bool DigitalRainAnim::loop() {
	
	uint32_t currentTime = this->GetMillis();
	if (((currentTime - lastUpdatedKeyTime) > KEY_RESET_TIME)) {
		this->resetKey();
	}
	//this->CheckTime();
	if (((currentTime - lastDrawTime) < timeFrame)) {
		return false;
	}
	
	if (isPlaying) {
		anim_offscreencanvas->Fill(bgColor.r, bgColor.g, bgColor.b);
		if (randomgen(0, 1000) == 0) {
			this->getKey(35);
		}
		for (int i = 0; i < numOfline; i++) this->lineAnimation(i);
	}
	lastDrawTime = currentTime;
	return true;
}

//a function to stop animation.
void DigitalRainAnim::pause() {
	isPlaying = false;
}

//a function to resume animation.
void DigitalRainAnim::resume() {
	isPlaying = true;
}

void DigitalRainAnim::CheckTime() {
	unsigned long timediff;
	unsigned long acttime = this->GetMillis();
	timediff = acttime - lasttime;
	lasttime = acttime;
	fprintf(stderr, "Time: '%lu'\n", timediff);
}

//a function that gets randomly from ASCII code 33 to 126.
char DigitalRainAnim::getASCIIChar()
{
	char charreturn;
	charreturn = (char)(randomgen(33, 127));
	//fprintf(stderr, "Time: '%c'\n", charreturn);
	return charreturn;
}

long DigitalRainAnim::GetMillis() {
	using namespace chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

//a function that gets only alphabets from ASCII code.
char DigitalRainAnim::getAbcASCIIChar()
{
	char charreturn;
	charreturn = (char)(randomgen(0, 2) == 0 ? randomgen(65, 91) : randomgen(97, 123));
	return charreturn;
}

//move the position to start from out of the screen.
int DigitalRainAnim::setYPos(int lineLen) {
	return lineLen * -20;
}

int DigitalRainAnim::map(int value, int min_in, int max_in, int min_out, int max_out) {
	int m;
	int b;

	m = (min_out - max_out) / (min_in - max_in);
	b = min_out - (m * min_in);

	return m * value + b;
}

int DigitalRainAnim::randomgen(int min, int max) //Pass in range
{
	return min + (rand() % (max - min));
}

//the function is to get the random number (including max)
int DigitalRainAnim::getRandomNum(int min, int max) {
	return randomgen(min, max + 1);
}

string DigitalRainAnim::getKey(int key_length) {
	this->resetKey();
	int maxKeyLength = (key_length > 0 ? (key_length > numOfline ? numOfline : key_length) : numOfline);

	string tmptext;

	for (int i = 0; i < maxKeyLength; i++) {
		tmptext = this->getAbcASCIIChar();
		keyString.append(tmptext.c_str());
	}

	if (randomgen(0, 14) != 0) {
		keyString = SecretKeys[randomgen(0, 13)];
	}

	return keyString;
}

//the function is to remove the generated key
void DigitalRainAnim::resetKey() {
	keyString = "";
	lastUpdatedKeyTime = GetMillis();
}

//set Head Char Color
void DigitalRainAnim::setHeadCharColor(uint8_t red, uint8_t green, uint8_t blue) {
	headCharColor = Color(red, green, blue);
}

//set Text Color
void DigitalRainAnim::setTextColor(uint8_t red, uint8_t green, uint8_t blue) {
	textColor = Color(red, green, blue);
}

//set BG Color
void DigitalRainAnim::setBGColor(uint8_t red, uint8_t green, uint8_t blue) {
	bgColor = Color(red, green, blue);
}

Color DigitalRainAnim::luminance(Color color, uint8_t luminance)
{
	Color out_color(color.r * luminance / 255, color.g * luminance / 255, color.b * luminance / 255);
	return out_color;
}

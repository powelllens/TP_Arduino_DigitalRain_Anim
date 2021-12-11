// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Example of a clock. This is very similar to the text-example,
// except that it shows the time :)
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "graphics.h"

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <DigitalRainAnim.h>

#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
	interrupt_received = true;
}

string getIP() {
	struct ifaddrs * ifAddrStruct = NULL;
	struct ifaddrs * ifa = NULL;
	void * tmpAddrPtr = NULL;
	string IPAddress = "0.0.0.0";

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			printf("'%s': %s\n", ifa->ifa_name, addressBuffer);
			IPAddress = addressBuffer;
		}
	}
	if (ifAddrStruct != NULL)
		freeifaddrs(ifAddrStruct);//remember to free ifAddrStruct
	return IPAddress;
}

int main() {

	RGBMatrix::Options matrix_options;
	matrix_options.hardware_mapping = "adafruit-hat";  // or e.g. "adafruit-hat"
	matrix_options.rows = 32;
	matrix_options.chain_length = 6;
	matrix_options.parallel = 1;
	matrix_options.show_refresh_rate = false;
	matrix_options.pwm_lsb_nanoseconds = 200;

	rgb_matrix::RuntimeOptions runtime_opt;
	runtime_opt.gpio_slowdown = 2;

	const char* time_format = "%H:%M";
	Color defaultcolor(50, 50, 50);
	Color clockcolor(00, 80, 00);
	Color bg_color(0, 0, 0);
	Color outline_color(0, 0, 0);
	bool with_outline = false;

	const char* bdf_font_file = NULL;
	const char* bdf_font_big_file = NULL;
	int x_orig = 0;
	int y_orig = 0;
	int brightness = 100;
	int letter_spacing = 0;

	char ExecPath[1024];
	getcwd(ExecPath, 1024);

	fprintf(stderr, "Exec Path '%s'\n", ExecPath);

	bdf_font_file = "/home/pi/rpi-rgb-led-matrix/fonts/5x7.bdf";
	bdf_font_big_file = "/home/pi/rpi-rgb-led-matrix/fonts/8x13B.bdf";


	/*
	 * Load font. This needs to be a filename with a bdf bitmap font.
	 */
	rgb_matrix::Font font;
	rgb_matrix::Font font_big;
	if (!font.LoadFont(bdf_font_file) || !font_big.LoadFont(bdf_font_big_file)) {
		fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
		fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_big_file);
		return 1;
	}
	rgb_matrix::Font* outline_font = NULL;
	if (with_outline) {
		outline_font = font.CreateOutlineFont();
	}

	RGBMatrix* matrix = rgb_matrix::CreateMatrixFromOptions(matrix_options,
		runtime_opt);
	if (matrix == NULL)
		return 1;

	matrix->SetBrightness(brightness);

	FrameCanvas* offscreen = matrix->CreateFrameCanvas();
	FrameCanvas* onscreen = matrix->CreateFrameCanvas();


	signal(SIGTERM, InterruptHandler);
	signal(SIGINT, InterruptHandler);

	DigitalRainAnim* digitalRainAnim = new DigitalRainAnim(offscreen);

	digitalRainAnim->init(offscreen, &font, false);

	const size_t arraySize = 256;  // Avoid magic numbers!
	char text_buffer[arraySize];

	offscreen->set_luminance_correct(true);

	string Pi_IP_Address = getIP();

	while (Pi_IP_Address == "127.0.0.1" && !interrupt_received) {

		strcpy(text_buffer, "Wait for IP-v4 Address!");
		rgb_matrix::DrawText(offscreen, font_big, 2, 10 + font_big.baseline(),
			defaultcolor, NULL, text_buffer,
			letter_spacing);

		onscreen = offscreen;
		onscreen = matrix->SwapOnVSync(onscreen);

		sleep(1);

		Pi_IP_Address = getIP();
	}

	if (interrupt_received) {
		// Finished. Shut down the RGB matrix.
		matrix->Clear();
		delete matrix;

		write(STDOUT_FILENO, "\n", 1);  // Create a fresh new line after ^C on screen
		return 0;
	}

	offscreen->Clear();

	strcpy(text_buffer, Pi_IP_Address.c_str());
	rgb_matrix::DrawText(offscreen, font_big, 16, 10 + font_big.baseline(),
		defaultcolor, NULL, text_buffer,
		letter_spacing);

	onscreen = offscreen;
	onscreen = matrix->SwapOnVSync(onscreen);

	sleep(10);

	bool new_anim;
	auto t = time(nullptr);
	auto tm = localtime(&t);

	while (!interrupt_received) {

		t = time(nullptr);
		tm = localtime(&t);
		
		memset(&(text_buffer[0]), 0, arraySize);
		strftime(text_buffer, sizeof(text_buffer), "%H:%M", tm); //"%a %b %d %H:%M:%S %Y"

		if (tm->tm_hour >= 7 && tm->tm_hour < 17 && tm->tm_wday < 6) {
			new_anim = digitalRainAnim->loop();
			clockcolor = Color(0, 150, 0);
		}
		else {
			offscreen->Clear();
			new_anim = true;
			clockcolor = defaultcolor;
		}

		if (new_anim) {
			rgb_matrix::DrawText(offscreen, font_big, 16, 10 + font_big.baseline(),
				clockcolor, NULL, text_buffer,
				letter_spacing);
			onscreen = matrix->SwapOnVSync(offscreen);
		}
	}

	// Finished. Shut down the RGB matrix.
	matrix->Clear();
	delete matrix;

	write(STDOUT_FILENO, "\n", 1);  // Create a fresh new line after ^C on screen
	return 0;
}

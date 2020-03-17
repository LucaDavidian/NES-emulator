#include "screen.hpp"

void Screen::SetPixel(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue)
{
	if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight)
	{
		buffer[y][x][0] = red;
		buffer[y][x][1] = green;
		buffer[y][x][2] = blue;
	}	
}

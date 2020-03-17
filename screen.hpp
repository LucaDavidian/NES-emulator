#ifndef SCREEN_H
#define SCREEN_H

#include <cstdint>
#include <cstddef>

class Screen
{
private:
	//enum { SCREEN_WIDTH = 256, SCREEN_HEIGHT = 240 };  // enum hack
	static const std::size_t screenWidth = 256;
	static const std::size_t screenHeight = 240;
public:
	void SetPixel(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue);
	
	const uint8_t (&GetBuffer() const)[screenHeight][screenWidth][3] { return buffer; }

	constexpr std::size_t GetWidth() const { return screenWidth; }
	constexpr std::size_t GetHeight() const { return screenHeight; }
private:
	uint8_t buffer[screenHeight][screenWidth][3];
};

#endif  // SCREEN_H

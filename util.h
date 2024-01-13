#pragma once

namespace util
{
	// Try to use standard cpp swap function (it may be faster?)
	template<typename T> void swap(T& t1, T& t2)
	{
		T temp = t1;
		t1 = t2;
		t2 = temp;
	}

	// Clamp for int
	inline int clamp(int x, int min, int max)
	{
		if (x < min) x = min;
		if (x > max) x = max;
		return x;
	}

	// Clamp for float
	inline float clampf(float n, float min, float max)
	{
		return n <= min ? min : n >= max ? max : n;
	}

	// For reading a file from its path
	std::string read(std::string fileName)
	{
		std::ifstream t(fileName);
		std::stringstream buffer;
		buffer << t.rdbuf();
		return buffer.str();
	}

	// Returns an insigned int in the form AGBR with the given r, g, b, and a values
	unsigned int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		return a << 24 | b << 16 | g << 8 | r;
	}

	unsigned int RGBA(unsigned int hex)
	{
		unsigned char r = (hex >> 24) & 0xff;
		unsigned char g = (hex >> 16) & 0xff;
		unsigned char b = (hex >> 8) & 0xff;
		unsigned char a = hex & 0xff;
		
		return a << 24 | b << 16 | g << 8 | r;
	}

	// Interpolating two colors and returning the interpolated color 
	unsigned int interpolate(unsigned int color1, unsigned int color2, float fraction)
	{
		unsigned char a1 = (color1 >> 24) & 0xff;
		unsigned char a2 = (color2 >> 24) & 0xff;
		unsigned char b1 = (color1 >> 16) & 0xff;
		unsigned char b2 = (color2 >> 16) & 0xff;
		unsigned char g1 = (color1 >> 8) & 0xff;
		unsigned char g2 = (color2 >> 8) & 0xff;
		unsigned char r1 = color1 & 0xff;
		unsigned char r2 = color2 & 0xff;

		return	(unsigned int)((a2 - a1) * fraction + a1) << 24 |
			(unsigned int)((b2 - b1) * fraction + b1) << 16 |
			(unsigned int)((g2 - g1) * fraction + g1) << 8 |
			(unsigned int)((r2 - r1) * fraction + r1);
	}

	// Initial key for randXOR method
	static unsigned int x = 123456789, y = 362436069, z = 521288629;

	// Random number generator that returns a number from 0 to unsigned int max
	unsigned int randXOR()
	{
		// Uses the xorshf96 algorithm
		// period 2^32-1

		unsigned int t;
		x ^= x << 16;
		x ^= x >> 5;
		x ^= x << 1;

		t = x;
		x = y;
		y = z;
		z = t ^ x ^ y;

		return z;
	}

	// Faster interpolation than with float
	unsigned int interpolate(unsigned int color1, unsigned int color2)
	{
		unsigned int scale = randXOR() & 0xff;

		unsigned char a1 = (color1 >> 24) & 0xff;
		unsigned char a2 = (color2 >> 24) & 0xff;
		unsigned char b1 = (color1 >> 16) & 0xff;
		unsigned char b2 = (color2 >> 16) & 0xff;
		unsigned char g1 = (color1 >> 8) & 0xff;
		unsigned char g2 = (color2 >> 8) & 0xff;
		unsigned char r1 = color1 & 0xff;
		unsigned char r2 = color2 & 0xff;

		return	((((a2 - a1) * scale) >> 8) + a1) << 24 |
			((((b2 - b1) * scale) >> 8) + b1) << 16 |
			((((g2 - g1) * scale) >> 8) + g1) << 8 |
			((((r2 - r1) * scale) >> 8) + r1);
	}

	// Array of choices for the direction method (10 choices of 0 : 11 choices of -1 : 11 choices of 1)
	static unsigned long long a = 0b01010101010101010101'0000000000000000000000'1010101010101010101010u;

	// Randomly generates a number thats either -1, 0, or 1
	int direction()
	{
		int index = randXOR() & 0b111110;

		return ((a >> index) & 3) - 1;
	}


	// This might be better for the direction() method because its smaller
	//static unsigned int b = 0b0101010101'000000000000'1010101010u;=

	// Randomly generates a number thats either -1, or 1
	int direction2()
	{
		return 2 * (randXOR() & 1) - 1;
	}
}
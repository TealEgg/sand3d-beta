#pragma once


namespace game
{
	// Strucutre for iterating over x,y, and z span with a single for loop
	struct index3d
	{
		unsigned int x = 0, y = 0, z = 0;
	    unsigned int xSize, ySize, zSize;
		unsigned int index = 0;

		index3d(unsigned int size) // This is for if all sizes are the same (aka size ^ 3)
		{
			this->xSize = size;
			this->ySize = size;
			this->zSize = size;
		}

		index3d(unsigned int xSize, unsigned int ySize, unsigned int zSize)
		{
			this->xSize = xSize;
			this->ySize = ySize;
			this->zSize = zSize;
		}

		void operator++(int)
		{
			x++;
			index++;
			if (x == xSize) { x = 0; y++; }
			if (y == ySize) { y = 0; z++; }
		}

		bool finished() { return z == zSize; }
	};

	/*struct clippedIndex3d
	{
		unsigned int x, y, z;
		unsigned int endX, endY, endZ;
		unsigned int size;
		unsigned int index;

		clippedIndex3d(unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int endX, unsigned int endY, unsigned int endZ, unsigned int size)
		{
			x = startX;
			y = startY;
			z = startZ;
			this->endX = endX;
			this->endY = endY;
			this->endZ = endZ;
			this->size = size;

			index = x + size * (y + size * z);
		}

		void operator++(int)
		{
			x++;
			index++;
			if (x == endX) { x = 0; y++; index = x + size * (y + size * z); }
			if (y == endY) { y = 0; z++; index = x + size * (y + size * z); }
		}

		bool finished() { return z == endZ; }
	};*/

	// Structure of elements
	struct element
	{
		unsigned long long elementID : 11;
		unsigned long long clock : 1;

		//	 ┌───────────┬──────────────────┐
		//	 │ Registers │     Values       │
		//	 ├───────────┼──────────────────┤
		//	 │  A-F (6)  │ 5 bits (0 - 31)  │
		//	 │  G-H (2)  │ 8 bits (0 - 255) │
		//	 │  I-K (3)  │ 1 bits (0 - 1)	│
		//	 │   L  (1)  │ 3 bits (0 - 7)   │
		//	 └───────────┴──────────────────┘ 

		unsigned long long registerA : 5; // 0 - 31
		unsigned long long registerB : 5; // 0 - 31
		unsigned long long registerC : 5; // 0 - 31
		unsigned long long registerD : 5; // 0 - 31
		unsigned long long registerE : 5; // 0 - 31
		unsigned long long registerF : 5; // 0 - 31

		unsigned long long registerG : 8; // 0 - 255
		unsigned long long registerH : 8; // 0 - 255

		unsigned long long registerI : 1; // 0 - 1
		unsigned long long registerJ : 1; // 0 - 1
		unsigned long long registerK : 1; // 0 - 1

		unsigned long long registerL : 3; // 0 - 7
	};

	// Empty element struct to clear an element in elementData if needed
	static const element emptyElement = {0};

	// How the element is placed into the world
	enum placement
	{
		hit, // Places the element where the raycast hits the face of another element
		player, // Places the element at the players position
	};

	// Element Catergories
	enum categories
	{
		solid,
		liquid,
		gas,
		life,
		energy,
		destruction,
		electricity,
		misc
	};

	// Structure to hold element name and description
	struct elementDescription
	{
		// The name of the element
		std::string name;

		// The description of the element
		std::string description;

		// The category of the element
		categories category;

		// If element is hidden in element selection menu;
		bool hidden = false;
	};

	// Structure of element definition
	struct elementDefinition
	{
		// Function pointers for particle init and update and reaction
		void (*init)(unsigned int, unsigned int, unsigned int);
		void (*update)(unsigned int, unsigned int, unsigned int);
		bool (*reaction)(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
		
		// These two colors will be interpolated when decided the elements color
		unsigned int color1;
		unsigned int color2;

		// The Denisty of the element 
		// Note: to make sure all elements have an integer represetation of the density multiply the actual density by 100
		unsigned int density;

		// Is this element flammable
		unsigned char isFlammable : 1;
		unsigned char flammability : 7;

		// Is this element fluid-like (Liquids and Gases for example)
		unsigned char isFluid : 1;

		// The placement method of the element
		unsigned char placement : 3; // 0-7 

		// Placement could be put in elementDescription because it isnt used that much 

		unsigned char isConductive : 1;
		// The unsigned char has 3 more bits left over

		//bool isParticle
	};

	// Structure of element chunks
	struct chunk
	{
		int currentMinX, currentMinY, currentMinZ;
		int currentMaxX, currentMaxY, currentMaxZ;

		int nextMinX, nextMinY, nextMinZ;
		int nextMaxX, nextMaxY, nextMaxZ;

		bool updateCurrentFrame = false;
		bool updateNextFrame = false;

		// Set the nextFrame bool to the currentFrame bool
		void FrameChange();

		// Set nextFrame bool to true
		void StepNextFrame()
		{
			updateNextFrame = true;
		}
	};

	// Structure of a particle
	struct particle
	{
		unsigned int elementID;
		unsigned int color;

		// 0 0000000000 00000
		// 0 - is for sign
		//   0000000000 - is for whole value
		//			    00000 - is for fractional value

		short posX, posY, posZ;
		short velX, velY, velZ;

		// TODO: See if this fixes the problem of paritcles warping in and out of existence
		// Represents the number of particles at the particle spot
		// unsigned int particlePop;
	};

	static const particle emptyParticle = {0};

	enum scene
	{
		splashScreen,
		mainMenu,
		gameLoop,
		gameExit
	};
}

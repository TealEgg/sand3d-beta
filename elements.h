#pragma once

namespace elements
{
	// Vector to map category enum to string
	std::vector<std::string> categoryList;

	// All element definitions will be stored here (use element index)
	std::vector<game::elementDefinition> definitionsList;
	game::elementDefinition* definitions;

	// Element names and descriptions will be stored here (use element index)
	std::vector<game::elementDescription> descriptionList;

	// Map that stores the elementID of each element
	std::map<std::string, unsigned int> identification;

	// NOTE: Don't move x, y, z variable into here and take out paramters of function, for lua modders they might need to do these calculations in the function
	struct
	{
		unsigned int localX = 0;
		unsigned int localY = 0;
		unsigned int localZ = 0;

		unsigned int chunkX = 0;
		unsigned int chunkY = 0;
		unsigned int chunkZ = 0;

		// unsigned int localIndex = 0;
		// unsigned int chunkIndex = 0;
		unsigned int index = 0;
	} 
	init, update;

	// TODO: init would not really make sense because if we call setElement inside the init functions of the elements, then everything will break
	
	#pragma region Element Utilities

	// TODO: Make stepchunk more efficient somehow (Note: It might not be possible)

	void StepChunkNew(unsigned int localX, unsigned int localY, unsigned int localZ, unsigned int chunkX, unsigned int chunkY, unsigned int chunkZ)
	{
		unsigned int chunkIndex = game::index3dC(chunkX, chunkY, chunkZ, game::chunkLength);

		game::chunkData[chunkIndex].updateNextFrame = true;

		// Since the update is starts implicitly from nextMin and explicitly from nextMax, 
		// the offset for nextMax (in this case the + 2) should technically be 1 larger than the offset for nextMin

		game::chunkData[chunkIndex].nextMinX = util::clamp(std::min((int)localX - 2, game::chunkData[chunkIndex].nextMinX), 0, game::chunkBounds);
		game::chunkData[chunkIndex].nextMinY = util::clamp(std::min((int)localY - 2, game::chunkData[chunkIndex].nextMinY), 0, game::chunkBounds);
		game::chunkData[chunkIndex].nextMinZ = util::clamp(std::min((int)localZ - 2, game::chunkData[chunkIndex].nextMinZ), 0, game::chunkBounds);
																					 
		game::chunkData[chunkIndex].nextMaxX = util::clamp(std::max((int)localX + 2, game::chunkData[chunkIndex].nextMaxX), 0, game::chunkBounds);
		game::chunkData[chunkIndex].nextMaxY = util::clamp(std::max((int)localY + 2, game::chunkData[chunkIndex].nextMaxY), 0, game::chunkBounds);
		game::chunkData[chunkIndex].nextMaxZ = util::clamp(std::max((int)localZ + 2, game::chunkData[chunkIndex].nextMaxZ), 0, game::chunkBounds);

		// return true;
	}

	void StepChunk2(unsigned int localX, unsigned int localY, unsigned int localZ, unsigned int chunkX, unsigned int chunkY, unsigned int chunkZ)
	{
		StepChunkNew(localX, localY, localZ, chunkX, chunkY, chunkZ);

		// TODO: Maybe put conditional check here to make sure only the correct ones go through

		bool stepX = false, stepY = false, stepZ = false;
		unsigned int newLocalX = 0, newLocalY = 0, newLocalZ = 0;
		unsigned int newChunkX = 0, newChunkY = 0, newChunkZ = 0;

		if (localX == 0 && chunkX != 0) 
		{ 
			stepX = true;
			newLocalX = game::chunkBounds - 1;
			newChunkX = chunkX - 1;
			StepChunkNew(newLocalX, localY, localZ, newChunkX, chunkY, chunkZ);
		}
		if (localY == 0 && chunkY != 0) 
		{ 
			stepY = true;
			newLocalY = game::chunkBounds - 1;
			newChunkY = chunkY - 1;
			StepChunkNew(localX, newLocalY, localZ, chunkX, newChunkY, chunkZ);
		}
		if (localZ == 0 && chunkZ != 0) 
		{ 
			stepZ = true;
			newLocalZ = game::chunkBounds - 1;
			newChunkZ = chunkZ - 1;
			StepChunkNew(localX, localY, newLocalZ, chunkX, chunkY, newChunkZ);
		}
		if (!stepX && localX == game::chunkBounds - 1 && chunkX != game::chunkLength - 1)
		{ 
			stepX = true;
			newLocalX = 0;
			newChunkX = chunkX + 1;
			StepChunkNew(newLocalX, localY, localZ, newChunkX, chunkY, chunkZ);
		}
		if (!stepY && localY == game::chunkBounds - 1 && chunkY != game::chunkLength - 1)
		{ 
			stepY = true;
			newLocalY = 0;
			newChunkY = chunkY + 1;
			StepChunkNew(localX, 0, localZ, chunkX, newChunkY, chunkZ);
		}
		if (!stepZ && localZ == game::chunkBounds - 1 && chunkZ != game::chunkLength - 1)
		{ 
			stepZ = true;
			newLocalZ = 0;
			newChunkZ = chunkZ + 1;
			StepChunkNew(localX, localY, newLocalZ, chunkX, chunkY, newChunkZ);
		}

		if (stepX && stepY)
		{
			StepChunkNew(newLocalX, newLocalY, localZ, newChunkX, newChunkY, chunkZ);
		}
		if (stepY && stepZ)
		{
			StepChunkNew(localX, newLocalY, newLocalZ, chunkX, newChunkY, newChunkZ);
		}
		if (stepX && stepZ)
		{
			StepChunkNew(newLocalX, localY, newLocalZ, newChunkX, chunkY, newChunkZ);
		}

		if (stepX && stepY && stepZ)
		{
			StepChunkNew(newLocalX, newLocalY, newLocalZ, newChunkX, newChunkY, newChunkZ);
		}
	}

	// This creates weird errors, look at it in the future
	void StepChunk2(unsigned int x, unsigned int y, unsigned int z)
	{
		unsigned int chunkX = x >> game::chunkExp;
		unsigned int chunkY = y >> game::chunkExp;
		unsigned int chunkZ = z >> game::chunkExp;

		unsigned int localX = x - (chunkX << game::chunkExp);
		unsigned int localY = y - (chunkY << game::chunkExp);
		unsigned int localZ = z - (chunkZ << game::chunkExp);

		StepChunk2(localX, localY, localZ, chunkX, chunkY, chunkZ);
	}

	// Check if a voxel is inside the game bounds
	inline bool VoxelIsInside(unsigned int x, unsigned int y, unsigned int z)
	{
		//return x < game::bounds&& y < game::bounds&& z < game::bounds&& x >= 0 && y >= 0 && z >= 0;

		// This should be faster. It works because we know that x, y, z will never be less than 0 because they are unsigned
		return x < game::bounds && y < game::bounds && z < game::bounds;
	}

	// Get element from array
	game::element getElement(unsigned int x, unsigned int y, unsigned int z) { return game::elementData[game::index3dA(x, y, z)]; }
	game::element getElement(unsigned int index) { return game::elementData[index]; }

	// Get element color from array
	unsigned int getElementColor(unsigned int x, unsigned int y, unsigned int z) { return game::voxData[game::index3dA(x, y, z)]; }
	unsigned int getElementColor(unsigned int index) { return game::voxData[index]; }

	// This is here because we need to do a performance check in setElement
	void emptyInit(unsigned int x, unsigned int y, unsigned int z);

	// Set element in array
	void setElement(unsigned int x, unsigned int y, unsigned int z, unsigned int elementID)
	{
		init.chunkX = x >> game::chunkExp; // Same as x / chunkBounds
		init.chunkY = y >> game::chunkExp;
		init.chunkZ = z >> game::chunkExp;

		init.localX = x - (init.chunkX << game::chunkExp); // Same as x * chunkBounds
		init.localY = y - (init.chunkY << game::chunkExp);
		init.localZ = z - (init.chunkZ << game::chunkExp);

		init.index = game::chunkVoxelNum * (init.chunkX + game::chunkLength * (init.chunkY + game::chunkLength * init.chunkZ)) + 
										  (init.localX + ((init.localY + (init.localZ << game::chunkExp)) << game::chunkExp));

		game::elementData[init.index] = game::emptyElement;
		game::elementData[init.index].elementID = elementID;

		game::voxData[init.index] = util::interpolate(elements::definitions[elementID].color1, elements::definitions[elementID].color2);

		if (elements::definitions[elementID].init != emptyInit)
		{
			elements::definitions[elementID].init(x, y, z);
		}

		StepChunk2(init.localX, init.localY, init.localZ, init.chunkX, init.chunkY, init.chunkZ);
	}

	// Fast set air (technically not only 10 ns faster (17 ns vs 27 ns)). It could be even faster if we take out stepChunk2 function
	void setElementAir(unsigned int x, unsigned int y, unsigned int z)
	{
		unsigned int chunkX = x >> game::chunkExp;
		unsigned int chunkY = y >> game::chunkExp;
		unsigned int chunkZ = z >> game::chunkExp;

		unsigned int localX = x - chunkX * game::chunkBounds;
		unsigned int localY = y - chunkY * game::chunkBounds;
		unsigned int localZ = z - chunkZ * game::chunkBounds;

		unsigned int index = game::chunkVoxelNum * game::index3dC(chunkX, chunkY, chunkZ, game::chunkLength) + game::index3dC(localX, localY, localZ, game::chunkBounds);

		game::elementData[index] = game::emptyElement;

		StepChunk2(localX, localY, localZ, chunkX, chunkY, chunkZ);
	}

	// Swap two elements
	void swapElement(unsigned int indexA, unsigned int indexB)
	{
		util::swap(game::elementData[indexA], game::elementData[indexB]);
		util::swap(game::voxData[indexA], game::voxData[indexB]);
	}

	// Copy element A to element B
	void copyElement(int indexA, int indexB)
	{
		game::elementData[indexB] = game::elementData[indexA];
		game::voxData[indexB] = game::voxData[indexA];
	}

	// Get Element Definitions
	game::elementDefinition getDefinition(unsigned int x, unsigned int y, unsigned int z)
	{
		return elements::definitions[game::elementData[game::index3dA(x, y, z)].elementID];
	}
	game::elementDefinition getDefinition(unsigned int index)
	{
		return elements::definitions[game::elementData[index].elementID];
	}

	// Create Particle at element location
	void setParticle(unsigned int x, unsigned int y, unsigned int z, float velX, float velY, float velZ)
	{
		unsigned int elementID = getElement(x, y, z).elementID;
		unsigned int color = game::voxData[game::currentIndex];

		// 36 is the elementID of particle element
		// The particle element should not be able to become a particle
		if (elementID == 36 || game::particleCount >= game::maxParticles) return;

		setElement(x, y, z, identification["particle"]);
		game::voxData[game::currentIndex] = color;

		game::particleData[game::particleCount].color = color;

		game::particleData[game::particleCount].elementID = elementID;

		game::particleData[game::particleCount].posX = x * 32 + 16; // Add a 0.5 offset to each component to center
		game::particleData[game::particleCount].posY = y * 32 + 16;	// Add a 0.5 offset to each component to center
		game::particleData[game::particleCount].posZ = z * 32 + 16;	// Add a 0.5 offset to each component to center

		game::particleData[game::particleCount].velX = velX * 32;
		game::particleData[game::particleCount].velY = velY * 32;
		game::particleData[game::particleCount].velZ = velZ * 32;

		game::particleCount++;
	}

	#pragma endregion

	// Fire and Electricity Rules
	/*
	Flammable elements will use registers G and I
	- G for storing amount of frames to burn for
	- I for storing if a flmaable object is currently on fire

	Electric elements will use register L
	- L = 0 means that the element can be charged
	- L = 2 means that it is charged
	- L = 3 means that the charge is on cooldown
	- L = 4 means that the charge cooldown is off
	*/

	#pragma region Element Initialization

	void emptyInit(unsigned int x, unsigned int y, unsigned int z) { }

	void smokeInit(unsigned int x, unsigned int y, unsigned int z) 
	{
		game::elementData[init.index].registerA = util::randXOR() % 32;
	}

	void fireInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerA = util::randXOR() % 32;
	}

	void woodInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerG = 128 + util::randXOR() % 32;
	}

	void gunpowderInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerG = 16 + util::randXOR() % 8;
	}

	void laserInit(unsigned int x, unsigned int y, unsigned int z)
	{
		glm::ivec3 dir = (glm::ivec3)(game::front * 15.0f);

		game::elementData[init.index].registerA = dir.x + 15;
		game::elementData[init.index].registerB = dir.y + 15;
		game::elementData[init.index].registerC = dir.z + 15;

		int dirMax = std::max(std::abs(dir.x), std::max(std::abs(dir.y), std::abs(dir.z)));

		game::elementData[init.index].registerD = dirMax / 2 + 15;
		game::elementData[init.index].registerE = dirMax / 2 + 15;
		game::elementData[init.index].registerF = dirMax / 2 + 15;
	}

	void randomInit(unsigned int x, unsigned int y, unsigned int z)
	{
		int randomElementID;

		do { randomElementID = util::randXOR() % definitionsList.size(); } 
		while (descriptionList[randomElementID].category == game::categories::misc);

		setElement(x, y, z, randomElementID);
	}

	void platformInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerH = 1;
	}

	void chunkDeleterInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerH = 32;
	}

	void currentInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerA = 10;
	}

	void seedInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerG = 32 + util::randXOR() % 32;
	}

	void sproutInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerH = 8 + util::randXOR() % 8; // For growth
	}

	void stemInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerG = 64 + util::randXOR() % 32;
	}

	void flowerSeedInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerG = 32 + util::randXOR() % 32;
	}

	void flowerSproutInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerH = (unsigned int)8 + util::randXOR() % 32; // For growth
	}

	void petalInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerG = 32 + util::randXOR() % 8;
	}

	void explosionFlashInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerG = 1;
	}

	void wetConcreteInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerG = util::randXOR() & 255;
	}

	void upwardWallInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerH = 1;
	}

	/*void worldDeleterInit(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[init.index].registerH = 4;
	}*/

	#pragma endregion

	#pragma region Element Reaction

	// noReaction and emptyReaction must have different returns or else the compiler will make then point to the same function pointer which creates errors

	// noReaction is for elements with no reactive properties
	bool noReaction(unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2)
	{
		return true;
	}

	// emptyReaction is for elements that have other elements that react to it.
	bool emptyReaction(unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2)
	{
		return false;
	}

	bool waterReaction(unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2)
	{
		bool react = false;

		if (getElement(x2, y2, z2).elementID == 4) // fire
		{
			setElement(x1, y1, z1, identification["steam"]);

			react = true;
		}
		else if (getDefinition(x2, y2, z2).isFlammable && getElement(x2, y2, z2).registerI) // element on fire
		{
			setElement(x1, y1, z1, identification["steam"]);

			if (getElement(x2, y2, z2).elementID == identification["lava"])
			{
				setElement(x2, y2, z2, identification["basalt"]);
			}

			react = true;
		}

		return react;
	}

	bool lavaReaction(unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2)
	{
		bool react = false;

		if (getElement(x2, y2, z2).elementID == 2) // water
		{
			setElement(x1, y1, z1, identification["basalt"]);
			setElement(x2, y2, z2, identification["steam"]);
			react = true;
		}

		return react;
	}

	bool stemReaction(unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2)
	{
		bool react = false;

		if (getElement(x2, y2, z2).elementID == 2) // water
		{
			setElement(x2, y2, z2, identification["sprout"]);
			react = true;
		}

		return react;
	}

	bool Reaction(unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2)
	{
		unsigned int index1 = game::index3dA(x1, y1, z1);
		unsigned int index2 = game::index3dA(x2, y2, z2);

		bool react = getDefinition(index2).reaction != noReaction &&
			getDefinition(index1).reaction != getDefinition(index2).reaction;

		return react && (getDefinition(x1, y1, z1).reaction(x1, y1, z1, x2, y2, z2) || getDefinition(x2, y2, z2).reaction(x2, y2, z2, x1, y1, z1));
	}

	#pragma endregion

	#pragma region Element Update

	void emptyUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		// Note: we might not even need to do this because nothing is happening
		game::elementData[update.index].clock = 1;
	}

	/*
	// This was the original sand
	void sandUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		unsigned int chunkX = x >> game::chunkExp;
		unsigned int chunkY = y >> game::chunkExp;
		unsigned int chunkZ = z >> game::chunkExp;
	
		unsigned int localX = x - chunkX * game::chunkBounds;
		unsigned int localY = y - chunkY * game::chunkBounds;
		unsigned int localZ = z - chunkZ * game::chunkBounds;
	
		unsigned int index = game::chunkVoxelNum * game::index3dC(chunkX, chunkY, chunkZ, game::chunkLength) + game::index3dC(localX, localY, localZ, game::chunkBounds);

		game::elementData[index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(index).density)
		{
			swapElement(index, game::currentIndex);
			StepChunk2(localX, localY, localZ, chunkX, chunkY, chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(index).density)
		{
			swapElement(index, game::currentIndex);
			StepChunk2(localX, localY, localZ, chunkX, chunkY, chunkZ);
		}
	}
	*/

	void sandUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void waterUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && !Reaction(x, y, z, x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && !Reaction(x, y, z, x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y, z + dir2) && !Reaction(x, y, z, x + dir1, y, z + dir2) && getDefinition(x + dir1, y, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void smokeUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		// We put this at the top because the element moves, which might change the index
		// If it moves, then it will use game::currentIndex, and if not then it will use cur.index
		// To not deal with this, we can just put this on the top
		if (game::elementData[update.index].registerA == 0) { setElement(x, y, z, 0); return; }
		else game::elementData[update.index].registerA--;

		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y + 1, z) && getDefinition(x, y + 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y + 1, z + dir2) && getDefinition(x + dir1, y + 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y, z + dir2) && getDefinition(x + dir1, y, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void fireUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		// Changing color
		// game::game::voxData[index] = util::interpolate(elements::definitions[identification["fire"]].color1, elements::definitions[identification["fire"]].color2, util::randXOR() / (float)UINT_MAX);

		if (game::elementData[update.index].registerA < 1)
		{
			setElement(x, y, z, 0);
			return;
		}
		else game::elementData[update.index].registerA--;

		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x + dir1, y + 1, z + dir2) && getDefinition(x + dir1, y + 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
		}
		else if (VoxelIsInside(x + dir1, y, z + dir2) && getDefinition(x + dir1, y, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
		}

		dir1 = util::direction();
		dir2 = util::direction();

		if ((util::randXOR() % 100) < 10) // 10 percent chance of smoke
		{
			if (VoxelIsInside(x + dir1, y + 1, z + dir2) && getElement(x + dir1, y + 1, z + dir2).elementID == 0)
			{
				setElement(x + dir1, y + 1, z + dir2, identification["smoke"]);
			}
		}

		// Set an element on fire if flammable
		dir1 = util::direction();
		dir2 = util::direction();
		int dir3 = util::direction();
		if (VoxelIsInside(x + dir1, y + dir2, z + dir3) && !Reaction(x, y, z, x + dir1, y + dir2, z + dir3) && getDefinition(x + dir1, y + dir2, z + dir3).isFlammable)
		{
			game::elementData[game::currentIndex].registerI = true;

			// StepChunk2(localX, localY, localZ, chunkX, chunkY, chunkZ);
		}

		// This is outside because fire will eventually end
		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
	}

	void woodUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerI == true) // On fire
		{
			game::voxData[update.index] = util::interpolate(elements::definitions[identification["fire"]].color1, elements::definitions[identification["fire"]].color2);

			int dir1 = util::direction();
			int dir2 = util::direction();
			int dir3 = util::direction();

			if (VoxelIsInside(x + dir1, y + dir2, z + dir3))
			{
				if (getDefinition(x + dir1, y + dir2, z + dir3).isFlammable)
				{
					game::elementData[game::currentIndex].registerI = true;
				}
				else if (game::elementData[game::currentIndex].elementID == 0 && (util::randXOR() % 512 < getDefinition(update.index).flammability))
				{
					setElement(x + dir1, y + dir2, z + dir3, identification["fire"]);
				}
			}

			// If finished burning then replace with air
			if (game::elementData[update.index].registerG == 0)
			{
				setElement(x, y, z, 0);
			}
			else { game::elementData[update.index].registerG--; }

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void gunpowderUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}

		if (game::elementData[update.index].registerI == true) // On fire
		{
			game::voxData[update.index] = util::interpolate(elements::definitions[identification["fire"]].color1, elements::definitions[identification["fire"]].color2);

			dir1 = util::direction();
			dir2 = util::direction();
			int dir3 = util::direction();

			if (VoxelIsInside(x + dir1, y + dir2, z + dir3))
			{
				if (getDefinition(x + dir1, y + dir2, z + dir3).isFlammable)
				{
					game::elementData[game::currentIndex].registerI = true;
				}
				else if (game::elementData[game::currentIndex].elementID == 0 && (util::randXOR() % 512 < getDefinition(update.index).flammability))
				{
					setElement(x + dir1, y + dir2, z + dir3, identification["fire"]);
				}
			}

			// If finished burning then replace with air
			if (game::elementData[update.index].registerG == 0)
			{
				setElement(x, y, z, 0);
			}
			else { game::elementData[update.index].registerG--; }

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void laserUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		game::element before = game::elementData[update.index];

		int newX = x, newY = y, newZ = z;

		int dirX = (int)game::elementData[update.index].registerA - 15;
		int dirY = (int)game::elementData[update.index].registerB - 15;
		int dirZ = (int)game::elementData[update.index].registerC - 15;

		int signX = dirX > 0 ? 1 : -1;
		int signY = dirY > 0 ? 1 : -1;
		int signZ = dirZ > 0 ? 1 : -1;

		dirX = std::abs(dirX);
		dirY = std::abs(dirY);
		dirZ = std::abs(dirZ);

		int dirMax = std::max(dirX, std::max(dirY, dirZ));

		int offsetX = (int)game::elementData[update.index].registerD - 15;
		int offsetY = (int)game::elementData[update.index].registerE - 15;
		int offsetZ = (int)game::elementData[update.index].registerF - 15;

		offsetX -= dirX; if (offsetX < 0) { offsetX += dirMax; newX += signX; }
		offsetY -= dirY; if (offsetY < 0) { offsetY += dirMax; newY += signY; }
		offsetZ -= dirZ; if (offsetZ < 0) { offsetZ += dirMax; newZ += signZ; }

		bool laserBefore = game::elementData[update.index].registerK;
		game::elementData[update.index].registerK = false;

		if (VoxelIsInside(newX, newY, newZ))
		{
			if (getElement(newX, newY, newZ).elementID == 7 && laserBefore)
			{
				// Stay in place
				game::elementData[game::currentIndex].registerK = true;
			}
			else if (getElement(newX, newY, newZ).elementID == 7 && !laserBefore)
			{
				// delete self
				setElement(x, y, z, 0);

				game::elementData[game::currentIndex].registerK = true;
			}
			else if (getElement(newX, newY, newZ).elementID != 7 && laserBefore)
			{
				// replace front
				// keep back

				game::elementData[update.index].registerD = offsetX + 15;
				game::elementData[update.index].registerE = offsetY + 15;
				game::elementData[update.index].registerF = offsetZ + 15;

				setElement(newX, newY, newZ, 0);

				swapElement(update.index, game::currentIndex);

				game::elementData[update.index] = before;
				game::elementData[update.index].registerK = false;
				game::voxData[update.index] = game::voxData[game::currentIndex];
			}
			else if (getElement(newX, newY, newZ).elementID != 7 && !laserBefore)
			{
				// only replace front

				game::elementData[update.index].registerD = offsetX + 15;
				game::elementData[update.index].registerE = offsetY + 15;
				game::elementData[update.index].registerF = offsetZ + 15;

				setElement(newX, newY, newZ, 0);
				swapElement(update.index, game::currentIndex);
			}

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else { setElement(x, y, z, 0); }
	}

	void chargeUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (VoxelIsInside(x + 1, y, z) &&
			getDefinition(x + 1, y, z).isConductive &&
			(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
		{
			game::elementData[game::currentIndex].registerL = 1;
		}
		if (VoxelIsInside(x - 1, y, z) &&
			getDefinition(x - 1, y, z).isConductive &&
			game::elementData[game::currentIndex].registerL == 0)
		{
			game::elementData[game::currentIndex].registerL = 2;
		}
		if (VoxelIsInside(x, y + 1, z) &&
			getDefinition(x, y + 1, z).isConductive &&
			(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
		{
			game::elementData[game::currentIndex].registerL = 1;
		}
		if (VoxelIsInside(x, y - 1, z) &&
			getDefinition(x, y - 1, z).isConductive &&
			game::elementData[game::currentIndex].registerL == 0)
		{
			game::elementData[game::currentIndex].registerL = 2;
		}
		if (VoxelIsInside(x, y, z + 1) &&
			getDefinition(x, y, z + 1).isConductive &&
			(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
		{
			game::elementData[game::currentIndex].registerL = 1;
		}
		if (VoxelIsInside(x, y, z - 1) &&
			getDefinition(x, y, z - 1).isConductive &&
			game::elementData[game::currentIndex].registerL == 0)
		{
			game::elementData[game::currentIndex].registerL = 2;
		}

		// This is outside so that they dont get stuck in place
		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);

		setElement(x, y, z, 0);
	}

	void wireUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		game::element& current = game::elementData[update.index];

		if (current.registerL == 2)
		{
			game::voxData[update.index] = 0xFF33FFFF;

			if (VoxelIsInside(x + 1, y, z) &&
				getDefinition(x + 1, y, z).isConductive &&
				(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
			{
				game::elementData[game::currentIndex].registerL = 1;
			}
			if (VoxelIsInside(x - 1, y, z) &&
				getDefinition(x - 1, y, z).isConductive &&
				game::elementData[game::currentIndex].registerL == 0)
			{
				game::elementData[game::currentIndex].registerL = 2;
			}
			if (VoxelIsInside(x, y + 1, z) &&
				getDefinition(x, y + 1, z).isConductive &&
				(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
			{
				game::elementData[game::currentIndex].registerL = 1;
			}
			if (VoxelIsInside(x, y - 1, z) &&
				getDefinition(x, y - 1, z).isConductive &&
				game::elementData[game::currentIndex].registerL == 0)
			{
				game::elementData[game::currentIndex].registerL = 2;
			}
			if (VoxelIsInside(x, y, z + 1) &&
				getDefinition(x, y, z + 1).isConductive &&
				(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
			{
				game::elementData[game::currentIndex].registerL = 1;
			}
			if (VoxelIsInside(x, y, z - 1) &&
				getDefinition(x, y, z - 1).isConductive &&
				game::elementData[game::currentIndex].registerL == 0)
			{
				game::elementData[game::currentIndex].registerL = 2;
			}
		}

		if (current.registerL == 3)
		{
			game::voxData[update.index] = util::interpolate(0xFF2A3157, 0xFF2E3453);
		}

		if (current.registerL == 4)
		{
			current.registerL = 0;
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}

		if (current.registerL > 0)
		{
			current.registerL++;

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void waterspoutUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		game::element& current = game::elementData[update.index];

		if (current.registerL == 2)
		{
			if (VoxelIsInside(x, y - 1, z) && getElement(x, y - 1, z).elementID == 0)
			{
				setElement(x, y - 1, z, identification["water"]);
			}

			if (VoxelIsInside(x + 1, y, z) &&
				getDefinition(x + 1, y, z).isConductive &&
				(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
			{
				game::elementData[game::currentIndex].registerL = 1;
			}
			if (VoxelIsInside(x - 1, y, z) &&
				getDefinition(x - 1, y, z).isConductive &&
				game::elementData[game::currentIndex].registerL == 0)
			{
				game::elementData[game::currentIndex].registerL = 2;
			}
			if (VoxelIsInside(x, y + 1, z) &&
				getDefinition(x, y + 1, z).isConductive &&
				(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
			{
				game::elementData[game::currentIndex].registerL = 1;
			}
			if (VoxelIsInside(x, y - 1, z) &&
				getDefinition(x, y - 1, z).isConductive &&
				game::elementData[game::currentIndex].registerL == 0)
			{
				game::elementData[game::currentIndex].registerL = 2;
			}
			if (VoxelIsInside(x, y, z + 1) &&
				getDefinition(x, y, z + 1).isConductive &&
				(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
			{
				game::elementData[game::currentIndex].registerL = 1;
			}
			if (VoxelIsInside(x, y, z - 1) &&
				getDefinition(x, y, z - 1).isConductive &&
				game::elementData[game::currentIndex].registerL == 0)
			{
				game::elementData[game::currentIndex].registerL = 2;
			}
		}

		if (current.registerL == 4)
		{
			current.registerL = 0;
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}

		if (current.registerL > 0)
		{
			current.registerL++;

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void basaltUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void steamUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if ((util::randXOR() % 1023) < 1) // 0.1 % chance of turning back into water
		{
			setElement(x, y, z, identification["water"]);
			return;
		}

		/*if (VoxelIsInside(x, y + 1, z) && getDefinition(x, y + 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(index).density)
		{
			swapElement(index, game::currentIndex);

			StepChunk2(localX, localY, localZ, chunkX, chunkY, chunkZ);
		}
		else*/ if (VoxelIsInside(x + dir1, y + 1, z + dir2) && getDefinition(x + dir1, y + 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
		}
		else if (VoxelIsInside(x + dir1, y, z + dir2) && getDefinition(x + dir1, y, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
		}

		// Because this element will be turned into water eventually, we can always stepChunk
		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
	}

	void platformUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerH > 0)
		{
			game::elementData[update.index].registerH--;

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
			return;
		}

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (i == 0 && j == 0)
				{
					setElement(x, y, z, 14);
				}
				else if (VoxelIsInside(x + j, y, z + i) && getElement(x + j, y, z + i).elementID == 0)
				{
					setElement(x + j, y, z + i, 15);
				}
			}
		}
	}

	void acidUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		// Create a system where if corode is true, then delete the elements. This will make things slower as easier to see. Also acid shouldn't be destoroyed instantly
		// bool corode = (util::randXOR() % 15) == 1;

		if (VoxelIsInside(x, y - 1, z) && getElement(x, y - 1, z).elementID == 0)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x, y - 1, z) && game::elementData[game::currentIndex].elementID != identification["acid"])
		{
			setElement(x, y - 1, z, 0);
			setElement(x, y, z, 0);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getElement(x + dir1, y - 1, z + dir2).elementID == 0)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && game::elementData[game::currentIndex].elementID != identification["acid"])
		{
			setElement(x + dir1, y - 1, z + dir2, 0);
			setElement(x, y, z, 0);
		}
		else if (VoxelIsInside(x + dir1, y, z + dir2) && getElement(x + dir1, y, z + dir2).elementID == 0)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y, z + dir2) && game::elementData[game::currentIndex].elementID != identification["acid"])
		{
			setElement(x + dir1, y, z + dir2, 0);
			setElement(x, y, z, 0);
		}
	}

	void lavaUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		// Always "on fire"
		game::elementData[update.index].registerI = true;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && !Reaction(x, y, z, x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && !Reaction(x, y, z, x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y, z + dir2) && !Reaction(x, y, z, x + dir1, y, z + dir2) && getDefinition(x + dir1, y, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}

		if (game::elementData[update.index].registerI == true) // On fire
		{
			dir1 = util::direction();
			dir2 = util::direction();
			int dir3 = util::direction();

			if (VoxelIsInside(x + dir1, y + dir2, z + dir3))
			{
				if (getDefinition(x + dir1, y + dir2, z + dir3).isFlammable)
				{
					game::elementData[game::currentIndex].registerI = true;
				}
				else if (game::elementData[game::currentIndex].elementID == 0 && (util::randXOR() % 512 < 1))
				{
					setElement(x + dir1, y + dir2, z + dir3, identification["fire"]);
				}
			}

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void chunkDeleterUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerH == 32)
		{
			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					for (int k = -1; k < 2; k++)
					{
						if ((update.localX + k) < game::chunkBounds && (update.localY + j) < game::chunkBounds && (update.localZ + i) < game::chunkBounds)
						{
							if (getElement(x + k, y + j, z + i).elementID != 18)
							{
								setElement(x + k, y + j, z + i, 18);
								game::elementData[game::currentIndex].clock = 1;
							}
						}
					}
				}
			}
		}
		else if (game::elementData[update.index].registerH == 0)
		{
			setElement(x, y, z, 0);
			return;
		}

		game::elementData[update.index].registerH--;

		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
	}

	void currentUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerA == 0)
		{
			setElement(x, y, z, 0);
			return;
		}
		game::elementData[update.index].registerA--;

		if (VoxelIsInside(x + 1, y, z) &&
			getDefinition(x + 1, y, z).isConductive &&
			(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
		{
			game::elementData[game::currentIndex].registerL = 1;
		}
		if (VoxelIsInside(x - 1, y, z) &&
			getDefinition(x - 1, y, z).isConductive &&
			game::elementData[game::currentIndex].registerL == 0)
		{
			game::elementData[game::currentIndex].registerL = 2;
		}
		if (VoxelIsInside(x, y + 1, z) &&
			getDefinition(x, y + 1, z).isConductive &&
			(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
		{
			game::elementData[game::currentIndex].registerL = 1;
		}
		if (VoxelIsInside(x, y - 1, z) &&
			getDefinition(x, y - 1, z).isConductive &&
			game::elementData[game::currentIndex].registerL == 0)
		{
			game::elementData[game::currentIndex].registerL = 2;
		}
		if (VoxelIsInside(x, y, z + 1) &&
			getDefinition(x, y, z + 1).isConductive &&
			(game::elementData[game::currentIndex].registerL == 0 || game::elementData[game::currentIndex].registerL == 4))
		{
			game::elementData[game::currentIndex].registerL = 1;
		}
		if (VoxelIsInside(x, y, z - 1) &&
			getDefinition(x, y, z - 1).isConductive &&
			game::elementData[game::currentIndex].registerL == 0)
		{
			game::elementData[game::currentIndex].registerL = 2;
		}

		// Current stops moving randomly, somehow due to the movement down here

		int ranDir1 = util::direction();
		int ranDir2 = util::direction();
		int ranDir3 = util::direction();

		if (VoxelIsInside(x + ranDir1, y + ranDir2, z + ranDir3) && getElement(x + ranDir1, y + ranDir2, z + ranDir3).elementID == 0)
		{
			swapElement(update.index, game::currentIndex);
		}

		// This is outside so that they dont get stuck in place
		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
	}

	void dirtUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void seedUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x + dir1, y - 1, z + dir2) && (getElement(x + dir1, y - 1, z + dir2).elementID == 20 || game::elementData[game::currentIndex].elementID == 14))
		{
			setElement(x, y, z, identification["sprout"]);
			return;
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}

		if (game::elementData[update.index].registerI == true) // On fire
		{
			game::voxData[update.index] = util::interpolate(elements::definitions[identification["fire"]].color1, elements::definitions[identification["fire"]].color2);

			dir1 = util::direction();
			dir2 = util::direction();
			int dir3 = util::direction();

			if (VoxelIsInside(x + dir1, y + dir2, z + dir3))
			{
				if (getDefinition(x + dir1, y + dir2, z + dir3).isFlammable)
				{
					game::elementData[game::currentIndex].registerI = true;
				}
				else if (game::elementData[game::currentIndex].elementID == 0 && (util::randXOR() % 512 < getDefinition(update.index).flammability))
				{
					setElement(x + dir1, y + dir2, z + dir3, identification["fire"]);
				}
			}

			// If finished burning then replace with air
			if (game::elementData[update.index].registerG == 0)
			{
				setElement(x, y, z, 0);
			}
			else { game::elementData[update.index].registerG--; }

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void perlinWormUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		float size = 10.0f;

		game::noise.SetSeed(1);
		int dir1 = (int)round(game::noise.GetNoise((float)x * size, (float)y * size, (float)z * size));// util::direction();
		game::noise.SetSeed(2);
		int dir2 = (int)round(game::noise.GetNoise((float)x * size, (float)y * size, (float)z * size));// util::direction();
		game::noise.SetSeed(3);
		int dir3 = (int)round(game::noise.GetNoise((float)x * size, (float)y * size, (float)z * size)); // util::direction();

		if (VoxelIsInside(x + dir1, y + 1, z + dir3)&& getElement(x + dir1, y + 1, z + dir3).elementID == 0)
		{
			swapElement(update.index, game::currentIndex);
			setElement(x, y, z, identification["wood"]);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else { setElement(x, y, z, 0); }
	}

	void sproutUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerH == 0)
		{
			setElement(x, y, z, identification["stem"]);
			return;
		}

		game::elementData[update.index].registerH--;


		int dir1 = util::direction();
		int dir2 = util::direction();
		int dir3 = util::direction();

		if (VoxelIsInside(x + dir1, y + 1, z + dir3) && getElement(x + dir1, y + 1, z + dir3).elementID == 0)
		{
			swapElement(update.index, game::currentIndex);
			setElement(x, y, z, identification["stem"]);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void stemUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerI == true) // On fire
		{
			game::voxData[update.index] = util::interpolate(elements::definitions[identification["fire"]].color1, elements::definitions[identification["fire"]].color2);

			int dir1 = util::direction();
			int dir2 = util::direction();
			int dir3 = util::direction();

			if (VoxelIsInside(x + dir1, y + dir2, z + dir3))
			{
				if (getDefinition(x + dir1, y + dir2, z + dir3).isFlammable)
				{
					game::elementData[game::currentIndex].registerI = true;
				}
				else if (game::elementData[game::currentIndex].elementID == 0 && (util::randXOR() % 512 < getDefinition(update.index).flammability))
				{
					setElement(x + dir1, y + dir2, z + dir3, identification["fire"]);
				}
			}

			// If finished burning then replace with air
			if (game::elementData[update.index].registerG == 0)
			{
				setElement(x, y, z, 0);
			}
			else { game::elementData[update.index].registerG--; }

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void flowerSeedUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x + dir1, y - 1, z + dir2) && (getElement(x + dir1, y - 1, z + dir2).elementID == 20 || game::elementData[game::currentIndex].elementID == 14))
		{
			setElement(x, y, z, identification["flower sprout"]);
			return;
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}

		if (game::elementData[update.index].registerI == true) // On fire
		{
			game::voxData[update.index] = util::interpolate(elements::definitions[identification["fire"]].color1, elements::definitions[identification["fire"]].color2);

			dir1 = util::direction();
			dir2 = util::direction();
			int dir3 = util::direction();

			if (VoxelIsInside(x + dir1, y + dir2, z + dir3))
			{
				if (getDefinition(x + dir1, y + dir2, z + dir3).isFlammable)
				{
					game::elementData[game::currentIndex].registerI = true;
				}
				else if (game::elementData[game::currentIndex].elementID == 0 && (util::randXOR() % 512 < getDefinition(update.index).flammability))
				{
					setElement(x + dir1, y + dir2, z + dir3, identification["fire"]);
				}
			}

			// If finished burning then replace with air
			if (game::elementData[update.index].registerG == 0)
			{
				setElement(x, y, z, 0);
			}
			else { game::elementData[update.index].registerG--; }

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void flowerSproutUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerH == 0)
		{
			setElement(x, y, z, identification["petal"]);
			game::elementData[update.index].registerH = util::randXOR() % 4;
			game::voxData[update.index] = util::randXOR();
			return;
		}

		game::elementData[update.index].registerH--;


		int dir1 = util::direction();
		int dir2 = util::direction();
		int dir3 = util::direction();

		if (VoxelIsInside(x + dir1, y + 1, z + dir3) && getElement(x + dir1, y + 1, z + dir3).elementID == 0)
		{
			swapElement(update.index, game::currentIndex);
			setElement(x, y, z, identification["stem"]);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void petalUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerI == true) // On fire
		{
			game::voxData[update.index] = util::interpolate(elements::definitions[identification["fire"]].color1, elements::definitions[identification["fire"]].color2);

			int dir1 = util::direction();
			int dir2 = util::direction();
			int dir3 = util::direction();

			if (VoxelIsInside(x + dir1, y + dir2, z + dir3))
			{
				if (getDefinition(x + dir1, y + dir2, z + dir3).isFlammable)
				{
					game::elementData[game::currentIndex].registerI = true;
				}
				else if (game::elementData[game::currentIndex].elementID == 0 && (util::randXOR() % 512 < getDefinition(update.index).flammability))
				{
					setElement(x + dir1, y + dir2, z + dir3, identification["fire"]);
				}
			}

			// If finished burning then replace with air
			if (game::elementData[update.index].registerG == 0)
			{
				setElement(x, y, z, 0);
			}
			else { game::elementData[update.index].registerG--; }

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}

		if (game::elementData[update.index].registerH == 0) return;

		int dir1 = util::direction();
		int dir2 = util::direction();
		int dir3 = util::direction();

		if (VoxelIsInside(x + dir1, y + dir2, z + dir3) && getElement(x + dir1, y + dir2, z + dir3).elementID == 0)
		{
			setElement(x + dir1, y + dir2, z + dir3, identification["petal"]);
			game::elementData[update.index].registerH--;
			game::elementData[game::currentIndex].registerH = game::elementData[update.index].registerH;
			game::voxData[game::currentIndex] = game::voxData[update.index];

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	int elementProcessingCount = 0;

	#pragma region Explosion Methods

	// ExplosionLine and explode2 are the main explosion functions that you should be using

	void explosionLine(int x0, int y0, int z0, int x1, int y1, int z1)
	{
		// x0,y0,z0 is starting point

		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int dz = abs(z1 - z0), sz = z0 < z1 ? 1 : -1;
		int dm = dx > dy && dx > dz ? dx : dy > dz ? dy : dz, i = dm;
		int x2 = x1, y2 = y1, z2 = z1;
		x1 = y1 = z1 = dm / 2;

		int rand = util::randXOR() % 255;

		while (true)
		{
			// SetElement used to be here

			//if (i-- == 0) break;
			if (x0 == x2 && y0 == y2 && z0 == z2) break;

			// x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; }
			// y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; }
			// z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; }

			x1 -= dx; y1 -= dy; z1 -= dz;
			if (x1 < 0)
			{
				x1 += dm; x0 += sx;

				if (VoxelIsInside(x0, y0, z0) && getElement(x0, y0, z0).elementID != 14) // wall
				{
					if (rand == 0)
					{
						setElement(x0, y0, z0, identification["fire"]);
					}
					else if (rand == 1)
					{
						setElement(x0, y0, z0, identification["smoke"]);
					}
					else if (rand == 2 || rand == 3)
					{
						setElement(x0, y0, z0, identification["explosion flash"]);
						game::elementData[game::index3dA(x0, y0, z0)].clock = 1;
					}
					else if (getElement(x0, y0, z0).elementID != 0) // An else should be included which calls stepChunk
					{
						setElement(x0, y0, z0, 0);
					}

					elementProcessingCount++;
				}
				else break;
			}
			if (y1 < 0)
			{
				y1 += dm; y0 += sy;

				if (VoxelIsInside(x0, y0, z0) && getElement(x0, y0, z0).elementID != 14)
				{
					if (rand == 0)
					{
						setElement(x0, y0, z0, identification["fire"]);
					}
					else if (rand == 1)
					{
						setElement(x0, y0, z0, identification["smoke"]);
					}
					else if (rand == 2 || rand == 3)
					{
						setElement(x0, y0, z0, identification["explosion flash"]);
						game::elementData[game::index3dA(x0, y0, z0)].clock = 1;
					}
					else if (getElement(x0, y0, z0).elementID != 0)
					{
						setElement(x0, y0, z0, 0);
					}

					elementProcessingCount++;
				}
				else break;
			}
			if (z1 < 0)
			{
				z1 += dm; z0 += sz;

				if (VoxelIsInside(x0, y0, z0) && getElement(x0, y0, z0).elementID != 14)
				{
					if (rand == 0)
					{
						setElement(x0, y0, z0, identification["fire"]);
					}
					else if (rand == 1)
					{
						setElement(x0, y0, z0, identification["smoke"]);
					}
					else if (rand == 2 || rand == 3)
					{
						setElement(x0, y0, z0, identification["explosion flash"]);
						game::elementData[game::index3dA(x0, y0, z0)].clock = 1;
					}
					else if (getElement(x0, y0, z0).elementID != 0)
					{
						setElement(x0, y0, z0, 0);
					}

					elementProcessingCount++;
				}
				else break;
			}
		}
	}

	void explode2(unsigned int x0, unsigned int y0, unsigned int z0, unsigned int r)
	{
		glm::vec3 dist = glm::vec3(x0 + 0.5f, y0 + 0.5f, z0 + 0.5f) - game::positionVec;
		float length = glm::length(dist);
		game::addTrauma((float)r / length);

		int i = 0, j = 0;
		int k = r, k0 = r;
		int s = 0, s0 = 0;
		int v = r - 1, v0 = r - 1;
		int l = 2 * v0, l0 = 2 * v0;

		elementProcessingCount = 0;

		while (i <= k)
		{
			while (j <= k)
			{
				if (s > v)
				{
					k--;
					v += l;
					l -= 2;
				}

				if (j <= k && (s != v || j != k))
				{
					explosionLine(x0, y0, z0, x0 + i, y0 + j, z0 + k);
					explosionLine(x0, y0, z0, x0 + i, y0 + k, z0 + j);
					explosionLine(x0, y0, z0, x0 + j, y0 + k, z0 + i);
					explosionLine(x0, y0, z0, x0 + j, y0 + i, z0 + k);
					explosionLine(x0, y0, z0, x0 + k, y0 + j, z0 + i);
					explosionLine(x0, y0, z0, x0 + k, y0 + i, z0 + j);

					explosionLine(x0, y0, z0, x0 - i, y0 + j, z0 + k);
					explosionLine(x0, y0, z0, x0 - i, y0 + k, z0 + j);
					explosionLine(x0, y0, z0, x0 - j, y0 + k, z0 + i);
					explosionLine(x0, y0, z0, x0 - j, y0 + i, z0 + k);
					explosionLine(x0, y0, z0, x0 - k, y0 + j, z0 + i);
					explosionLine(x0, y0, z0, x0 - k, y0 + i, z0 + j);

					explosionLine(x0, y0, z0, x0 + i, y0 - j, z0 + k);
					explosionLine(x0, y0, z0, x0 + i, y0 - k, z0 + j);
					explosionLine(x0, y0, z0, x0 + j, y0 - k, z0 + i);
					explosionLine(x0, y0, z0, x0 + j, y0 - i, z0 + k);
					explosionLine(x0, y0, z0, x0 + k, y0 - j, z0 + i);
					explosionLine(x0, y0, z0, x0 + k, y0 - i, z0 + j);

					explosionLine(x0, y0, z0, x0 - i, y0 - j, z0 + k);
					explosionLine(x0, y0, z0, x0 - i, y0 - k, z0 + j);
					explosionLine(x0, y0, z0, x0 - j, y0 - k, z0 + i);
					explosionLine(x0, y0, z0, x0 - j, y0 - i, z0 + k);
					explosionLine(x0, y0, z0, x0 - k, y0 - j, z0 + i);
					explosionLine(x0, y0, z0, x0 - k, y0 - i, z0 + j);

					explosionLine(x0, y0, z0, x0 + i, y0 + j, z0 - k);
					explosionLine(x0, y0, z0, x0 + i, y0 + k, z0 - j);
					explosionLine(x0, y0, z0, x0 + j, y0 + k, z0 - i);
					explosionLine(x0, y0, z0, x0 + j, y0 + i, z0 - k);
					explosionLine(x0, y0, z0, x0 + k, y0 + j, z0 - i);
					explosionLine(x0, y0, z0, x0 + k, y0 + i, z0 - j);

					explosionLine(x0, y0, z0, x0 - i, y0 + j, z0 - k);
					explosionLine(x0, y0, z0, x0 - i, y0 + k, z0 - j);
					explosionLine(x0, y0, z0, x0 - j, y0 + k, z0 - i);
					explosionLine(x0, y0, z0, x0 - j, y0 + i, z0 - k);
					explosionLine(x0, y0, z0, x0 - k, y0 + j, z0 - i);
					explosionLine(x0, y0, z0, x0 - k, y0 + i, z0 - j);

					explosionLine(x0, y0, z0, x0 + i, y0 - j, z0 - k);
					explosionLine(x0, y0, z0, x0 + i, y0 - k, z0 - j);
					explosionLine(x0, y0, z0, x0 + j, y0 - k, z0 - i);
					explosionLine(x0, y0, z0, x0 + j, y0 - i, z0 - k);
					explosionLine(x0, y0, z0, x0 + k, y0 - j, z0 - i);
					explosionLine(x0, y0, z0, x0 + k, y0 - i, z0 - j);

					explosionLine(x0, y0, z0, x0 - i, y0 - j, z0 - k);
					explosionLine(x0, y0, z0, x0 - i, y0 - k, z0 - j);
					explosionLine(x0, y0, z0, x0 - j, y0 - k, z0 - i);
					explosionLine(x0, y0, z0, x0 - j, y0 - i, z0 - k);
					explosionLine(x0, y0, z0, x0 - k, y0 - j, z0 - i);
					explosionLine(x0, y0, z0, x0 - k, y0 - i, z0 - j);
				}

				s += 2 * j + 1;
				j++;
			}

			s0 += 4 * i + 2;
			i++;

			while (s0 > v0 && i <= k0)
			{
				k0--;
				v0 += l0;
				l0 -= 2;
			}
			j = i;
			k = k0;
			v = v0;
			l = l0;
			s = s0;
		}

		//  std::cout << elementProcessingCount << " elements were processed. " << (elementProcessingCount / pow(r * 2 + 1, 3)) << " times the proper amount" << std::endl;

	}

	// Test with particles

	void explosionLineTest(int x0, int y0, int z0, int x1, int y1, int z1)
	{
		// x0,y0,z0 is starting point

		int startx = x0, starty = y0, startz = z0;

		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int dz = abs(z1 - z0), sz = z0 < z1 ? 1 : -1;
		int dm = dx > dy && dx > dz ? dx : dy > dz ? dy : dz, i = dm;
		int x2 = x1, y2 = y1, z2 = z1;
		x1 = y1 = z1 = dm / 2;

		int rand = util::randXOR() % 255;

		double power = 1.0f;

		while (true)
		{
			// SetElement used to be here

			//if (i-- == 0) break;
			if (x0 == x2 && y0 == y2 && z0 == z2) break;

			// x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; }
			// y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; }
			// z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; }

			x1 -= dx; y1 -= dy; z1 -= dz;
			if (x1 < 0)
			{
				x1 += dm; x0 += sx;

				if (VoxelIsInside(x0, y0, z0) && getElement(x0, y0, z0).elementID != 14) // wall
				{
					if (getElement(x0, y0, z0).elementID != 0) // An else should be included which calls stepChunk
					{
						int dx = abs(x0 - startx);
						int dy = abs(y0 - starty);
						int dz = abs(z0 - startz);

						double length = power / sqrt(dx * dx + dy * dy + dz * dz);

						setParticle(x0, y0, z0, sx * dx * length, dy * length, sz * dz * length);
					}

					elementProcessingCount++;
				}
				else break;
			}
			if (y1 < 0)
			{
				y1 += dm; y0 += sy;

				if (VoxelIsInside(x0, y0, z0) && getElement(x0, y0, z0).elementID != 14)
				{
					if (getElement(x0, y0, z0).elementID != 0) // An else should be included which calls stepChunk
					{
						int dx = abs(x0 - startx);
						int dy = abs(y0 - starty);
						int dz = abs(z0 - startz);

						double length = power / sqrt(dx * dx + dy * dy + dz * dz);

						setParticle(x0, y0, z0, sx * dx * length, dy * length, sz * dz * length);
					}

					elementProcessingCount++;
				}
				else break;
			}
			if (z1 < 0)
			{
				z1 += dm; z0 += sz;

				if (VoxelIsInside(x0, y0, z0) && getElement(x0, y0, z0).elementID != 14)
				{
					if (getElement(x0, y0, z0).elementID != 0) // An else should be included which calls stepChunk
					{
						int dx = abs(x0 - startx);
						int dy = abs(y0 - starty);
						int dz = abs(z0 - startz);

						double length = power / sqrt(dx * dx + dy * dy + dz * dz);

						setParticle(x0, y0, z0, sx * dx * length, dy * length, sz * dz * length);
					}

					elementProcessingCount++;
				}
				else break;
			}
		}
	}

	void explode2Test(unsigned int x0, unsigned int y0, unsigned int z0, unsigned int r)
	{
		glm::vec3 dist = glm::vec3(x0 + 0.5f, y0 + 0.5f, z0 + 0.5f) - game::positionVec;
		float length = glm::length(dist);
		game::addTrauma((float)r / length);

		int i = 0, j = 0;
		int k = r, k0 = r;
		int s = 0, s0 = 0;
		int v = r - 1, v0 = r - 1;
		int l = 2 * v0, l0 = 2 * v0;

		elementProcessingCount = 0;

		while (i <= k)
		{
			while (j <= k)
			{
				if (s > v)
				{
					k--;
					v += l;
					l -= 2;
				}

				if (j <= k && (s != v || j != k))
				{
					explosionLineTest(x0, y0, z0, x0 + i, y0 + j, z0 + k);
					explosionLineTest(x0, y0, z0, x0 + i, y0 + k, z0 + j);
					explosionLineTest(x0, y0, z0, x0 + j, y0 + k, z0 + i);
					explosionLineTest(x0, y0, z0, x0 + j, y0 + i, z0 + k);
					explosionLineTest(x0, y0, z0, x0 + k, y0 + j, z0 + i);
					explosionLineTest(x0, y0, z0, x0 + k, y0 + i, z0 + j);

					explosionLineTest(x0, y0, z0, x0 - i, y0 + j, z0 + k);
					explosionLineTest(x0, y0, z0, x0 - i, y0 + k, z0 + j);
					explosionLineTest(x0, y0, z0, x0 - j, y0 + k, z0 + i);
					explosionLineTest(x0, y0, z0, x0 - j, y0 + i, z0 + k);
					explosionLineTest(x0, y0, z0, x0 - k, y0 + j, z0 + i);
					explosionLineTest(x0, y0, z0, x0 - k, y0 + i, z0 + j);
								 
					explosionLineTest(x0, y0, z0, x0 + i, y0 - j, z0 + k);
					explosionLineTest(x0, y0, z0, x0 + i, y0 - k, z0 + j);
					explosionLineTest(x0, y0, z0, x0 + j, y0 - k, z0 + i);
					explosionLineTest(x0, y0, z0, x0 + j, y0 - i, z0 + k);
					explosionLineTest(x0, y0, z0, x0 + k, y0 - j, z0 + i);
					explosionLineTest(x0, y0, z0, x0 + k, y0 - i, z0 + j);
								 
					explosionLineTest(x0, y0, z0, x0 - i, y0 - j, z0 + k);
					explosionLineTest(x0, y0, z0, x0 - i, y0 - k, z0 + j);
					explosionLineTest(x0, y0, z0, x0 - j, y0 - k, z0 + i);
					explosionLineTest(x0, y0, z0, x0 - j, y0 - i, z0 + k);
					explosionLineTest(x0, y0, z0, x0 - k, y0 - j, z0 + i);
					explosionLineTest(x0, y0, z0, x0 - k, y0 - i, z0 + j);
								 
					explosionLineTest(x0, y0, z0, x0 + i, y0 + j, z0 - k);
					explosionLineTest(x0, y0, z0, x0 + i, y0 + k, z0 - j);
					explosionLineTest(x0, y0, z0, x0 + j, y0 + k, z0 - i);
					explosionLineTest(x0, y0, z0, x0 + j, y0 + i, z0 - k);
					explosionLineTest(x0, y0, z0, x0 + k, y0 + j, z0 - i);
					explosionLineTest(x0, y0, z0, x0 + k, y0 + i, z0 - j);
								 
					explosionLineTest(x0, y0, z0, x0 - i, y0 + j, z0 - k);
					explosionLineTest(x0, y0, z0, x0 - i, y0 + k, z0 - j);
					explosionLineTest(x0, y0, z0, x0 - j, y0 + k, z0 - i);
					explosionLineTest(x0, y0, z0, x0 - j, y0 + i, z0 - k);
					explosionLineTest(x0, y0, z0, x0 - k, y0 + j, z0 - i);
					explosionLineTest(x0, y0, z0, x0 - k, y0 + i, z0 - j);
								 
					explosionLineTest(x0, y0, z0, x0 + i, y0 - j, z0 - k);
					explosionLineTest(x0, y0, z0, x0 + i, y0 - k, z0 - j);
					explosionLineTest(x0, y0, z0, x0 + j, y0 - k, z0 - i);
					explosionLineTest(x0, y0, z0, x0 + j, y0 - i, z0 - k);
					explosionLineTest(x0, y0, z0, x0 + k, y0 - j, z0 - i);
					explosionLineTest(x0, y0, z0, x0 + k, y0 - i, z0 - j);
								 
					explosionLineTest(x0, y0, z0, x0 - i, y0 - j, z0 - k);
					explosionLineTest(x0, y0, z0, x0 - i, y0 - k, z0 - j);
					explosionLineTest(x0, y0, z0, x0 - j, y0 - k, z0 - i);
					explosionLineTest(x0, y0, z0, x0 - j, y0 - i, z0 - k);
					explosionLineTest(x0, y0, z0, x0 - k, y0 - j, z0 - i);
					explosionLineTest(x0, y0, z0, x0 - k, y0 - i, z0 - j);
				}

				s += 2 * j + 1;
				j++;
			}

			s0 += 4 * i + 2;
			i++;

			while (s0 > v0 && i <= k0)
			{
				k0--;
				v0 += l0;
				l0 -= 2;
			}
			j = i;
			k = k0;
			v = v0;
			l = l0;
			s = s0;
		}

		//  std::cout << elementProcessingCount << " elements were processed. " << (elementProcessingCount / pow(r * 2 + 1, 3)) << " times the proper amount" << std::endl;

	}

	void explosionPoint(int xFirst, int yFirst, int zFirst, int xLast, int yLast, int zLast)
	{
		if (VoxelIsInside(xFirst, yFirst, zFirst) && getElement(xFirst, yFirst, zFirst).elementID != 14 &&
			VoxelIsInside(xLast, yLast, zLast) && getElement(xLast, yLast, zLast).elementID == 0)
		{
			if (getElement(xFirst, yFirst, zFirst).elementID != 0)
			{
				setElement(xFirst, yFirst, zFirst, 0);
			}
		}
	}

	void explosionLine2(int x, int y, int z, int i, int j, int k)
	{
		setElement(x, y, z, 0);

		int xLast = 0, yLast = 0, zLast = 0; // Only use this after the first iteration

		int x0 = 0, y0 = 0, z0 = 0;
		int x1 = i, y1 = j, z1 = k;

		int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
		int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
		int dz = abs(z1 - z0), sz = z0 < z1 ? 1 : -1;
		int dm = dx > dy && dx > dz ? dx : dy > dz ? dy : dz;
		int x2 = x1, y2 = y1, z2 = z1;
		x1 = y1 = z1 = dm / 2;


		while (true)
		{
			if (x0 == x2 && y0 == y2 && z0 == z2) break;

			x1 -= dx; y1 -= dy; z1 -= dz;
			if (x1 < 0)
			{
				x1 += dm; x0 += sx;

				// If first dont do last check because then nothing will be deleted

				//setElement(x + x0, y + y0, z + z0, identification["wall"]);

				explosionPoint(x + x0, y + y0, z + z0, x + xLast, y + yLast, z + zLast);
				explosionPoint(x + x0, y + z0, z + y0, x + xLast, y + zLast, z + yLast);
				explosionPoint(x + y0, y + z0, z + x0, x + yLast, y + zLast, z + xLast);
				explosionPoint(x + y0, y + x0, z + z0, x + yLast, y + xLast, z + zLast);
				explosionPoint(x + z0, y + y0, z + x0, x + zLast, y + yLast, z + xLast);
				explosionPoint(x + z0, y + x0, z + y0, x + zLast, y + xLast, z + yLast);

				explosionPoint(x - x0, y + y0, z + z0, x - xLast, y + yLast, z + zLast);
				explosionPoint(x - x0, y + z0, z + y0, x - xLast, y + zLast, z + yLast);
				explosionPoint(x - y0, y + z0, z + x0, x - yLast, y + zLast, z + xLast);
				explosionPoint(x - y0, y + x0, z + z0, x - yLast, y + xLast, z + zLast);
				explosionPoint(x - z0, y + y0, z + x0, x - zLast, y + yLast, z + xLast);
				explosionPoint(x - z0, y + x0, z + y0, x - zLast, y + xLast, z + yLast);

				explosionPoint(x + x0, y - y0, z + z0, x + xLast, y - yLast, z + zLast);
				explosionPoint(x + x0, y - z0, z + y0, x + xLast, y - zLast, z + yLast);
				explosionPoint(x + y0, y - z0, z + x0, x + yLast, y - zLast, z + xLast);
				explosionPoint(x + y0, y - x0, z + z0, x + yLast, y - xLast, z + zLast);
				explosionPoint(x + z0, y - y0, z + x0, x + zLast, y - yLast, z + xLast);
				explosionPoint(x + z0, y - x0, z + y0, x + zLast, y - xLast, z + yLast);

				explosionPoint(x - x0, y - y0, z + z0, x - xLast, y - yLast, z + zLast);
				explosionPoint(x - x0, y - z0, z + y0, x - xLast, y - zLast, z + yLast);
				explosionPoint(x - y0, y - z0, z + x0, x - yLast, y - zLast, z + xLast);
				explosionPoint(x - y0, y - x0, z + z0, x - yLast, y - xLast, z + zLast);
				explosionPoint(x - z0, y - y0, z + x0, x - zLast, y - yLast, z + xLast);
				explosionPoint(x - z0, y - x0, z + y0, x - zLast, y - xLast, z + yLast);

				explosionPoint(x + x0, y + y0, z - z0, x + xLast, y + yLast, z - zLast);
				explosionPoint(x + x0, y + z0, z - y0, x + xLast, y + zLast, z - yLast);
				explosionPoint(x + y0, y + z0, z - x0, x + yLast, y + zLast, z - xLast);
				explosionPoint(x + y0, y + x0, z - z0, x + yLast, y + xLast, z - zLast);
				explosionPoint(x + z0, y + y0, z - x0, x + zLast, y + yLast, z - xLast);
				explosionPoint(x + z0, y + x0, z - y0, x + zLast, y + xLast, z - yLast);

				explosionPoint(x - x0, y + y0, z - z0, x - xLast, y + yLast, z - zLast);
				explosionPoint(x - x0, y + z0, z - y0, x - xLast, y + zLast, z - yLast);
				explosionPoint(x - y0, y + z0, z - x0, x - yLast, y + zLast, z - xLast);
				explosionPoint(x - y0, y + x0, z - z0, x - yLast, y + xLast, z - zLast);
				explosionPoint(x - z0, y + y0, z - x0, x - zLast, y + yLast, z - xLast);
				explosionPoint(x - z0, y + x0, z - y0, x - zLast, y + xLast, z - yLast);

				explosionPoint(x + x0, y - y0, z - z0, x + xLast, y - yLast, z - zLast);
				explosionPoint(x + x0, y - z0, z - y0, x + xLast, y - zLast, z - yLast);
				explosionPoint(x + y0, y - z0, z - x0, x + yLast, y - zLast, z - xLast);
				explosionPoint(x + y0, y - x0, z - z0, x + yLast, y - xLast, z - zLast);
				explosionPoint(x + z0, y - y0, z - x0, x + zLast, y - yLast, z - xLast);
				explosionPoint(x + z0, y - x0, z - y0, x + zLast, y - xLast, z - yLast);

				explosionPoint(x - x0, y - y0, z - z0, x - xLast, y - yLast, z - zLast);
				explosionPoint(x - x0, y - z0, z - y0, x - xLast, y - zLast, z - yLast);
				explosionPoint(x - y0, y - z0, z - x0, x - yLast, y - zLast, z - xLast);
				explosionPoint(x - y0, y - x0, z - z0, x - yLast, y - xLast, z - zLast);
				explosionPoint(x - z0, y - y0, z - x0, x - zLast, y - yLast, z - xLast);
				explosionPoint(x - z0, y - x0, z - y0, x - zLast, y - xLast, z - yLast);

				// Put processing here
			}
			if (y1 < 0)
			{
				y1 += dm; y0 += sy;
				//setElement(x + x0, y + y0, z + z0, identification["wall"]);

				explosionPoint(x + x0, y + y0, z + z0, x + xLast, y + yLast, z + zLast);
				explosionPoint(x + x0, y + z0, z + y0, x + xLast, y + zLast, z + yLast);
				explosionPoint(x + y0, y + z0, z + x0, x + yLast, y + zLast, z + xLast);
				explosionPoint(x + y0, y + x0, z + z0, x + yLast, y + xLast, z + zLast);
				explosionPoint(x + z0, y + y0, z + x0, x + zLast, y + yLast, z + xLast);
				explosionPoint(x + z0, y + x0, z + y0, x + zLast, y + xLast, z + yLast);

				explosionPoint(x - x0, y + y0, z + z0, x - xLast, y + yLast, z + zLast);
				explosionPoint(x - x0, y + z0, z + y0, x - xLast, y + zLast, z + yLast);
				explosionPoint(x - y0, y + z0, z + x0, x - yLast, y + zLast, z + xLast);
				explosionPoint(x - y0, y + x0, z + z0, x - yLast, y + xLast, z + zLast);
				explosionPoint(x - z0, y + y0, z + x0, x - zLast, y + yLast, z + xLast);
				explosionPoint(x - z0, y + x0, z + y0, x - zLast, y + xLast, z + yLast);

				explosionPoint(x + x0, y - y0, z + z0, x + xLast, y - yLast, z + zLast);
				explosionPoint(x + x0, y - z0, z + y0, x + xLast, y - zLast, z + yLast);
				explosionPoint(x + y0, y - z0, z + x0, x + yLast, y - zLast, z + xLast);
				explosionPoint(x + y0, y - x0, z + z0, x + yLast, y - xLast, z + zLast);
				explosionPoint(x + z0, y - y0, z + x0, x + zLast, y - yLast, z + xLast);
				explosionPoint(x + z0, y - x0, z + y0, x + zLast, y - xLast, z + yLast);

				explosionPoint(x - x0, y - y0, z + z0, x - xLast, y - yLast, z + zLast);
				explosionPoint(x - x0, y - z0, z + y0, x - xLast, y - zLast, z + yLast);
				explosionPoint(x - y0, y - z0, z + x0, x - yLast, y - zLast, z + xLast);
				explosionPoint(x - y0, y - x0, z + z0, x - yLast, y - xLast, z + zLast);
				explosionPoint(x - z0, y - y0, z + x0, x - zLast, y - yLast, z + xLast);
				explosionPoint(x - z0, y - x0, z + y0, x - zLast, y - xLast, z + yLast);

				explosionPoint(x + x0, y + y0, z - z0, x + xLast, y + yLast, z - zLast);
				explosionPoint(x + x0, y + z0, z - y0, x + xLast, y + zLast, z - yLast);
				explosionPoint(x + y0, y + z0, z - x0, x + yLast, y + zLast, z - xLast);
				explosionPoint(x + y0, y + x0, z - z0, x + yLast, y + xLast, z - zLast);
				explosionPoint(x + z0, y + y0, z - x0, x + zLast, y + yLast, z - xLast);
				explosionPoint(x + z0, y + x0, z - y0, x + zLast, y + xLast, z - yLast);

				explosionPoint(x - x0, y + y0, z - z0, x - xLast, y + yLast, z - zLast);
				explosionPoint(x - x0, y + z0, z - y0, x - xLast, y + zLast, z - yLast);
				explosionPoint(x - y0, y + z0, z - x0, x - yLast, y + zLast, z - xLast);
				explosionPoint(x - y0, y + x0, z - z0, x - yLast, y + xLast, z - zLast);
				explosionPoint(x - z0, y + y0, z - x0, x - zLast, y + yLast, z - xLast);
				explosionPoint(x - z0, y + x0, z - y0, x - zLast, y + xLast, z - yLast);

				explosionPoint(x + x0, y - y0, z - z0, x + xLast, y - yLast, z - zLast);
				explosionPoint(x + x0, y - z0, z - y0, x + xLast, y - zLast, z - yLast);
				explosionPoint(x + y0, y - z0, z - x0, x + yLast, y - zLast, z - xLast);
				explosionPoint(x + y0, y - x0, z - z0, x + yLast, y - xLast, z - zLast);
				explosionPoint(x + z0, y - y0, z - x0, x + zLast, y - yLast, z - xLast);
				explosionPoint(x + z0, y - x0, z - y0, x + zLast, y - xLast, z - yLast);

				explosionPoint(x - x0, y - y0, z - z0, x - xLast, y - yLast, z - zLast);
				explosionPoint(x - x0, y - z0, z - y0, x - xLast, y - zLast, z - yLast);
				explosionPoint(x - y0, y - z0, z - x0, x - yLast, y - zLast, z - xLast);
				explosionPoint(x - y0, y - x0, z - z0, x - yLast, y - xLast, z - zLast);
				explosionPoint(x - z0, y - y0, z - x0, x - zLast, y - yLast, z - xLast);
				explosionPoint(x - z0, y - x0, z - y0, x - zLast, y - xLast, z - yLast);
			}
			if (z1 < 0)
			{
				z1 += dm; z0 += sz;

				//setElement(x + x0, y + y0, z + z0, identification["wall"]);

				explosionPoint(x + x0, y + y0, z + z0, x + xLast, y + yLast, z + zLast);
				explosionPoint(x + x0, y + z0, z + y0, x + xLast, y + zLast, z + yLast);
				explosionPoint(x + y0, y + z0, z + x0, x + yLast, y + zLast, z + xLast);
				explosionPoint(x + y0, y + x0, z + z0, x + yLast, y + xLast, z + zLast);
				explosionPoint(x + z0, y + y0, z + x0, x + zLast, y + yLast, z + xLast);
				explosionPoint(x + z0, y + x0, z + y0, x + zLast, y + xLast, z + yLast);

				explosionPoint(x - x0, y + y0, z + z0, x - xLast, y + yLast, z + zLast);
				explosionPoint(x - x0, y + z0, z + y0, x - xLast, y + zLast, z + yLast);
				explosionPoint(x - y0, y + z0, z + x0, x - yLast, y + zLast, z + xLast);
				explosionPoint(x - y0, y + x0, z + z0, x - yLast, y + xLast, z + zLast);
				explosionPoint(x - z0, y + y0, z + x0, x - zLast, y + yLast, z + xLast);
				explosionPoint(x - z0, y + x0, z + y0, x - zLast, y + xLast, z + yLast);

				explosionPoint(x + x0, y - y0, z + z0, x + xLast, y - yLast, z + zLast);
				explosionPoint(x + x0, y - z0, z + y0, x + xLast, y - zLast, z + yLast);
				explosionPoint(x + y0, y - z0, z + x0, x + yLast, y - zLast, z + xLast);
				explosionPoint(x + y0, y - x0, z + z0, x + yLast, y - xLast, z + zLast);
				explosionPoint(x + z0, y - y0, z + x0, x + zLast, y - yLast, z + xLast);
				explosionPoint(x + z0, y - x0, z + y0, x + zLast, y - xLast, z + yLast);

				explosionPoint(x - x0, y - y0, z + z0, x - xLast, y - yLast, z + zLast);
				explosionPoint(x - x0, y - z0, z + y0, x - xLast, y - zLast, z + yLast);
				explosionPoint(x - y0, y - z0, z + x0, x - yLast, y - zLast, z + xLast);
				explosionPoint(x - y0, y - x0, z + z0, x - yLast, y - xLast, z + zLast);
				explosionPoint(x - z0, y - y0, z + x0, x - zLast, y - yLast, z + xLast);
				explosionPoint(x - z0, y - x0, z + y0, x - zLast, y - xLast, z + yLast);

				explosionPoint(x + x0, y + y0, z - z0, x + xLast, y + yLast, z - zLast);
				explosionPoint(x + x0, y + z0, z - y0, x + xLast, y + zLast, z - yLast);
				explosionPoint(x + y0, y + z0, z - x0, x + yLast, y + zLast, z - xLast);
				explosionPoint(x + y0, y + x0, z - z0, x + yLast, y + xLast, z - zLast);
				explosionPoint(x + z0, y + y0, z - x0, x + zLast, y + yLast, z - xLast);
				explosionPoint(x + z0, y + x0, z - y0, x + zLast, y + xLast, z - yLast);

				explosionPoint(x - x0, y + y0, z - z0, x - xLast, y + yLast, z - zLast);
				explosionPoint(x - x0, y + z0, z - y0, x - xLast, y + zLast, z - yLast);
				explosionPoint(x - y0, y + z0, z - x0, x - yLast, y + zLast, z - xLast);
				explosionPoint(x - y0, y + x0, z - z0, x - yLast, y + xLast, z - zLast);
				explosionPoint(x - z0, y + y0, z - x0, x - zLast, y + yLast, z - xLast);
				explosionPoint(x - z0, y + x0, z - y0, x - zLast, y + xLast, z - yLast);

				explosionPoint(x + x0, y - y0, z - z0, x + xLast, y - yLast, z - zLast);
				explosionPoint(x + x0, y - z0, z - y0, x + xLast, y - zLast, z - yLast);
				explosionPoint(x + y0, y - z0, z - x0, x + yLast, y - zLast, z - xLast);
				explosionPoint(x + y0, y - x0, z - z0, x + yLast, y - xLast, z - zLast);
				explosionPoint(x + z0, y - y0, z - x0, x + zLast, y - yLast, z - xLast);
				explosionPoint(x + z0, y - x0, z - y0, x + zLast, y - xLast, z - yLast);

				explosionPoint(x - x0, y - y0, z - z0, x - xLast, y - yLast, z - zLast);
				explosionPoint(x - x0, y - z0, z - y0, x - xLast, y - zLast, z - yLast);
				explosionPoint(x - y0, y - z0, z - x0, x - yLast, y - zLast, z - xLast);
				explosionPoint(x - y0, y - x0, z - z0, x - yLast, y - xLast, z - zLast);
				explosionPoint(x - z0, y - y0, z - x0, x - zLast, y - yLast, z - xLast);
				explosionPoint(x - z0, y - x0, z - y0, x - zLast, y - xLast, z - yLast);
			}
			xLast = x0;
			yLast = y0;
			zLast = z0;
		}

		//std::cout << "It exits" << std::endl;
	}

	void explode2a(unsigned int x, unsigned int y, unsigned int z, unsigned int r)
	{
		glm::vec3 dist = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f) - game::positionVec;
		float length = glm::length(dist);
		game::addTrauma((float)r / length);

		int i = 0, j = 0;
		int k = r, k0 = r;
		int s = 0, s0 = 0;
		int v = r - 1, v0 = r - 1;
		int l = 2 * v0, l0 = 2 * v0;

		elementProcessingCount = 0;

		while (i <= k)
		{
			while (j <= k)
			{
				if (s > v)
				{
					k--;
					v += l;
					l -= 2;
				}

				if (j <= k && (s != v || j != k))
				{
					// Add bresenham here

					// explosionLine(x0, y0, z0, x0 + i, y0 + j, z0 + k);
					explosionLine2(x, y, z, i, j, k);
				}

				s += 2 * j + 1;
				j++;
			}

			s0 += 4 * i + 2;
			i++;

			while (s0 > v0 && i <= k0)
			{
				k0--;
				v0 += l0;
				l0 -= 2;
			}

			j = i; k = k0; v = v0; l = l0; s = s0;
		}

		std::cout << elementProcessingCount << " elements were processed. " << (elementProcessingCount / pow(r * 2 + 1, 3)) << " times the proper amount" << std::endl;

	}

	void explode(unsigned int x, unsigned int y, unsigned int z, int radius)
	{
		glm::vec3 dist = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f) - game::positionVec;
		float length = glm::length(dist);
		game::addTrauma((float)radius / length);

		elementProcessingCount = 0;


		for (game::index3d i(radius * 2 + 1); !i.finished(); i++)
		{
			/*if (VoxelIsInside(x + i.x - radius, y + i.y - radius, z + i.z - radius) && getElement(x + i.x - radius, y + i.y - radius, z + i.z - radius).elementID != 14)
			{
				setElement(x + i.x - radius, y + i.y - radius, z + i.z - radius, identification["explosion flash"]);
			}*/

			// If at border
			if ((i.x & i.y & i.z & (i.x - i.xSize + 1) & (i.y - i.ySize + 1) & (i.z - i.zSize + 1)) == 0)
			{
				glm::vec3 dir = glm::normalize(glm::vec3((int)i.x - radius, (int)i.y - radius, (int)i.z - radius));
				glm::vec3 pos = glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f) + (float)radius * dir;
				glm::ivec3 mapPos = glm::ivec3(pos);

				explosionLine(x, y, z, mapPos.x, mapPos.y, mapPos.z);
			}
		}

		std::cout << elementProcessingCount << " elements were processed. " << (elementProcessingCount / pow(radius * 2 + 1, 3)) << " times the proper amount" << std::endl;
	}

	#pragma endregion

	void bombUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		bool explosion = false;

		if (game::elementData[update.index].registerI == true) explosion = true;
		else if (game::elementData[update.index].registerL > 0) explosion = true;

		if (explosion)
		{
			explode2Test(x, y, z, 16); // take out test to put it back to normal
			setElement(x, y, z, 0);
			return;
		}
		
		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	void explosionUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		explode2(x, y, z, 16);
		setElement(x, y, z, 0);
	}

	void explosionFlashUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);

		if (game::elementData[update.index].registerG-- == 0) setElement(x, y, z, 0);
	}

	void rgbBlockUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		unsigned char r = 255 * (0.5 + 0.5 * cos(game::currentTime + x / (double)game::bounds + 0));
		unsigned char g = 255 * (0.5 + 0.5 * cos(game::currentTime + y / (double)game::bounds + 2));
		unsigned char b = 255 * (0.5 + 0.5 * cos(game::currentTime + z / (double)game::bounds + 4));

		game::voxData[update.index] = util::RGBA(r, g, b, 255);

		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
	}

	void wetConcreteUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		//if (game::elementData[update.index].registerG == 0)
		//{
		//	setElement(x, y, z, identification["wall"]);
		//	return;
		//}
		//game::elementData[update.index].registerG--;

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			game::elementData[update.index].registerA = 0;
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			game::elementData[update.index].registerA = 0;
		}
		else if (VoxelIsInside(x + dir1, y, z + dir2) && getDefinition(x + dir1, y, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			game::elementData[update.index].registerA = 0;
		}
		else { game::elementData[update.index].registerA++; }

		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);

		if (game::elementData[update.index].registerA == 31)
		{
			setElement(x, y, z, identification["concrete"]);
		}

		
	}

	void upwardWallUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerH > 0)
		{
			game::elementData[update.index].registerH--;

			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
			return;
		}

		for (int i = -1; i < 2; i++)
		{
			if (i == 0)
			{
				setElement(x, y, z, 14);
			}
			else if (VoxelIsInside(x, y + i, z) && getElement(x, y + i, z).elementID == 0)
			{
				setElement(x, y + i, z, 34);
			}
		}
	}

	void biggusBombusUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		bool explosion = false;

		if (game::elementData[update.index].registerI == true) explosion = true;
		else if (game::elementData[update.index].registerL > 0) explosion = true;

		if (explosion)
		{
			explode2(x, y, z, 64);
			setElement(x, y, z, 0);
			return;
		}

		int dir1 = util::direction();
		int dir2 = util::direction();

		if (VoxelIsInside(x, y - 1, z) && getDefinition(x, y - 1, z).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
		else if (VoxelIsInside(x + dir1, y - 1, z + dir2) && getDefinition(x + dir1, y - 1, z + dir2).isFluid && getDefinition(game::currentIndex).density < getDefinition(update.index).density)
		{
			swapElement(update.index, game::currentIndex);
			StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
		}
	}

	/*void worldDeleterUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		if (game::elementData[update.index].registerH == 4)
		{
			for (int i = -1; i < 2; i++)
			{
				for (int j = -1; j < 2; j++)
				{
					for (int k = -1; k < 2; k++)
					{
						if (VoxelIsInside(x + k, y + j, z + i) && getElement(x + k, y + j, z + i).elementID != 34)
						{
							setElement(x + k, y + j, z + i, 34);
							game::elementData[game::currentIndex].clock = 1;
						}
					}
				}
			}
		}
		else if (game::elementData[update.index].registerH == 0)
		{
			setElement(x, y, z, 0);
			return;
		}

		game::elementData[update.index].registerH--;

		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
	}*/

	/*void particleUpdate(unsigned int x, unsigned int y, unsigned int z)
	{
		game::elementData[update.index].clock = 1;

		// 00000
		// 000   - are for position	 (0.125 accuracy) 
		//    00 - are for direction (0.250 accuracy)

		float dirX = (int)game::elementData[update.index].registerA - 15 + (game::elementData[update.index].registerD & 0b11) * 0.25f;
		float dirY = (int)game::elementData[update.index].registerB - 15 + (game::elementData[update.index].registerE & 0b11) * 0.25f;
		float dirZ = (int)game::elementData[update.index].registerC - 15 + (game::elementData[update.index].registerF & 0b11) * 0.25f;

		float posX = x + (game::elementData[update.index].registerD >> 2) * 0.125f;
		float posY = y + (game::elementData[update.index].registerE >> 2) * 0.125f;
		float posZ = z + (game::elementData[update.index].registerF >> 2) * 0.125f;

		// add 0.25 to dirYf
		// If the simulation fps is 30 per second, then the downwards acceleration is roughly 7.5 m/s^2
		// NOTE: We might need to make direction 

		dirY += 0.250f;

		posX += dirX;
		posY += dirY;
		posZ += dirZ;

		//unsigned int 

		StepChunk2(update.localX, update.localY, update.localZ, update.chunkX, update.chunkY, update.chunkZ);
	}*/



#pragma endregion

	void InitElementDefinitions()
	{
		categoryList.insert(categoryList.end(), {"solid", "liquid", "gas", "life", "energy", "destruction", "electricity", "misc"});

		// Element: air
		{
			game::elementDescription airDesc;
			airDesc.name = "air";
			airDesc.description = "This is just empty space";
			airDesc.category = game::categories::gas;
			airDesc.hidden = true;
			descriptionList.push_back(airDesc);

			game::elementDefinition air;
			air.init = emptyInit;
			air.update = emptyUpdate;
			air.reaction = noReaction;
			air.color1 = 0;
			air.color2 = 0;
			air.density = 123; // Density of air is 1.225 kg/m^3 (1.225 * 100 = 123)
			air.isFluid = true;
			air.isFlammable = false;
			air.flammability = 0;
			air.isConductive = false;
			air.placement = game::placement::hit;
			definitionsList.push_back(air);
			identification[airDesc.name] = definitionsList.size() - 1;
		}

		// Element: sand
		{
			game::elementDescription sandDesc;
			sandDesc.name = "sand";
			sandDesc.description = "It's in the name";
			sandDesc.category = game::categories::solid;
			sandDesc.hidden = false;
			descriptionList.push_back(sandDesc);

			game::elementDefinition sand;
			sand.init = emptyInit;
			sand.update = sandUpdate;
			sand.reaction = noReaction;
			sand.color1 = 0xFFD4FAFD;
			sand.color2 = 0xFF5CCFFF;
			sand.density = 168000; // Density of sand is roughly 1680 kg/m^3 (1680 * 100 = 168000)
			sand.isFluid = false;
			sand.isFlammable = false;
			sand.flammability = 0;
			sand.isConductive = false;
			sand.placement = game::placement::hit;
			definitionsList.push_back(sand);
			identification[sandDesc.name] = definitionsList.size() - 1;
		}

		// Element: water
		{
			game::elementDescription waterDesc;
			waterDesc.name = "water";
			waterDesc.description = "The best drink out there";
			waterDesc.category = game::categories::liquid;
			waterDesc.hidden = false;
			descriptionList.push_back(waterDesc);

			game::elementDefinition water;
			water.init = emptyInit;
			water.update = waterUpdate;
			water.reaction = waterReaction;
			water.color1 = 0xFF9C5E0F;
			water.color2 = 0xFFF4CC74;
			water.density = 99700; // Density of water is roughly 997 kg/m^3 (997 * 100 = 99700)
			water.isFluid = true;
			water.isFlammable = false;
			water.flammability = 0;
			water.isConductive = false;
			water.placement = game::placement::hit;
			definitionsList.push_back(water);
			identification[waterDesc.name] = definitionsList.size() - 1;
		}

		// Element: smoke
		{
			game::elementDescription smokeDesc;
			smokeDesc.name = "smoke";
			smokeDesc.description = "Bad for your lungs";
			smokeDesc.category = game::categories::gas;
			smokeDesc.hidden = false;
			descriptionList.push_back(smokeDesc);

			game::elementDefinition smoke;
			smoke.init = smokeInit;
			smoke.update = smokeUpdate;
			smoke.reaction = noReaction;
			smoke.color1 = 0xFF848884;
			smoke.color2 = 0xFF5E685C;
			smoke.density = 145; // Density of smoke is 1.445 kg/m^3 (1.445 * 100 = 145)
			smoke.isFluid = true;
			smoke.isFlammable = false;
			smoke.flammability = 0;
			smoke.isConductive = false;
			smoke.placement = game::placement::hit;
			definitionsList.push_back(smoke);
			identification[smokeDesc.name] = definitionsList.size() - 1;
		}

		// Element: fire
		{
			game::elementDescription fireDesc;
			fireDesc.name = "fire";
			fireDesc.description = "Used to commit arson";
			fireDesc.category = game::categories::energy;
			fireDesc.hidden = false;
			descriptionList.push_back(fireDesc);

			game::elementDefinition fire;
			fire.init = fireInit;
			fire.update = fireUpdate;
			fire.reaction = emptyReaction;
			fire.color1 = 0xFF2019CF;
			fire.color2 = 0xFF2EBDFF;
			fire.density = 140; // Fire has no density so ill set it in between air and smoke
			fire.isFluid = true;
			fire.isFlammable = false;
			fire.flammability = 0;
			fire.isConductive = false;
			fire.placement = game::placement::hit;
			definitionsList.push_back(fire);
			identification[fireDesc.name] = definitionsList.size() - 1;
		}
		
		// Element: wood
		{
			game::elementDescription woodDesc;
			woodDesc.name = "wood";
			woodDesc.description = "Please dont burn the forests";
			woodDesc.category = game::categories::solid;
			woodDesc.hidden = false;
			descriptionList.push_back(woodDesc);

			game::elementDefinition wood;
			wood.init = woodInit;
			wood.update = woodUpdate;
			wood.reaction = emptyReaction;
			wood.color1 = 0xFF336F96;
			wood.color2 = 0xFF638CBA;
			wood.density = 75000; // Wood has an average density of 750 kg/m^3 (750 * 100 = 75000)
			wood.isFluid = false;
			wood.isFlammable = true;
			wood.flammability = 30;
			wood.isConductive = false;
			wood.placement = game::placement::hit;
			definitionsList.push_back(wood);
			identification[woodDesc.name] = definitionsList.size() - 1;
		}

		// Element: gunpowder
		{
			game::elementDescription gunpowderDesc;
			gunpowderDesc.name = "gunpowder";
			gunpowderDesc.description = "No, this isn't powdered-gun";
			gunpowderDesc.category = game::categories::solid;
			gunpowderDesc.hidden = false;
			descriptionList.push_back(gunpowderDesc);

			game::elementDefinition gunpowder;
			gunpowder.init = gunpowderInit;
			gunpowder.update = gunpowderUpdate;
			gunpowder.reaction = emptyReaction;
			gunpowder.color1 = util::RGBA(123, 123, 123, 255);
			gunpowder.color2 = util::RGBA(50, 50, 50, 255);
			gunpowder.density = 170000; // Gunpowder has a density of 1700 kg/m^3 (1700 * 100 = 170000)
			gunpowder.isFluid = false;
			gunpowder.isFlammable = true;
			gunpowder.flammability = 63;
			gunpowder.isConductive = false;
			gunpowder.placement = game::placement::hit;
			definitionsList.push_back(gunpowder);
			identification[gunpowderDesc.name] = definitionsList.size() - 1;
		}
		
		// Element: laser
		{
			game::elementDescription laserDesc;
			laserDesc.name = "laser";
			laserDesc.description = "Pew pew";
			laserDesc.category = game::categories::destruction;
			laserDesc.hidden = false;
			descriptionList.push_back(laserDesc);

			game::elementDefinition laser;
			laser.init = laserInit;
			laser.update = laserUpdate;
			laser.reaction = noReaction;
			laser.color1 = util::RGBA(255, 0, 0, 255);
			laser.color2 = util::RGBA(240, 0, 0, 255);
			laser.density = 123; // lasers dont have a density so it will the same as air
			laser.isFluid = false;
			laser.isFlammable = false;
			laser.flammability = 0;
			laser.isConductive = false;
			laser.placement = game::placement::player;
			definitionsList.push_back(laser);
			identification[laserDesc.name] = definitionsList.size() - 1;
		}
		
		// Element: charge
		{
			game::elementDescription chargeDesc;
			chargeDesc.name = "charge";
			chargeDesc.description = "don't touch, it dangerous";
			chargeDesc.category = game::categories::electricity;
			chargeDesc.hidden = false;
			descriptionList.push_back(chargeDesc);

			game::elementDefinition charge;
			charge.init = emptyInit;
			charge.update = chargeUpdate;
			charge.reaction = noReaction;
			charge.color1 = util::RGBA(255, 255, 51, 255);
			charge.color2 = util::RGBA(255, 255, 0, 255);
			charge.density = 123; // electricity doesnt have a density so it will the same as air
			charge.isFluid = false;
			charge.isFlammable = false;
			charge.flammability = 0;
			charge.isConductive = false;
			charge.placement = game::placement::hit;
			definitionsList.push_back(charge);
			identification[chargeDesc.name] = definitionsList.size() - 1;
		}

		// Element: wire
		{
			game::elementDescription wireDesc;
			wireDesc.name = "wire";
			wireDesc.description = "Electricity will travel through it";
			wireDesc.category = game::categories::electricity;
			wireDesc.hidden = false;
			descriptionList.push_back(wireDesc);

			game::elementDefinition wire;
			wire.init = emptyInit;
			wire.update = wireUpdate;
			wire.reaction = noReaction;
			wire.color1 = 0xFF2A3157;
			wire.color2 = 0xFF2E3453;
			wire.density = 896000; // wire has a density of 8960 kg/m^3 (8960 * 100 = 896000)
			wire.isFluid = false;
			wire.isFlammable = false;
			wire.flammability = 0;
			wire.isConductive = true;
			wire.placement = game::placement::hit;
			definitionsList.push_back(wire);
			identification[wireDesc.name] = definitionsList.size() - 1;
		}

		// Element: waterspout
		{
			game::elementDescription waterspoutDesc;
			waterspoutDesc.name = "waterspout";
			waterspoutDesc.description = "A spout of water";
			waterspoutDesc.category = game::categories::electricity;
			waterspoutDesc.hidden = false;
			descriptionList.push_back(waterspoutDesc);

			game::elementDefinition waterspout;
			waterspout.init = emptyInit;
			waterspout.update = waterspoutUpdate;
			waterspout.reaction = noReaction;
			waterspout.color1 = util::RGBA(71, 174, 156, 255);
			waterspout.color2 = util::RGBA(130, 205, 173, 255);
			waterspout.density = 896000; // has same density as wire
			waterspout.isFluid = false;
			waterspout.isFlammable = false;
			waterspout.flammability = 0;
			waterspout.isConductive = true;
			waterspout.placement = game::placement::hit;
			definitionsList.push_back(waterspout);
			identification[waterspoutDesc.name] = definitionsList.size() - 1;
		}

		// Element: random
		{
			game::elementDescription randomDesc;
			randomDesc.name = "random";
			randomDesc.description = "creates a random element";
			randomDesc.category = game::categories::misc;
			randomDesc.hidden = false;
			descriptionList.push_back(randomDesc);

			game::elementDefinition random;
			random.init = randomInit;
			random.update = emptyUpdate;
			random.reaction = noReaction;
			random.color1 = util::RGBA(142, 146, 148, 255);
			random.color2 = util::RGBA(152, 151, 169, 255);
			random.density = 0; // doesn't have a density
			random.isFluid = false;
			random.isFlammable = false;
			random.flammability = 0;
			random.isConductive = false;
			random.placement = game::placement::hit;
			definitionsList.push_back(random);
			identification[randomDesc.name] = definitionsList.size() - 1;
		}

		// Element: basalt
		{
			game::elementDescription basaltDesc;
			basaltDesc.name = "basalt";
			basaltDesc.description = "a type of rock that starts with b. It also rhymes with salt";
			basaltDesc.category = game::categories::solid;
			basaltDesc.hidden = false;
			descriptionList.push_back(basaltDesc);

			game::elementDefinition basalt;
			basalt.init = emptyInit;
			basalt.update = basaltUpdate;
			basalt.reaction = noReaction;
			basalt.color1 = util::RGBA(87, 93, 94, 255);
			basalt.color2 = util::RGBA(140, 140, 156, 255);
			basalt.density = 155500; // basalt has a density of 1555 kg/m^3 (1555 * 100 = 155500)
			basalt.isFluid = false;
			basalt.isFlammable = false;
			basalt.flammability = 0;
			basalt.isConductive = false;
			basalt.placement = game::placement::hit;
			definitionsList.push_back(basalt);
			identification[basaltDesc.name] = definitionsList.size() - 1;
		}

		// Element: steam
		{
			game::elementDescription steamDesc;
			steamDesc.name = "steam";
			steamDesc.description = "The smoke of water";
			steamDesc.category = game::categories::gas;
			steamDesc.hidden = false;
			descriptionList.push_back(steamDesc);

			game::elementDefinition steam;
			steam.init = emptyInit;
			steam.update = steamUpdate;
			steam.reaction = noReaction;
			steam.color1 = util::RGBA(174, 202, 200, 255);
			steam.color2 = util::RGBA(221, 221, 221, 255);
			steam.density = 182; // Density of steam is 0.5974 kg/m^3, plus the air it is in makes it 1.8224 kg/m^3 (1.8224 * 100 = 182)
			steam.isFluid = true;
			steam.isFlammable = false;
			steam.flammability = 0;
			steam.isConductive = false;
			steam.placement = game::placement::hit;
			definitionsList.push_back(steam);
			identification[steamDesc.name] = definitionsList.size() - 1;
		}

		// Element: wall
		{
			game::elementDescription wallDesc;
			wallDesc.name = "wall";
			wallDesc.description = "The strongest thing on earth";
			wallDesc.category = game::categories::solid;
			wallDesc.hidden = false;
			descriptionList.push_back(wallDesc);

			game::elementDefinition wall;
			wall.init = emptyInit;
			wall.update = emptyUpdate;
			wall.reaction = noReaction;
			wall.color1 = util::RGBA(211, 211, 211, 255);
			wall.color2 = util::RGBA(211, 211, 211, 255);
			wall.density = 240000; // Density of concrete is roughly 2400 kg/m^3 (2400 * 100 = 240000)
			wall.isFluid = false;
			wall.isFlammable = false;
			wall.flammability = 0;
			wall.isConductive = false;
			wall.placement = game::placement::hit;
			definitionsList.push_back(wall);
			identification[wallDesc.name] = definitionsList.size() - 1;
		}

		// Element: platform
		{
			game::elementDescription platformDesc;
			platformDesc.name = "platform";
			platformDesc.description = "Creates a platform made of wall";
			platformDesc.category = game::categories::misc;
			platformDesc.hidden = false;
			descriptionList.push_back(platformDesc);

			game::elementDefinition platform;
			platform.init = platformInit;
			platform.update = platformUpdate;
			platform.reaction = noReaction;
			platform.color1 = util::RGBA(139, 0, 0, 255);
			platform.color2 = util::RGBA(139, 0, 0, 255);
			platform.density = 240000;
			platform.isFluid = false;
			platform.isFlammable = false;
			platform.flammability = 0;
			platform.isConductive = false;
			platform.placement = game::placement::hit;
			definitionsList.push_back(platform);
			identification[platformDesc.name] = definitionsList.size() - 1;
		}

		// Element: acid
		{
			game::elementDescription acidDesc;
			acidDesc.name = "acid";
			acidDesc.description = "Tasted a little funny...";
			acidDesc.category = game::categories::liquid;
			acidDesc.hidden = false;
			descriptionList.push_back(acidDesc);

			game::elementDefinition acid;
			acid.init = emptyInit;
			acid.update = acidUpdate;
			acid.reaction = noReaction;
			acid.color1 = util::RGBA(0x00ff22ff);
			acid.color2 = util::RGBA(0x11ff22ff);
			acid.density = 99701; // Same as water + 1
			acid.isFluid = true;
			acid.isFlammable = false;
			acid.flammability = 0;
			acid.isConductive = false;
			acid.placement = game::placement::hit;
			definitionsList.push_back(acid);
			identification[acidDesc.name] = definitionsList.size() - 1;
		}

		// Element: lava
		{
			game::elementDescription lavaDesc;
			lavaDesc.name = "lava";
			lavaDesc.description = "Don't touch! Very hot";
			lavaDesc.category = game::categories::liquid;
			lavaDesc.hidden = false;
			descriptionList.push_back(lavaDesc);

			game::elementDefinition lava;
			lava.init = emptyInit;
			lava.update = lavaUpdate;
			lava.reaction = lavaReaction;
			lava.color1 = util::RGBA(0xfc5a1fff);
			lava.color2 = util::RGBA(0xe46f34ff);
			lava.density = 310000; // Lava has a denisty of 3100 kg/m^3 (3100 * 100 = 310000)
			lava.isFluid = true;
			lava.isFlammable = true;
			lava.flammability = 0;
			lava.isConductive = false;
			lava.placement = game::placement::hit;
			definitionsList.push_back(lava);
			identification[lavaDesc.name] = definitionsList.size() - 1;
		}

		// Element: chunk deleter
		{
			game::elementDescription chunkDeleterDesc;
			chunkDeleterDesc.name = "chunk deleter";
			chunkDeleterDesc.description = "deleter of chunks and small children. Was supposed to be called 'chunk destroyer' but that wouldn't fit on the button";
			chunkDeleterDesc.category = game::categories::misc;
			chunkDeleterDesc.hidden = false;
			descriptionList.push_back(chunkDeleterDesc);

			game::elementDefinition chunkDeleter;
			chunkDeleter.init = chunkDeleterInit;
			chunkDeleter.update = chunkDeleterUpdate;
			chunkDeleter.reaction = noReaction;
			chunkDeleter.color1 = util::RGBA(255, 0, 0, 255);
			chunkDeleter.color2 = util::RGBA(255, 0, 0, 255);
			chunkDeleter.density = 240000; // Lava has a denisty of 3100 kg/m^3 (3100 * 100 = 310000)
			chunkDeleter.isFluid = false;
			chunkDeleter.isFlammable = false;
			chunkDeleter.flammability = 0;
			chunkDeleter.isConductive = false;
			chunkDeleter.placement = game::placement::hit;
			definitionsList.push_back(chunkDeleter);
			identification[chunkDeleterDesc.name] = definitionsList.size() - 1;
		}

		// Element: current
		{
			game::elementDescription currentDesc;
			currentDesc.name = "current";
			currentDesc.description = "like charge but you can actually see it";
			currentDesc.category = game::categories::energy;
			currentDesc.hidden = false;
			descriptionList.push_back(currentDesc);

			game::elementDefinition current;
			current.init = currentInit;
			current.update = currentUpdate;
			current.reaction = noReaction;
			current.color1 = util::RGBA(255, 255, 51, 255);
			current.color2 = util::RGBA(255, 255, 0, 255);
			current.density = 123;
			current.isFluid = false;
			current.isFlammable = false;
			current.flammability = 0;
			current.isConductive = false;
			current.placement = game::placement::hit;
			definitionsList.push_back(current);
			identification[currentDesc.name] = definitionsList.size() - 1;
		}

		// Element: dirt
		{
			game::elementDescription dirtDesc;
			dirtDesc.name = "dirt";
			dirtDesc.description = "dirty stuff";
			dirtDesc.category = game::categories::solid;
			dirtDesc.hidden = false;
			descriptionList.push_back(dirtDesc);

			game::elementDefinition dirt;
			dirt.init = emptyInit;
			dirt.update = dirtUpdate;
			dirt.reaction = noReaction;
			dirt.color1 = util::RGBA(0x9b7653ff);
			dirt.color2 = util::RGBA(0x836539ff);
			dirt.density = 122000; // Density of dirt is 1220 kg / m^3
			dirt.isFluid = false;
			dirt.isFlammable = false;
			dirt.flammability = 0;
			dirt.isConductive = false;
			dirt.placement = game::placement::hit;
			definitionsList.push_back(dirt);
			identification[dirtDesc.name] = definitionsList.size() - 1;
		}

		// Element: seed
		{
			game::elementDescription seedDesc;
			seedDesc.name = "seed";
			seedDesc.description = "default seed. Every seed in the world derives from this. I think...";
			seedDesc.category = game::categories::life;
			seedDesc.hidden = false;
			descriptionList.push_back(seedDesc);

			game::elementDefinition seed;
			seed.init = seedInit;
			seed.update = seedUpdate;
			seed.reaction = emptyReaction;
			seed.color1 = util::RGBA(200, 173, 127, 255);
			seed.color2 = util::RGBA(219, 181, 161, 255);
			seed.density = 1600; // Density of grass seed is 16 kg / m^3
			seed.isFluid = false;
			seed.isFlammable = true;
			seed.flammability = 50;
			seed.isConductive = false;
			seed.placement = game::placement::hit;
			definitionsList.push_back(seed);
			identification[seedDesc.name] = definitionsList.size() - 1;
		}

		// Element: perlin worm
		{
			game::elementDescription perlinWormDesc;
			perlinWormDesc.name = "perlin worm";
			perlinWormDesc.description = "A slimy little fella";
			perlinWormDesc.category = game::categories::misc;
			perlinWormDesc.hidden = false;
			descriptionList.push_back(perlinWormDesc);

			game::elementDefinition perlinWorm;
			perlinWorm.init = emptyInit;
			perlinWorm.update = perlinWormUpdate;
			perlinWorm.reaction = noReaction;
			perlinWorm.color1 = util::RGBA(251, 195, 167, 255);
			perlinWorm.color2 = util::RGBA(251, 217, 177, 255);
			perlinWorm.density = 240000;
			perlinWorm.isFluid = false;
			perlinWorm.isFlammable = false;
			perlinWorm.flammability = 0;
			perlinWorm.isConductive = false;
			perlinWorm.placement = game::placement::hit;
			definitionsList.push_back(perlinWorm);
			identification[perlinWormDesc.name] = definitionsList.size() - 1;
		}

		// Element: sprout
		{
			game::elementDescription sproutDesc;
			sproutDesc.name = "sprout";
			sproutDesc.description = "does the stem create the sprout, or does the sprout create the stem?";
			sproutDesc.category = game::categories::life;
			sproutDesc.hidden = true;
			descriptionList.push_back(sproutDesc);

			game::elementDefinition sprout;
			sprout.init = sproutInit;
			sprout.update = sproutUpdate;
			sprout.reaction = noReaction;
			sprout.color1 = util::RGBA(0, 173, 0, 255);
			sprout.color2 = util::RGBA(0, 181, 0, 255);
			sprout.density = 50700; // Density of stem is 507 kg / m^3
			sprout.isFluid = false;
			sprout.isFlammable = false;
			sprout.flammability = 0;
			sprout.isConductive = false;
			sprout.placement = game::placement::hit;
			definitionsList.push_back(sprout);
			identification[sproutDesc.name] = definitionsList.size() - 1;
		}

		// Element: stem
		{
			game::elementDescription stemDesc;
			stemDesc.name = "stem";
			stemDesc.description = "All plants have a stem. Its like a spine";
			stemDesc.category = game::categories::life;
			stemDesc.hidden = true;
			descriptionList.push_back(stemDesc);

			game::elementDefinition stem;
			stem.init = stemInit;
			stem.update = stemUpdate;
			stem.reaction = stemReaction;
			stem.color1 = util::RGBA(0, 173, 0, 255);
			stem.color2 = util::RGBA(0, 181, 0, 255);
			stem.density = 50700; // Density of stem is 507 kg / m^3
			stem.isFluid = false;
			stem.isFlammable = true;
			stem.flammability = 40;
			stem.isConductive = false;
			stem.placement = game::placement::hit;
			definitionsList.push_back(stem);
			identification[stemDesc.name] = definitionsList.size() - 1;
		}

		// Element: flower seed
		{
			game::elementDescription flowerSeedDesc;
			flowerSeedDesc.name = "flower seed";
			flowerSeedDesc.description = "seed of a nice little flower :)";
			flowerSeedDesc.category = game::categories::life;
			flowerSeedDesc.hidden = false;
			descriptionList.push_back(flowerSeedDesc);

			game::elementDefinition flowerSeed;
			flowerSeed.init = flowerSeedInit;
			flowerSeed.update = flowerSeedUpdate;
			flowerSeed.reaction = emptyReaction;
			flowerSeed.color1 = util::RGBA(112, 75, 57, 255);
			flowerSeed.color2 = util::RGBA(217, 148, 70, 255);
			flowerSeed.density = 1600; // Density of grass seed is 16 kg / m^3
			flowerSeed.isFluid = false;
			flowerSeed.isFlammable = true;
			flowerSeed.flammability = 50;
			flowerSeed.isConductive = false;
			flowerSeed.placement = game::placement::hit;
			definitionsList.push_back(flowerSeed);
			identification[flowerSeedDesc.name] = definitionsList.size() - 1;
		}

		// Element: flower sprout
		{
			game::elementDescription flowerSproutDesc;
			flowerSproutDesc.name = "flower sprout";
			flowerSproutDesc.description = "Why is the flower sprout look like the normal sprout???";
			flowerSproutDesc.category = game::categories::life;
			flowerSproutDesc.hidden = true;
			descriptionList.push_back(flowerSproutDesc);

			game::elementDefinition flowerSprout;
			flowerSprout.init = flowerSproutInit;
			flowerSprout.update = flowerSproutUpdate;
			flowerSprout.reaction = noReaction;
			flowerSprout.color1 = util::RGBA(0, 173, 0, 255);
			flowerSprout.color2 = util::RGBA(0, 181, 0, 255);
			flowerSprout.density = 50700;
			flowerSprout.isFluid = false;
			flowerSprout.isFlammable = false;
			flowerSprout.flammability = 0;
			flowerSprout.isConductive = false;
			flowerSprout.placement = game::placement::hit;
			definitionsList.push_back(flowerSprout);
			identification[flowerSproutDesc.name] = definitionsList.size() - 1;
		}

		// Element: petal
		{
			game::elementDescription petalDesc;
			petalDesc.name = "petal";
			petalDesc.description = "Who is reading this? If you are, then please send me the string 'hlj1impkou'. I would really appreciate it.";
			petalDesc.category = game::categories::life;
			petalDesc.hidden = true;
			descriptionList.push_back(petalDesc);

			game::elementDefinition petal;
			petal.init = petalInit;
			petal.update = petalUpdate;
			petal.reaction = emptyReaction;
			petal.color1 = util::RGBA(255, 255, 255, 255);
			petal.color2 = util::RGBA(255, 255, 255, 255);
			petal.density = 50700; // Density of stem is 507 kg / m^3
			petal.isFluid = false;
			petal.isFlammable = true;
			petal.flammability = 40;
			petal.isConductive = false;
			petal.placement = game::placement::hit;
			definitionsList.push_back(petal);
			identification[petalDesc.name] = definitionsList.size() - 1;
		}

		// Element: explosion
		{
			game::elementDescription explosionDesc;
			explosionDesc.name = "explosion";
			explosionDesc.description = "BOOOOOOOOOOOM";
			explosionDesc.category = game::categories::destruction;
			explosionDesc.hidden = false;
			descriptionList.push_back(explosionDesc);

			game::elementDefinition explosion;
			explosion.init = emptyInit;
			explosion.update = explosionUpdate;
			explosion.reaction = noReaction;
			explosion.color1 = util::RGBA(0xd4ac54ff);
			explosion.color2 = util::RGBA(0x7a0808ff);
			explosion.density = 123; // electricity doesnt have a density so it will the same as air
			explosion.isFluid = false;
			explosion.isFlammable = false;
			explosion.flammability = 0;
			explosion.isConductive = false;
			explosion.placement = game::placement::hit;
			definitionsList.push_back(explosion);
			identification[explosionDesc.name] = definitionsList.size() - 1;
		}

		// Element: bomb
		{
			game::elementDescription bombDesc;
			bombDesc.name = "bomb";
			bombDesc.description = "Fun Fact: Bombs are not allowed in airports";
			bombDesc.category = game::categories::destruction;
			bombDesc.hidden = false;
			descriptionList.push_back(bombDesc);

			game::elementDefinition bomb;
			bomb.init = emptyInit;
			bomb.update = bombUpdate;
			bomb.reaction = noReaction;
			bomb.color1 = util::RGBA(20, 20, 20, 255);
			bomb.color2 = util::RGBA(40, 40, 40, 255);
			bomb.density = 680000; // Density of bomb (cast iron) is 6800 kg/m^3 (6800 * 100 = 680000)
			bomb.isFluid = false;
			bomb.isFlammable = true;
			bomb.flammability = 0;
			bomb.isConductive = true;
			bomb.placement = game::placement::hit;
			definitionsList.push_back(bomb);
			identification[bombDesc.name] = definitionsList.size() - 1;
		}

		// Element: explosion flash
		{
			game::elementDescription explosionFlashDesc;
			explosionFlashDesc.name = "explosion flash";
			explosionFlashDesc.description = "How and why are your reading this? You wern't supposed to be here >:(";
			explosionFlashDesc.category = game::categories::gas;
			explosionFlashDesc.hidden = true;
			descriptionList.push_back(explosionFlashDesc);

			game::elementDefinition explosionFlash;
			explosionFlash.init = explosionFlashInit;
			explosionFlash.update = explosionFlashUpdate;
			explosionFlash.reaction = noReaction;
			explosionFlash.color1 = util::RGBA(0xff5733ff);
			explosionFlash.color2 = util::RGBA(0xff5733ff);
			explosionFlash.density = 145; // Density of smoke is 1.445 kg/m^3 (1.445 * 100 = 145)
			explosionFlash.isFluid = true;
			explosionFlash.isFlammable = false;
			explosionFlash.flammability = 0;
			explosionFlash.isConductive = false;
			explosionFlash.placement = game::placement::hit;
			definitionsList.push_back(explosionFlash);
			identification[explosionFlashDesc.name] = definitionsList.size() - 1;
		}

		// Element: rgb block
		{
			game::elementDescription rgbBlockDesc;
			rgbBlockDesc.name = "rgb block";
			rgbBlockDesc.description = "just a regular rgb block. It does what those funny computers do";
			rgbBlockDesc.category = game::categories::misc;
			rgbBlockDesc.hidden = false;
			descriptionList.push_back(rgbBlockDesc);

			game::elementDefinition rgbBlock;
			rgbBlock.init = emptyInit;
			rgbBlock.update = rgbBlockUpdate;
			rgbBlock.reaction = noReaction;
			rgbBlock.color1 = util::RGBA(0xff0000ff);
			rgbBlock.color2 = util::RGBA(0xffff00ff);
			rgbBlock.density = 240000; // same as concrete
			rgbBlock.isFluid = false;
			rgbBlock.isFlammable = false;
			rgbBlock.flammability = 0;
			rgbBlock.isConductive = false;
			rgbBlock.placement = game::placement::hit;
			definitionsList.push_back(rgbBlock);
			identification[rgbBlockDesc.name] = definitionsList.size() - 1;
		}

		// Element: wet concrete
		{
			game::elementDescription wetConcreteDesc;
			wetConcreteDesc.name = "wet concrete";
			wetConcreteDesc.description = "If you stick your feet in here, they probably won't come out.";
			wetConcreteDesc.category = game::categories::liquid;
			wetConcreteDesc.hidden = false;
			descriptionList.push_back(wetConcreteDesc);

			game::elementDefinition wetConcrete;
			wetConcrete.init = wetConcreteInit;
			wetConcrete.update = wetConcreteUpdate;
			wetConcrete.reaction = noReaction;
			wetConcrete.color1 = util::RGBA(0x57595dff);
			wetConcrete.color2 = util::RGBA(0x75787dff);
			wetConcrete.density = 260000; // wet concrete has a density of 2600 kg/m^3
			wetConcrete.isFluid = true;
			wetConcrete.isFlammable = false;
			wetConcrete.flammability = 0;
			wetConcrete.isConductive = false;
			wetConcrete.placement = game::placement::hit;
			definitionsList.push_back(wetConcrete);
			identification[wetConcreteDesc.name] = definitionsList.size() - 1;
		}

		// Element: concrete
		{
			game::elementDescription concreteDesc;
			concreteDesc.name = "concrete";
			concreteDesc.description = "This is some dry concrete, not like that wet stuff";
			concreteDesc.category = game::categories::solid;
			concreteDesc.hidden = false;
			descriptionList.push_back(concreteDesc);

			game::elementDefinition concrete;
			concrete.init = emptyInit;
			concrete.update = emptyUpdate;
			concrete.reaction = noReaction;
			concrete.color1 = util::RGBA(0xbab6b2ff);
			concrete.color2 = util::RGBA(0xd2d1cdff);
			concrete.density = 240000; // same as concrete
			concrete.isFluid = false;
			concrete.isFlammable = false;
			concrete.flammability = 0;
			concrete.isConductive = false;
			concrete.placement = game::placement::hit;
			definitionsList.push_back(concrete);
			identification[concreteDesc.name] = definitionsList.size() - 1;
		}

		// Element: upward wall
		{
			game::elementDescription upwardWallDesc;
			upwardWallDesc.name = "upward wall";
			upwardWallDesc.description = "Creates columns of walls";
			upwardWallDesc.category = game::categories::misc;
			upwardWallDesc.hidden = false;
			descriptionList.push_back(upwardWallDesc);

			game::elementDefinition upwardWall;
			upwardWall.init = upwardWallInit;
			upwardWall.update = upwardWallUpdate;
			upwardWall.reaction = noReaction;
			upwardWall.color1 = util::RGBA(0, 139, 0, 255);
			upwardWall.color2 = util::RGBA(0, 139, 0, 255);
			upwardWall.density = 240000;
			upwardWall.isFluid = false;
			upwardWall.isFlammable = false;
			upwardWall.flammability = 0;
			upwardWall.isConductive = false;
			upwardWall.placement = game::placement::hit;
			definitionsList.push_back(upwardWall);
			identification[upwardWallDesc.name] = definitionsList.size() - 1;
		}

		// Element: biggus bombus
		{
			game::elementDescription biggusBombusDesc;
			biggusBombusDesc.name = "biggus bombus";
			biggusBombusDesc.description = "This is very dangerous! Think of the children :'(";
			biggusBombusDesc.category = game::categories::destruction;
			biggusBombusDesc.hidden = false;
			descriptionList.push_back(biggusBombusDesc);

			game::elementDefinition biggusBombus;
			biggusBombus.init = emptyInit;
			biggusBombus.update = biggusBombusUpdate;
			biggusBombus.reaction = noReaction;
			biggusBombus.color1 = util::RGBA(0x4b5320ff);
			biggusBombus.color2 = util::RGBA(0x6c7735ff);
			biggusBombus.density = 680000; // Density of bomb (cast iron) is 6800 kg/m^3 (6800 * 100 = 680000)
			biggusBombus.isFluid = false;
			biggusBombus.isFlammable = true;
			biggusBombus.flammability = 0;
			biggusBombus.isConductive = true;
			biggusBombus.placement = game::placement::hit;
			definitionsList.push_back(biggusBombus);
			identification[biggusBombusDesc.name] = definitionsList.size() - 1;
		}

		// Element: particle
		{
			game::elementDescription particleDesc;
			particleDesc.name = "particle";
			particleDesc.description = "I don't think your supposed to be seeing this :(";
			particleDesc.category = game::categories::misc;
			particleDesc.hidden = true;
			descriptionList.push_back(particleDesc);

			game::elementDefinition particle;
			particle.init = emptyInit;
			particle.update = emptyUpdate;
			particle.reaction = noReaction;
			particle.color1 = util::RGBA(0xffffffff);
			particle.color2 = util::RGBA(0xffffffff);
			particle.density = 123; // Same density as air because it doesnt matter for projectile motion
			particle.isFluid = false;
			particle.isFlammable = false;
			particle.flammability = 0;
			particle.isConductive = false;
			particle.placement = game::placement::player;
			definitionsList.push_back(particle);
			identification[particleDesc.name] = definitionsList.size() - 1;
		}

		// Element: world deleter
		//{
		//	game::elementDescription worldDeleterDesc;
		//	worldDeleterDesc.name = "world deleter";
		//	worldDeleterDesc.description = "Imagine building something cool and accidently using this element smh";
		//	worldDeleterDesc.category = game::categories::misc;
		//	worldDeleterDesc.hidden = false;
		//	descriptionList.push_back(worldDeleterDesc);
		//
		//	game::elementDefinition worldDeleter;
		//	worldDeleter.init = worldDeleterInit;
		//	worldDeleter.update = worldDeleterUpdate;
		//	worldDeleter.reaction = noReaction;
		//	worldDeleter.color1 = util::RGBA(255, 0, 0, 255);
		//	worldDeleter.color2 = util::RGBA(255, 0, 0, 255);
		//	worldDeleter.density = 240000; // Lava has a denisty of 3100 kg/m^3 (3100 * 100 = 310000)
		//	worldDeleter.isFluid = false;
		//	worldDeleter.isFlammable = false;
		//	worldDeleter.flammability = 0;
		//	worldDeleter.isConductive = false;
		//	worldDeleter.placement = game::placement::hit;
		//	definitionsList.push_back(worldDeleter);
		//	identification[worldDeleterDesc.name] = definitionsList.size() - 1;
		//}

		// Call this at the end
		definitions = definitionsList.data();
	}
}
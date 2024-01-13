#pragma once
#include <chrono>

namespace simulation
{

#pragma region Debug

#if DEBUG_CHUNKS == true
	// Verticies for Debug
	static const float debugVertices[] =
	{
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.0f, -1.0f,

		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, -1.0f,
		0.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 1.0f, -1.0f,
		1.0f, 0.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
	};

	// Buffers for Debug
	unsigned int debugVAO;
	unsigned int debugVBO;

	// Debug Shader Program
	unsigned int debugProgram;

	// Matrices for debug
	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 scale;
	glm::vec3 position;


	// Debug Shader Uniform Addresses
	int uniformProjection;
	int uniformView;
	int uniformScale;
	int uniformPosition;

	void InitChunkDebug()
	{
		// Creating vertex buffer object
		glGenBuffers(1, &debugVBO);

		// Creating vertex array object
		glGenVertexArrays(1, &debugVAO);

		// Binding vertex array buffer
		glBindVertexArray(debugVAO);

		// Binding vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(debugVertices), debugVertices, GL_STATIC_DRAW);

		// Set Vertex Atribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Create debug program
		debugProgram = shader::createShader("assets/shaders/debug.vert", "assets/shaders/debug.frag");

		// Getting uniform locations
		uniformProjection = glGetUniformLocation(debugProgram, "projection");
		uniformView = glGetUniformLocation(debugProgram, "view");
		uniformScale = glGetUniformLocation(debugProgram, "scale");
		uniformPosition = glGetUniformLocation(debugProgram, "position");

		// Initialize projection and view matrices
		float fov = 2 * atan( tan(glm::radians(90.0f) * 0.5f) / ((float)game::width / (float)game::height));
		projection = glm::perspective(fov, (float)game::width / (float)game::height, 0.1f, 1000.0f);
		view = glm::lookAt(game::positionVec, game::positionVec + game::front, glm::vec3(0.0f, 1.0f, 0.0f));

		// Send matrices to shader
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

		scale = glm::vec3(1.0f, 1.0f, 1.0f);
		position = glm::vec3(0.0f, 0.0f, 0.0f);

		glUniform3fv(uniformScale, 1, glm::value_ptr(scale));
		glUniform3fv(uniformPosition, 1, glm::value_ptr(position));

		//glUniform3fv(simulation::uniformProjection, 1, glm::value_ptr(game::positionVec));
	}

	void DrawChunkDebug(glm::vec3 pos, glm::vec3 length)
	{
		shader::useShader(debugProgram);

		scale = length;
		pos.z = -pos.z;
		position = pos;

		glUniform3fv(uniformScale, 1, glm::value_ptr(scale));
		glUniform3fv(uniformPosition, 1, glm::value_ptr(position));

		glBindVertexArray(debugVAO);
		glDrawArrays(GL_LINES, 0, sizeof(debugVertices) / 4);
		// glBindVertexArray(0);
	}

	void DisposeChunkDebug()
	{
		// Delete VAO and VBO
		glDeleteVertexArrays(1, &debugVAO);
		glDeleteBuffers(1, &debugVBO);

		// Delete raytracer shader program
		shader::deleteShader(debugProgram);
	}
#endif

#pragma endregion

#pragma region  rendering

	// Verticies for Raytracing
	static const float vertices[] =
	{
	   -2.0f, -1.0f, 0.0f,
	    2.0f, -1.0f, 0.0f,
	    0.0f,  3.0f, 0.0f,
	};

	// Buffers for Raytracing
	unsigned int VAO;
	unsigned int VBO;
	unsigned int SSBO;

	// Raytracing Shader Program
	unsigned int raytraceProgram;
	
	// Raytacing Shader Uniform Addresses
	int uniformTime;
	int uniformResolution;
	int uniformPlayerPosition;

	/*int uniformComputeResolution;
	int uniformComputePixelSize;*/

	int uniformFront;
	int uniformRight;
	int uniformUp;

	int uniformBounds;
	int uniformChunkExp;
	int uniformChunkBounds;
	int uniformChunkLength;
	int uniformChunkVoxelNum;

	//// Compute Prediction Buffer
	//unsigned int predictionSSBO;=
	//unsigned int predictionProgram;
	//unsigned int pixelSizeExp = 2; // Exponent (2 ^ pixelSizeExp)
	//unsigned int computeWidth;
	//unsigned int computeHeight;
	//// Prediction Buffer Uniform Addresses
	//int predictionTime;
	//int predictionPlayerPosition;
	//	
	//int predictionFront;
	//int predictionRight;
	//int predictionUp;
	//	
	//int predictionBounds;
	//int predictionChunkExp;
	//int predictionChunkBounds;
	//int predictionChunkLength;
	//int predictionChunkVoxelNum;

	// Initialize the raytracer
	void InitRaytrace()
	{
		#pragma region  raytracer

		// Creating vertex buffer object
		glGenBuffers(1, &VBO);

		// Creating vertex array object
		glGenVertexArrays(1, &VAO);

		// Binding vertex array buffer
		glBindVertexArray(VAO);

		// Binding vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Set Vertex Atribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// Unbind
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Create raytrace program
		raytraceProgram = shader::createShader("assets/shaders/raytrace.vert", "assets/shaders/raytrace.frag");

		// Initialize raytrace program uniforms
		uniformTime = glGetUniformLocation(raytraceProgram, "iTime");
		uniformResolution = glGetUniformLocation(raytraceProgram, "iResolution");
		uniformPlayerPosition = glGetUniformLocation(raytraceProgram, "position");

		/*uniformComputeResolution = glGetUniformLocation(raytraceProgram, "iComputeResolution");
		uniformComputePixelSize = glGetUniformLocation(raytraceProgram, "computePixelSize");*/

		uniformFront = glGetUniformLocation(raytraceProgram, "front");
		uniformRight = glGetUniformLocation(raytraceProgram, "right");
		uniformUp = glGetUniformLocation(raytraceProgram, "up");

		uniformBounds = glGetUniformLocation(raytraceProgram, "bounds");
		uniformChunkExp = glGetUniformLocation(raytraceProgram, "chunkExp");
		uniformChunkBounds = glGetUniformLocation(raytraceProgram, "chunkBounds");
		uniformChunkLength = glGetUniformLocation(raytraceProgram, "chunkLength");
		uniformChunkVoxelNum = glGetUniformLocation(raytraceProgram, "chunkVoxelNum");

		// Setting constant uniforms
		glUniform1ui(uniformBounds, game::bounds);
		glUniform1ui(uniformChunkExp, game::chunkExp);
		glUniform1ui(uniformChunkBounds, game::chunkBounds);
		glUniform1ui(uniformChunkLength, game::chunkLength);
		glUniform1ui(uniformChunkVoxelNum, game::chunkVoxelNum);

		// Setting reselution uniform
		glUniform2f(uniformResolution, game::width, game::height);

		// Create Shader source buffer object
		glGenBuffers(1, &SSBO);
		// Binding Shader source buffer object
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		// Initialize SSBO with voxel data
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * game::voxelNum, game::voxData, GL_DYNAMIC_DRAW);

		// This doesnt add performance. To try take out glBufferData and change opengl minor version
		//glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * game::voxelNum, game::voxData, GL_DYNAMIC_STORAGE_BIT);

		// Bind SSBO to shader location = 3
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SSBO);

		#pragma endregion

		// Prediction buffer
		/*#pragma region predictionBuffer

		computeWidth = (game::currentWidth + (1 << pixelSizeExp) - 1) >> pixelSizeExp;
		computeHeight = (game::currentHeight + (1 << pixelSizeExp) - 1) >> pixelSizeExp;

		// Setting reselution uniform and compute pixel size
		glUniform2f(uniformComputeResolution, computeWidth, computeHeight);
		glUniform1ui(uniformComputePixelSize, pixelSizeExp);

		// Creating prediciton program
		predictionProgram = shader::createComputeShader("assets/shaders/test.comp");

		// Initialize prediction program uniforms
		predictionTime = glGetUniformLocation(predictionProgram, "iTime");
		predictionPlayerPosition = glGetUniformLocation(predictionProgram, "position");
		
		predictionFront = glGetUniformLocation(predictionProgram, "front");
		predictionRight = glGetUniformLocation(predictionProgram, "right");
		predictionUp = glGetUniformLocation(predictionProgram, "up");
		
		predictionBounds = glGetUniformLocation(predictionProgram, "bounds");
		predictionChunkExp = glGetUniformLocation(predictionProgram, "chunkExp");
		predictionChunkBounds = glGetUniformLocation(predictionProgram, "chunkBounds");
		predictionChunkLength = glGetUniformLocation(predictionProgram, "chunkLength");
		predictionChunkVoxelNum = glGetUniformLocation(predictionProgram, "chunkVoxelNum");

		// Setting constant uniforms
		glUniform1ui(predictionBounds, game::bounds);
		glUniform1ui(predictionChunkExp, game::chunkExp);
		glUniform1ui(predictionChunkBounds, game::chunkBounds);
		glUniform1ui(predictionChunkLength, game::chunkLength);
		glUniform1ui(predictionChunkVoxelNum, game::chunkVoxelNum);

		// Create Shader source buffer object
		glGenBuffers(1, &predictionSSBO);
		// Binding Shader source buffer object
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, predictionSSBO);
		// Initialize SSBO with NULL because dat will be updating in shader
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * computeWidth * computeHeight, NULL, GL_DYNAMIC_DRAW);

		// Bind SSBO to shader location = 1
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, predictionSSBO);
		#pragma endregion*/
	}

	// Render the raytracer (Call every frame)
	void RenderRaytrace()
	{
		//shader::useShader(predictionProgram);
		//glDispatchCompute(computeWidth, computeHeight, 1);
		////glMemoryBarrier(GL_ALL_BARRIER_BITS);

		if (VAO == 0)
		{
			std::cout << "ERROR : VAO is equal to 0" << std::endl;
		}

		shader::useShader(raytraceProgram);

		glUniform1f(uniformTime, game::currentTime);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// glBindVertexArray(0);

#if DEBUG_CHUNKS == true
		// Loop over every chunk to reset the clock for only active chunks
		for (game::index3d chunk(game::chunkLength); !chunk.finished(); chunk++)
		{
			if (game::chunkData[chunk.index].updateCurrentFrame == true)
			{
				game::chunk& chnk = game::chunkData[chunk.index];
				
				if (chnk.currentMaxX - chnk.currentMinX < 0) continue;

				simulation::DrawChunkDebug(
					glm::vec3(chunk.x * game::chunkBounds + chnk.currentMinX, chunk.y * game::chunkBounds + chnk.currentMinY, chunk.z * game::chunkBounds + chnk.currentMinZ), 
					glm::vec3(chnk.currentMaxX - chnk.currentMinX, chnk.currentMaxY - chnk.currentMinY, chnk.currentMaxZ - chnk.currentMinZ));
			}

			/*simulation::DrawChunkDebug(
				glm::vec3(chunk.x * game::chunkBounds, chunk.y * game::chunkBounds, chunk.z * game::chunkBounds),
				glm::vec3(game::chunkBounds, game::chunkBounds, game::chunkBounds));*/
		}

		// Debug particles
		for (int i = 0; i < game::particleCount; i++)
		{
			simulation::DrawChunkDebug(
				glm::vec3(game::particleData[i].posX / 32.0, 
					game::particleData[i].posY / 32.0, game::particleData[i].posZ / 32.0),
				glm::vec3(0.05));
		}


#endif
	}

	// Dispose the raytracer
	void DisposeRaytrace()
	{
		// Delete VAO and VBO
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		// Delete raytracer shader program
		shader::deleteShader(raytraceProgram);

		// Unbind and delete SSBO
		glDeleteBuffers(1, &SSBO);

		//// Delete Prediction
		//shader::deleteShader(predictionProgram);
		//// Delete Predicition SSBO
		//// Unbind and delete SSBO
		//glDeleteBuffers(1, &predictionSSBO);
	}

#pragma endregion

#pragma region simulating

	int brushSize = 1;

	// If Change then do not draw line between current and previous
	bool heldDown = false;
	bool leftBounds = false;
	float leftMouseClickTime = 0.0f;
	float rightMouseClickTime = 0.0f;

	

	glm::ivec3 current;
	glm::ivec3 previous;

	// Cast ray and place elements
	void RayPlaceElement(glm::vec3 rayPos, glm::vec3 rayDir)
	{
		int maxRaySteps = 512;

		#pragma region Bounding Box

		glm::vec3 boxMin = glm::vec3(0.0f);
		glm::vec3 boxMax = glm::vec3(game::bounds);

		glm::vec3 t1 = (boxMin - rayPos) / rayDir;
		glm::vec3 t2 = (boxMax - rayPos) / rayDir;

		glm::vec3 tminV = glm::min(t1, t2);
		float tmin = std::max(std::max(tminV.x, tminV.y), tminV.z);
		glm::vec3 tmaxV = glm::max(t1, t2);
		float tmax = std::min(std::min(tmaxV.x, tmaxV.y), tmaxV.z);

		if (tmin > 0.0)
		{
			rayPos = rayPos + rayDir * (tmin - 0.001f);
		}

		#pragma endregion

		#pragma region Trace Ray

		glm::ivec3 mapPos = glm::ivec3(floor(rayPos));
		glm::ivec3 rayStep = glm::ivec3(sign(rayDir));
		glm::vec3 tDelta = 1.0f / rayDir * glm::vec3(rayStep);
		glm::vec3 tMax = (glm::vec3(rayStep) * ((glm::vec3(mapPos) - rayPos) + 0.5f) + 0.5f) * tDelta;

		glm::bvec3 mask = glm::bvec3(false);

		glm::ivec3 prevMapPos(0);

		for (int i = 0; i < maxRaySteps; i++)
		{
			prevMapPos = mapPos;

			mask.x = (tMax.x <= tMax.y) && (tMax.x <= tMax.z);
			mask.y = (tMax.y <= tMax.x) && (tMax.y <= tMax.z);
			mask.z = (tMax.z <= tMax.x) && (tMax.z <= tMax.y);

			tMax += (glm::vec3)mask * tDelta;
			mapPos += (glm::ivec3)mask * rayStep;

			if (elements::VoxelIsInside(mapPos.x, mapPos.y, mapPos.z))
			{
				// If ray hits voxel that isnt air, set element at the previous voxel
				if (elements::getElement(mapPos.x, mapPos.y, mapPos.z).elementID != 0)
				{
					// Only do this if the previous voxel is in the map
					if (elements::VoxelIsInside(prevMapPos.x, prevMapPos.y, prevMapPos.z))
					{
						current = prevMapPos;
						leftBounds = false;
						break;
					}
				}
			}
			else if (elements::VoxelIsInside(prevMapPos.x, prevMapPos.y, prevMapPos.z))
			{
				current = prevMapPos;
				leftBounds = false;
				break;
			}
		}

		if (tmax < tmin || tmax < 0.0)
		{
			leftBounds = true;
		}

		#pragma endregion
	}

	// Place Element using the elements placement type
	void PlaceElement(int elementID, glm::vec3 rayPos, glm::vec3 rayDir)
	{
		if (elements::definitions[elementID].placement == game::placement::hit)
		{
			if (leftBounds)
			{
				// previous = current;
				heldDown = false;
			}

			RayPlaceElement(rayPos, rayDir);

			if (heldDown == true)
			{
				int x1 = current.x, y1 = current.y, z1 = current.z;
				int x2 = previous.x, y2 = previous.y, z2 = previous.z;

				int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
				int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
				int dz = abs(z2 - z1), sz = z1 < z2 ? 1 : -1;
				int dm = std::max(std::max(dx, dy), dz), i = dm;
				x2 = y2 = z2 = dm / 2;

				while (true)
				{
					for (game::index3d i(1 + brushSize * 2); !i.finished(); i++)
					{
						int x = x1 + i.x - brushSize, y = y1 + i.y - brushSize, z = z1 + i.z - brushSize;

						if (elements::VoxelIsInside(x, y, z) && elements::getElement(x, y, z).elementID == 0)
						{
							elements::setElement(x, y, z, game::elementSelect);
						}
					}

					if (i-- == 0) break;
					x2 -= dx; if (x2 < 0) { x2 += dm; x1 += sx; }
					y2 -= dy; if (y2 < 0) { y2 += dm; y1 += sy; }
					z2 -= dz; if (z2 < 0) { z2 += dm; z1 += sz; }
				}

				previous = current;
			}
			else if (!leftBounds)
			{
				for (game::index3d i(1 + brushSize * 2); !i.finished(); i++)
				{
					int x = current.x + i.x - brushSize, y = current.y + i.y - brushSize, z = current.z + i.z - brushSize;

					if (elements::VoxelIsInside(x, y, z) && elements::getElement(x, y, z).elementID == 0)
					{
						elements::setElement(x, y, z, game::elementSelect);
					}
				}

				previous = current;
			}
		}
		else if (elements::definitions[elementID].placement == game::placement::player)
		{
			glm::ivec3 p = rayPos + rayDir * 10.0f;

			// This works, just comment it in
			for (int i = p.z - brushSize; i <= p.z + brushSize; i++)
			{
				for (int j = p.y - brushSize; j <= p.y + brushSize; j++)
				{
					for (int k = p.x - brushSize; k <= p.x + brushSize; k++)
					{
						if (elements::VoxelIsInside(k, j, i) && elements::getElement(k, j, i).elementID == 0) // && elements::getElement(k, j, i).elementID == 0)
						{
							elements::setElement(k, j, i, game::elementSelect);
						}
					}
				}
			}

			//if (elements::VoxelIsInside(p.x, p.y, p.z)) // && elements::getElement(k, j, i).elementID == 0)
			//{
			//	elements::setElement(p.x, p.y, p.z, game::elementSelect);
			//}
		}
	}

	// Cast ray and remove elements
	void RayRemoveElement(glm::vec3 rayPos, glm::vec3 rayDir)
	{
		int maxRaySteps = 512;

#pragma region Bounding Box

		glm::vec3 boxMin = glm::vec3(0.0f);
		glm::vec3 boxMax = glm::vec3(game::bounds);

		glm::vec3 t1 = (boxMin - rayPos) / rayDir;
		glm::vec3 t2 = (boxMax - rayPos) / rayDir;

		glm::vec3 tminV = glm::min(t1, t2);
		float tmin = std::max(std::max(tminV.x, tminV.y), tminV.z);
		glm::vec3 tmaxV = glm::max(t1, t2);
		float tmax = std::min(std::min(tmaxV.x, tmaxV.y), tmaxV.z);

		if (tmin > 0.0f)
		{
			rayPos = rayPos + rayDir * (tmin - 0.001f);
		}

#pragma endregion

#pragma region Trace Ray

		glm::ivec3 mapPos = glm::ivec3(floor(rayPos));
		glm::ivec3 rayStep = glm::ivec3(sign(rayDir));
		glm::vec3 tDelta = 1.0f / rayDir * glm::vec3(rayStep);
		glm::vec3 tMax = (glm::vec3(rayStep) * ((glm::vec3(mapPos) - rayPos) + 0.5f) + 0.5f) * tDelta;

		glm::bvec3 mask = glm::bvec3(false);

		for (int i = 0; i < maxRaySteps; i++)
		{
			mask.x = (tMax.x <= tMax.y) && (tMax.x <= tMax.z);
			mask.y = (tMax.y <= tMax.x) && (tMax.y <= tMax.z);
			mask.z = (tMax.z <= tMax.x) && (tMax.z <= tMax.y);

			tMax += (glm::vec3)mask * tDelta;
			mapPos += (glm::ivec3)mask * rayStep;

			if (elements::VoxelIsInside(mapPos.x, mapPos.y, mapPos.z))
			{
				if (elements::getElement(mapPos.x, mapPos.y, mapPos.z).elementID != 0)
				{
					current = mapPos;
					break;
				}
			}
		}

#pragma endregion

		for (game::index3d i(1 + brushSize * 2); !i.finished(); i++)
		{
			int x = current.x + i.x - brushSize, y = current.y + i.y - brushSize, z = current.z + i.z - brushSize;

			if (elements::VoxelIsInside(x, y, z) && elements::getElement(x, y, z).elementID != 0)
			{
				elements::setElement(x, y, z, 0);
			}
		}

		previous = current;
	}

	void DiscreteCircle(int x0, int y0, int z0, int r, unsigned int col)
	{
		int x = 0, y = r;
		int err = r * r;

		while (y >= x)
		{
			elements::setElement(x0 + x, y0 + y, z0, elements::identification["wall"]);
			game::voxData[game::index3dA(x0 + x, y0 + y, z0)] = col;
			elements::setElement(x0 + x, y0 - y, z0, elements::identification["wall"]);
			game::voxData[game::index3dA(x0 + x, y0 - y, z0)] = col;
			elements::setElement(x0 - x, y0 + y, z0, elements::identification["wall"]);
			game::voxData[game::index3dA(x0 - x, y0 + y, z0)] = col;
			elements::setElement(x0 - x, y0 - y, z0, elements::identification["wall"]);
			game::voxData[game::index3dA(x0 - x, y0 - y, z0)] = col;

			elements::setElement(x0 + y, y0 + x, z0, elements::identification["wall"]);
			game::voxData[game::index3dA(x0 + y, y0 + x, z0)] = col;
			elements::setElement(x0 + y, y0 - x, z0, elements::identification["wall"]);
			game::voxData[game::index3dA(x0 + y, y0 - x, z0)] = col;
			elements::setElement(x0 - y, y0 + x, z0, elements::identification["wall"]);
			game::voxData[game::index3dA(x0 - y, y0 + x, z0)] = col;
			elements::setElement(x0 - y, y0 - x, z0, elements::identification["wall"]);
			game::voxData[game::index3dA(x0 - y, y0 - x, z0)] = col;

			if (err <= r * r + r - 2 * x - 1)
			{
				err += 2 * x + 1;
				x++;
			}
			else if (err >= r * r + r + 2 * (y - r))
			{
				err += 1 - 2 * y;
				y--;
			}
			else
			{
				err += 2 * (x - y + 1);
				x++;
				y--;
			}
		}
	}

	void setColoredWall(unsigned int x, unsigned int y, unsigned int z, unsigned int col)
	{
		elements::setElement(x, y, z, elements::identification["wall"]);
		game::voxData[game::index3dA(x, y, z)] = col;
	}

	/*
			setColoredWall(x0 + x, y0 + y, z0 + z, col);
			setColoredWall(x0 + x, y0 + z, z0 + y, col);
			setColoredWall(x0 + y, y0 + z, z0 + x, col);
			setColoredWall(x0 + y, y0 + x, z0 + z, col);
			setColoredWall(x0 + z, y0 + y, z0 + x, col);
			setColoredWall(x0 + z, y0 + x, z0 + y, col);

			setColoredWall(x0 - x, y0 + y, z0 + z, col);
			setColoredWall(x0 - x, y0 + z, z0 + y, col);
			setColoredWall(x0 - y, y0 + z, z0 + x, col);
			setColoredWall(x0 - y, y0 + x, z0 + z, col);
			setColoredWall(x0 - z, y0 + y, z0 + x, col);
			setColoredWall(x0 - z, y0 + x, z0 + y, col);

			setColoredWall(x0 + x, y0 - y, z0 + z, col);
			setColoredWall(x0 + x, y0 - z, z0 + y, col);
			setColoredWall(x0 + y, y0 - z, z0 + x, col);
			setColoredWall(x0 + y, y0 - x, z0 + z, col);
			setColoredWall(x0 + z, y0 - y, z0 + x, col);
			setColoredWall(x0 + z, y0 - x, z0 + y, col);

			setColoredWall(x0 - x, y0 - y, z0 + z, col);
			setColoredWall(x0 - x, y0 - z, z0 + y, col);
			setColoredWall(x0 - y, y0 - z, z0 + x, col);
			setColoredWall(x0 - y, y0 - x, z0 + z, col);
			setColoredWall(x0 - z, y0 - y, z0 + x, col);
			setColoredWall(x0 - z, y0 - x, z0 + y, col);

			setColoredWall(x0 + x, y0 + y, z0 - z, col);
			setColoredWall(x0 + x, y0 + z, z0 - y, col);
			setColoredWall(x0 + y, y0 + z, z0 - x, col);
			setColoredWall(x0 + y, y0 + x, z0 - z, col);
			setColoredWall(x0 + z, y0 + y, z0 - x, col);
			setColoredWall(x0 + z, y0 + x, z0 - y, col);

			setColoredWall(x0 - x, y0 + y, z0 - z, col);
			setColoredWall(x0 - x, y0 + z, z0 - y, col);
			setColoredWall(x0 - y, y0 + z, z0 - x, col);
			setColoredWall(x0 - y, y0 + x, z0 - z, col);
			setColoredWall(x0 - z, y0 + y, z0 - x, col);
			setColoredWall(x0 - z, y0 + x, z0 - y, col);

			setColoredWall(x0 + x, y0 - y, z0 - z, col);
			setColoredWall(x0 + x, y0 - z, z0 - y, col);
			setColoredWall(x0 + y, y0 - z, z0 - x, col);
			setColoredWall(x0 + y, y0 - x, z0 - z, col);
			setColoredWall(x0 + z, y0 - y, z0 - x, col);
			setColoredWall(x0 + z, y0 - x, z0 - y, col);

			setColoredWall(x0 - x, y0 - y, z0 - z, col);
			setColoredWall(x0 - x, y0 - z, z0 - y, col);
			setColoredWall(x0 - y, y0 - z, z0 - x, col);
			setColoredWall(x0 - y, y0 - x, z0 - z, col);
			setColoredWall(x0 - z, y0 - y, z0 - x, col);
			setColoredWall(x0 - z, y0 - x, z0 - y, col);*/

	void DiscreteSphereCircle(int x0, int y0, int z0, int z, int r, int originalR, int min, int max, unsigned int col)
	{
		int x = 0, y = r;
		int err = r;
		int localMin = r * r + originalR - max;
		int localMax = r * r + originalR - min;


		while (y >= x)
		{
			if (err >= localMin && err < localMax)
			{
				setColoredWall(x0 + x, y0 + y, z0 + z, col);
				setColoredWall(x0 + y, y0 + x, z0 + z, col);
				setColoredWall(x0 - x, y0 + y, z0 + z, col);
				setColoredWall(x0 - y, y0 + x, z0 + z, col);
				setColoredWall(x0 + x, y0 - y, z0 + z, col);
				setColoredWall(x0 + y, y0 - x, z0 + z, col);
				setColoredWall(x0 - x, y0 - y, z0 + z, col);
				setColoredWall(x0 - y, y0 - x, z0 + z, col);

				setColoredWall(x0 + x, y0 + y, z0 - z, col);
				setColoredWall(x0 + y, y0 + x, z0 - z, col);
				setColoredWall(x0 - x, y0 + y, z0 - z, col);
				setColoredWall(x0 - y, y0 + x, z0 - z, col);
				setColoredWall(x0 + x, y0 - y, z0 - z, col);
				setColoredWall(x0 + y, y0 - x, z0 - z, col);
				setColoredWall(x0 - x, y0 - y, z0 - z, col);
				setColoredWall(x0 - y, y0 - x, z0 - z, col);
			}
			


			

			if (err > 2 * x)
			{
				err -= 2 * x + 1;
				x++;
			}
			else if (err <= 2 * (originalR - y))
			{
				err += 2 * y - 1;
				y--;
			}
			else
			{
				err += 2 * (y - x - 1);
				x++;
				y--;
			}
		}
	}

	void DiscreteSphere(int x0, int y0, int z0, int r, unsigned int col)
	{
		int z = 0, y = r;
		int err = r * r;

		while (y >= 0)
		{
			int cstmax = r * r + r - z * z;
			int cstmin = cstmax - 2 * r;
			DiscreteSphereCircle(x0, y0, z0, z, y, r, cstmin, cstmax, col);

			if (err <= r * r + r - 2 * z - 1)
			{
				err += 2 * z + 1;
				z++;
			}
			else if (err >= r * r + r + 2 * (y - r))
			{
				err += 1 - 2 * y;
				y--;
			}
			else
			{
				err += 2 * (z - y + 1);
				z++;
				y--;
			}
		}
	}

	// This is not concentric
	void DiscreteSphere2(int x0, int y0, int z0, int r, unsigned int col)
	{

		int i = 0, j = 0;
		int k = r, k0 = r;
		int s = 0, s0 = 0;
		int v = r - 1, v0 = r - 1;
		int l = 2 * v0, l0 = 2 * v0;

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
					setColoredWall(x0 + i, y0 + j, z0 + k, col);
					setColoredWall(x0 + i, y0 + k, z0 + j, col);
					setColoredWall(x0 + j, y0 + k, z0 + i, col);
					setColoredWall(x0 + j, y0 + i, z0 + k, col);
					setColoredWall(x0 + k, y0 + j, z0 + i, col);
					setColoredWall(x0 + k, y0 + i, z0 + j, col);
														   
					setColoredWall(x0 - i, y0 + j, z0 + k, col);
					setColoredWall(x0 - i, y0 + k, z0 + j, col);
					setColoredWall(x0 - j, y0 + k, z0 + i, col);
					setColoredWall(x0 - j, y0 + i, z0 + k, col);
					setColoredWall(x0 - k, y0 + j, z0 + i, col);
					setColoredWall(x0 - k, y0 + i, z0 + j, col);
														   
					setColoredWall(x0 + i, y0 - j, z0 + k, col);
					setColoredWall(x0 + i, y0 - k, z0 + j, col);
					setColoredWall(x0 + j, y0 - k, z0 + i, col);
					setColoredWall(x0 + j, y0 - i, z0 + k, col);
					setColoredWall(x0 + k, y0 - j, z0 + i, col);
					setColoredWall(x0 + k, y0 - i, z0 + j, col);
														   
					setColoredWall(x0 - i, y0 - j, z0 + k, col);
					setColoredWall(x0 - i, y0 - k, z0 + j, col);
					setColoredWall(x0 - j, y0 - k, z0 + i, col);
					setColoredWall(x0 - j, y0 - i, z0 + k, col);
					setColoredWall(x0 - k, y0 - j, z0 + i, col);
					setColoredWall(x0 - k, y0 - i, z0 + j, col);
														   
					setColoredWall(x0 + i, y0 + j, z0 - k, col);
					setColoredWall(x0 + i, y0 + k, z0 - j, col);
					setColoredWall(x0 + j, y0 + k, z0 - i, col);
					setColoredWall(x0 + j, y0 + i, z0 - k, col);
					setColoredWall(x0 + k, y0 + j, z0 - i, col);
					setColoredWall(x0 + k, y0 + i, z0 - j, col);
														   
					setColoredWall(x0 - i, y0 + j, z0 - k, col);
					setColoredWall(x0 - i, y0 + k, z0 - j, col);
					setColoredWall(x0 - j, y0 + k, z0 - i, col);
					setColoredWall(x0 - j, y0 + i, z0 - k, col);
					setColoredWall(x0 - k, y0 + j, z0 - i, col);
					setColoredWall(x0 - k, y0 + i, z0 - j, col);
														   
					setColoredWall(x0 + i, y0 - j, z0 - k, col);
					setColoredWall(x0 + i, y0 - k, z0 - j, col);
					setColoredWall(x0 + j, y0 - k, z0 - i, col);
					setColoredWall(x0 + j, y0 - i, z0 - k, col);
					setColoredWall(x0 + k, y0 - j, z0 - i, col);
					setColoredWall(x0 + k, y0 - i, z0 - j, col);
														   
					setColoredWall(x0 - i, y0 - j, z0 - k, col);
					setColoredWall(x0 - i, y0 - k, z0 - j, col);
					setColoredWall(x0 - j, y0 - k, z0 - i, col);
					setColoredWall(x0 - j, y0 - i, z0 - k, col);
					setColoredWall(x0 - k, y0 - j, z0 - i, col);
					setColoredWall(x0 - k, y0 - i, z0 - j, col);

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
	}

	int giveMin(unsigned int z, unsigned int r)
	{
		int rmin = r;
		int arcmin = r * r - r;
		int mins = arcmin - z * z;
		if (mins < 0) rmin = 0;
		else
		{
			while (mins < arcmin)
			{
				rmin--;
				arcmin -= 2 * rmin;
			}
		}

		return rmin;
	}

	int giveMax(unsigned int z, unsigned int r)
	{
		int rmax = r;
		int arcmax = r * r - r;
		int maxs = arcmax + 2 * r - z * z;
		
		while (maxs < arcmax)
		{
			rmax--;
			arcmax -= 2 * rmax;

		}

		return rmax;
	}

	void circleSphere(int x0, int y0, int z0, int r, int rCurrent, int cstmin, int cstmax, int z, int col)
	{
		int x = 0;
		int y = rCurrent;
		int delta = rCurrent;

		int cstLocalMin = rCurrent * rCurrent + r - cstmax;
		int cstLocalMax = rCurrent * rCurrent + r - cstmin;

		while (y >= x)
		{
			if (delta >= cstLocalMin && delta < cstLocalMax)
			{
				setColoredWall(x0 + x, y0 + y, z0 + z, col);
				setColoredWall(x0 + x, y0 + z, z0 + y, col);
				setColoredWall(x0 + y, y0 + z, z0 + x, col);
				setColoredWall(x0 + y, y0 + x, z0 + z, col);
				setColoredWall(x0 + z, y0 + y, z0 + x, col);
				setColoredWall(x0 + z, y0 + x, z0 + y, col);

				setColoredWall(x0 - x, y0 + y, z0 + z, col);
				setColoredWall(x0 - x, y0 + z, z0 + y, col);
				setColoredWall(x0 - y, y0 + z, z0 + x, col);
				setColoredWall(x0 - y, y0 + x, z0 + z, col);
				setColoredWall(x0 - z, y0 + y, z0 + x, col);
				setColoredWall(x0 - z, y0 + x, z0 + y, col);

				setColoredWall(x0 + x, y0 - y, z0 + z, col);
				setColoredWall(x0 + x, y0 - z, z0 + y, col);
				setColoredWall(x0 + y, y0 - z, z0 + x, col);
				setColoredWall(x0 + y, y0 - x, z0 + z, col);
				setColoredWall(x0 + z, y0 - y, z0 + x, col);
				setColoredWall(x0 + z, y0 - x, z0 + y, col);

				setColoredWall(x0 - x, y0 - y, z0 + z, col);
				setColoredWall(x0 - x, y0 - z, z0 + y, col);
				setColoredWall(x0 - y, y0 - z, z0 + x, col);
				setColoredWall(x0 - y, y0 - x, z0 + z, col);
				setColoredWall(x0 - z, y0 - y, z0 + x, col);
				setColoredWall(x0 - z, y0 - x, z0 + y, col);

				setColoredWall(x0 + x, y0 + y, z0 - z, col);
				setColoredWall(x0 + x, y0 + z, z0 - y, col);
				setColoredWall(x0 + y, y0 + z, z0 - x, col);
				setColoredWall(x0 + y, y0 + x, z0 - z, col);
				setColoredWall(x0 + z, y0 + y, z0 - x, col);
				setColoredWall(x0 + z, y0 + x, z0 - y, col);

				setColoredWall(x0 - x, y0 + y, z0 - z, col);
				setColoredWall(x0 - x, y0 + z, z0 - y, col);
				setColoredWall(x0 - y, y0 + z, z0 - x, col);
				setColoredWall(x0 - y, y0 + x, z0 - z, col);
				setColoredWall(x0 - z, y0 + y, z0 - x, col);
				setColoredWall(x0 - z, y0 + x, z0 - y, col);

				setColoredWall(x0 + x, y0 - y, z0 - z, col);
				setColoredWall(x0 + x, y0 - z, z0 - y, col);
				setColoredWall(x0 + y, y0 - z, z0 - x, col);
				setColoredWall(x0 + y, y0 - x, z0 - z, col);
				setColoredWall(x0 + z, y0 - y, z0 - x, col);
				setColoredWall(x0 + z, y0 - x, z0 - y, col);

				setColoredWall(x0 - x, y0 - y, z0 - z, col);
				setColoredWall(x0 - x, y0 - z, z0 - y, col);
				setColoredWall(x0 - y, y0 - z, z0 - x, col);
				setColoredWall(x0 - y, y0 - x, z0 - z, col);
				setColoredWall(x0 - z, y0 - y, z0 - x, col);
				setColoredWall(x0 - z, y0 - x, z0 - y, col);
			}

			if (delta > 2 * x)
			{
				delta -= 2 * x + 1;
				x++;
			}
			else if (delta <= 2 * (r - y))
			{
				delta += 2 * y - 1;
				y--;
			}
			else
			{
				delta += 2 * (y - x - 1);
				x++;
				y--;
			}
		}

	}

	void DiscreteSphere3(int x0, int y0, int z0, int r, unsigned int col)
	{
		for (int z = 0; z <= r; z++)
		{
			int rMin = giveMin(z, r);
			int rMax = giveMax(z, r);
			int cstMax = r * r + r - z * z;
			int cstMin = cstMax - 2 * r;

			for (int rCurrent = rMin; rCurrent <= rMax; rCurrent++)
			{
				circleSphere(x0, y0, z0, r, rCurrent, cstMin, cstMax, z, col);
			}
		}
	}


	// Generate the Voxel World
	void GenerateWorld()
	{
		#if RANDOM_GEN

		int sand = elements::identification["sand"];

		for (int z = 0; z < game::bounds; z++)
		{
			for (int x = 0; x < game::bounds; x++)
			{
				int length = 32 * (game::noise.GetNoise((float)x, (float)z) + 1) * 0.5f;

				for (int y = 0; y < length; y++)
				{
					elements::setElement(x, y, z, sand);
				}
			}
		}

		int water = elements::identification["water"];

		for (game::index3d i(game::bounds, 16, game::bounds); !i.finished(); i++)
		{
			if (elements::getElement(i.x, i.y, i.z).elementID == 0 && util::direction() == 1)
			{
				elements::setElement(i.x, i.y, i.z, water);
			}
		}

		#else

		int sand = elements::identification["sand"];

		// Create base
		for (unsigned int i = 0; i < game::bounds; i++)
		{
			for (unsigned int j = 0; j < game::bounds; j++)
			{
				elements::setElement(i, 0, j, elements::identification["wall"]);
			}
		}

		for (int i = 0; i < 128; i++)
		{
			DiscreteCircle(128, 128, 128 + i, 16 + sin(i / 6.0) * 6, util::randXOR());
		}

		// DiscreteSphere(64, 64, 64, 32, util::randXOR()); // This one is broken
		DiscreteSphere3(64, 64, 64, 32, util::randXOR()); // This one is concentric
		// DiscreteSphere2(64, 64, 64, i, util::randXOR()); // This one is not concnetric but very fast

		for (game::index3d i(game::bounds, 32, game::bounds); !i.finished(); i++)
		{
			elements::setElement(i.x, i.y, i.z, elements::identification["wire"]);
		}

		for (game::index3d i(1, 32, game::bounds); !i.finished(); i++)
		{
			elements::setElement(i.x + 128, i.y, i.z, elements::identification["wall"]);
		}

		#endif
	}

	// Initialize the element simulation
	void InitSimulation()
	{
		// Initialize simplex noise
		game::noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

		// Initialize the element definitions
		elements::InitElementDefinitions();

		// Allocate data for elements
		game::elementData = new game::element[game::voxelNum];

		// Initalize voxel color data
		game::voxData = new unsigned int[game::voxelNum];

		// Allocate data for chunks
		game::chunkData = new game::chunk[game::chunkNum];

		// Initializing chunks
		for (game::index3d chunks(game::chunkLength); !chunks.finished(); chunks++)
		{
			game::chunkData[chunks.index].updateNextFrame = false;
			game::chunkData[chunks.index].updateCurrentFrame = false;

			game::chunkData[chunks.index].currentMinX = game::chunkBounds - 1;
			game::chunkData[chunks.index].currentMaxX = 0;
			game::chunkData[chunks.index].currentMinY = game::chunkBounds - 1;
			game::chunkData[chunks.index].currentMaxY = 0;
			game::chunkData[chunks.index].currentMinZ = game::chunkBounds - 1;
			game::chunkData[chunks.index].currentMaxZ = 0;

			game::chunkData[chunks.index].nextMinX = game::chunkBounds - 1; 
			game::chunkData[chunks.index].nextMaxX = 0;
			game::chunkData[chunks.index].nextMinY = game::chunkBounds - 1; 
			game::chunkData[chunks.index].nextMaxY = 0;
			game::chunkData[chunks.index].nextMinZ = game::chunkBounds - 1; 
			game::chunkData[chunks.index].nextMaxZ = 0;
		}

		// Allocate data for particles
		game::particleData = new game::particle[game::maxParticles];

		// Setting all elements to air

		auto t1 = std::chrono::high_resolution_clock::now();
		int temp = 0;
		for (game::index3d i(game::bounds); !i.finished(); i++)
		{
			//temp = game::index3dA(i.x, i.y, i.z);
			elements::setElement(i.x, i.y, i.z, 0);
		}
		auto t2 = std::chrono::high_resolution_clock::now();

		/* Getting number of milliseconds as a double. */
		std::chrono::duration<double, std::nano> ms_double = t2 - t1;
		std::cout << (ms_double.count() / game::voxelNum) << " ns " << temp << "\n";

		// Gen world
		GenerateWorld();
	}

#if DEBUG_CHUNKS
	// Variables to pause and increment the frame when in debug mode
	bool pause = false;
	bool increment = false;
#endif

	// TODO: Fix the explosion to make it feel more natural and less fake

	// Update the element simulation ( + particle sim)
	void UpdateSimulation()
	{
#if DEBUG_CHUNKS
		if (increment || !pause)
		{
			increment = false;
		}
		else return;
#endif
		// Entire Simulation
		if (game::elementUpdateTick > 1.0 / 30.0) // 30 ticks per second
		{
			// Cool particle spiral
			// elements::setElement(game::bounds / 2 - 20, game::bounds / 2 - 20, game::bounds / 2 - 20, elements::identification["sand"]);
			// elements::setParticle(game::bounds / 2 - 20, game::bounds / 2 - 20, game::bounds / 2 - 20, ((util::randXOR() & 255) - 128.0) / 128.0, ((util::randXOR() & 255) - 128.0) / 128.0, ((util::randXOR() & 255) - 128.0) / 128.0);

			#pragma region element simulation

			// Only place elements when not in the element choosing menu
			if (!ui::choosingElement)
			{
				// TODO: Put single cluck functionality in here
				// 
				// If left button is pressed, then place element
				// If right button is pressed, then remove element
				if (glfwGetMouseButton(game::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
				{
					if (leftMouseClickTime == 0.0f || leftMouseClickTime >= 67.0f)
					{
						PlaceElement(game::elementSelect, game::positionVec, game::front);
					}

					heldDown = true;

					leftMouseClickTime += 33.3333333333f;
				}
				else if (glfwGetMouseButton(game::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
				{
					if (rightMouseClickTime == 0.0f || rightMouseClickTime >= 67.0f)
					{
						RayRemoveElement(game::positionVec, game::front);
					}

					heldDown = false;

					rightMouseClickTime += 33.3333333333f;
				}
				else 
				{ 
					heldDown = false; 

					if (leftMouseClickTime != 0.0f) { std::cout << "leftMouseClickTime: " << leftMouseClickTime << " ms" << std::endl; }
					leftMouseClickTime = 0.0f; 
					
					if (rightMouseClickTime != 0.0f) { std::cout << "rightMouseClickTime: " << rightMouseClickTime << " ms" << std::endl; }
					rightMouseClickTime = 0.0f;
				}
			}


			// Checks which chunks should be updated this frame
			for (int i = 0; i < game::chunkNum; i++)
			{
				game::chunkData[i].FrameChange();
			}

			// Loop over every chunk
			for (game::index3d chunk(game::chunkLength); !chunk.finished(); chunk++)
			{
				// If the chunk should be updated this frame
				if (game::chunkData[chunk.index].updateCurrentFrame == true)
				{
					game::chunk& chnk = game::chunkData[chunk.index];

					for (int z = chnk.currentMinZ; z < chnk.currentMaxZ; z++)
					{
						for (int y = chnk.currentMinY; y < chnk.currentMaxY; y++)
						{
							for (int x = chnk.currentMinX; x < chnk.currentMaxX; x++)
							{
								elements::update.chunkX = chunk.x;
								elements::update.chunkY = chunk.y;
								elements::update.chunkZ = chunk.z;

								elements::update.localX = x;
								elements::update.localY = y;
								elements::update.localZ = z;

								unsigned int localIndex = (x + ((y + (z << game::chunkExp)) << game::chunkExp));
								elements::update.index = game::chunkVoxelNum * chunk.index + localIndex;

								if (game::elementData[elements::update.index].clock == 0 && game::elementData[elements::update.index].elementID != 0)
								{
									elements::definitions[game::elementData[elements::update.index].elementID].update((chunk.x << game::chunkExp) + x, (chunk.y << game::chunkExp) + y, (chunk.z << game::chunkExp) + z);
								}
							}
						}
					}
				}
			}

			#pragma endregion

			#pragma region particle simulation
			
			int particle = elements::identification["particle"];

			int index = 0;
			while (index < game::particleCount)
			{
				if (index < 0) std::cout << "THIS IS BAD" << std::endl;

				// Gravity
				game::particleData[index].velY -= 1; // when subtracting by 10 per frame, we are actually subracting 9.375 m/s^2 (aka 10/32 * 30)  

				// Old positions
				int posX0 = game::particleData[index].posX;
				int posY0 = game::particleData[index].posY;
				int posZ0 = game::particleData[index].posZ;

				// New positions
				int posX1 = game::particleData[index].posX + game::particleData[index].velX;
				int posY1 = game::particleData[index].posY + game::particleData[index].velY;
				int posZ1 = game::particleData[index].posZ + game::particleData[index].velZ;

				// Fractional value to whole integer value
				int x0 = posX0 / 32, y0 = posY0 / 32, z0 = posZ0 / 32;
				int x1 = posX1 / 32, y1 = posY1 / 32, z1 = posZ1 / 32;

				// Bresemham line algorithm
				int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
				int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
				int dz = abs(z1 - z0), sz = z0 < z1 ? 1 : -1;
				int dm = std::max(std::max(dx, dy), dz), i = dm;
				x1 = y1 = z1 = dm / 2;

				// Last position
				int xLast = x0, yLast = y0, zLast = z0;

				bool particleDestoyed = false;

				// If there is a particle at the initial position, but the position contains an air element in the element sim, then replace with particle
				if (elements::VoxelIsInside(x0, y0, z0) && elements::getElement(x0, y0, z0).elementID == 0)
				{
					elements::setElement(x0, y0, z0, particle);
					game::voxData[game::index3dA(x0, y0, z0)] = game::particleData[index].color;
				}

				while (true)
				{
					// Set current position as last position;
					xLast = x0, yLast = y0, zLast = z0;

					if (i-- == 0) break;
					x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; }
					y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; }
					z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; }

					if (elements::VoxelIsInside(x0, y0, z0) && elements::getElement(x0, y0, z0).elementID == 0 && elements::getElement(xLast, yLast, zLast).elementID == particle)
					{
						// If first run, then delete particle element if it exists and create it at new location
						// If particle does not exist, then create it and swap between the current and last position
						
						// If color is same, then move to new position
						// Else create new particle at new position
						if (game::voxData[game::currentIndex] == game::particleData[index].color)
						{
							elements::swapElement(game::index3dA(x0, y0, z0), game::index3dA(xLast, yLast, zLast));
							elements::StepChunk2(x0, y0, z0);
						}
						else
						{
							elements::setElement(x0, y0, z0, particle);
							game::voxData[game::index3dA(x0, y0, z0)] = game::particleData[index].color;
						}
					}
					else if (elements::VoxelIsInside(x0, y0, z0) && elements::getElement(x0, y0, z0).elementID == particle)
					{
						// TODO: Make if that only sets (xLast, yLast, zLast) to air if there is only 1 particle inside the voxel
						elements::setElement(xLast, yLast, zLast, 0);
					}
					else // We destroy the particle and place it 
					{
						/*
						if (elements::getElement(xLast, yLast, zLast).elementID != particle)
						{
							std::cout << "something is off : (" << std::endl;
						}*/

						particleDestoyed = true;

						elements::setElement(xLast, yLast, zLast, game::particleData[index].elementID);
						game::voxData[game::index3dA(xLast, yLast, zLast)] = game::particleData[index].color;

						game::particleCount--;

						// This is to make that if the last particle is destroyed, it doesn't look
						// infinitly and break the game
						if (index != game::particleCount)
						{
							game::particleData[index] = game::particleData[game::particleCount];
							index--;
						}
						else
						{
							game::particleData[index] = game::emptyParticle;
						}

						break;
					}
				}

				// If the particle can fully travel the distance, then set its values in particleData
				if (i < 0 && !particleDestoyed)
				{
					game::particleData[index].posX = posX1;
					game::particleData[index].posY = posY1;
					game::particleData[index].posZ = posZ1;
				}

				index++;
			}

			// This might be useful to get rid of random order bugs that happen
			std::shuffle(&game::particleData[0], 
				         &game::particleData[game::particleCount], 
				         std::default_random_engine((int)(game::currentTime*1000)));

			#pragma endregion



			// Loop over every chunk to reset the clock for only active chunks
			for (game::index3d chunk(game::chunkLength); !chunk.finished(); chunk++)
			{
				// If the chunk should be updated this frame
				if (game::chunkData[chunk.index].updateNextFrame == true || game::chunkData[chunk.index].updateCurrentFrame == true)
				{
					for (int i = 0; i < game::chunkVoxelNum; i++)
					{
						game::elementData[game::chunkVoxelNum * chunk.index + i].clock = 0;
					}
				}
			}

			// Upload to ssbo
			//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int) * game::voxelNum, game::voxData);

			// This is only if we have a compute shader
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);

			// To make this faster, what we could do is add the index3d of minX, minY, minZ to the the offset and add the index3d of maxX, maxY, maxZ to the size

			// Loop over every chunk to send changed chunks to gpu
			for (game::index3d chunk(game::chunkLength); !chunk.finished(); chunk++)
			{
				if (game::chunkData[chunk.index].updateNextFrame == true || game::chunkData[chunk.index].updateCurrentFrame == true)
				{
					glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * game::chunkVoxelNum * chunk.index, sizeof(unsigned int) * game::chunkVoxelNum, game::voxData + game::chunkVoxelNum * (unsigned long long)chunk.index);
				}
			}

			// Reset elementUpdateTick
			game::elementUpdateTick = 0;
		}
	}

	// Dispose of the element simulation
	void DisposeSimulation()
	{
		// Deallocate data for elements
		delete[] game::elementData;

		// Dispose of voxel color data
		delete[] game::voxData;

		// Dispose of chunk data
		delete[] game::chunkData;

		// Dispose of particle data
		delete[] game::particleData;
	}

#pragma endregion

}
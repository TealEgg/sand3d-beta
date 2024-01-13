#pragma once

namespace game
{
	// Window variables
	unsigned int width = 1024;
	unsigned int height = 540;
	const char* title = "sand3d-beta"; // SAND3D ?
	GLFWwindow* window;

	unsigned int currentWidth = width;
	unsigned int currentHeight = height;

	// Time variables
	double previousTime;
	double currentTime;
	double deltaTime;
	int frameCount;
	double elementUpdateTick;
#if PRINT_FPS
	double fpsTimeCounter;
	int fpsFrameCounter;
#endif

	// Player movement variables
	glm::vec3 positionVec = glm::vec3(0.0, 40.0, 0.0);
	float mouseSensitivity = 0.005f; // Sensitivity of rotation
	float speed = 20.0f; // 20 units per second
	float yaw = 3.14f / 4;
	float pitch = 0;
	float turnspeed = 3.14f; // half rotation per second
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	// Camera shake variables
	float trauma = 0.0f; // shake = trauma * trauma;

	void updateTrauma()
	{
		trauma = util::clampf(trauma, 0.0f, 1.0f);
		trauma -= game::deltaTime;
	}

	void addTrauma(float t) { trauma += t; trauma = util::clampf(trauma, 0.0f, 1.0f); }

	// mouse movement variables
	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;
	double mouseOffsetX;
	double mouseOffsetY;

	// Voxel Variables
	unsigned int chunkExp = 5; // The exponent of the chunk bounds (2 ^ chunkExp)
	unsigned int chunkBounds = pow(2, chunkExp); // The length of each chunk in number of voxels (Ex. 2 ^ 5 means chunks are 32 by 32 by 32)
	unsigned int chunkLength = 8; // The chunks per side length (Ex. 2 means that there are 2^3 chunks)
	unsigned int chunkNum = pow(chunkLength, 3); // The toal number of chunks (chunkLength ^ 3)
	unsigned int chunkVoxelNum = pow(chunkBounds, 3); // total voxels in a chunk
	unsigned int bounds = chunkLength * chunkBounds; // The length of the cube boundary in number of voxels
	unsigned int voxelNum = pow(bounds, 3); // bounds ^ 3 (the total voxels in the boundary)

	// Implement this in the future
	unsigned int vperm = 4; // Voxels per meter
	double voxelSize = 1.0 / vperm; // Size of each voxel

	void chunk::FrameChange()
	{
		updateCurrentFrame = updateNextFrame;
		updateNextFrame = false;

		currentMinX = nextMinX; currentMaxX = nextMaxX;
		currentMinY = nextMinY; currentMaxY = nextMaxY;
		currentMinZ = nextMinZ; currentMaxZ = nextMaxZ;

		nextMinX = game::chunkBounds - 1; nextMaxX = 0;
		nextMinY = game::chunkBounds - 1; nextMaxY = 0;
		nextMinZ = game::chunkBounds - 1; nextMaxZ = 0;
	}

	// Voxel Data Variables
	unsigned int * voxData; // Color data of the voxels
	element * elementData; // Element data of the voxels
	chunk * chunkData; // Chunk data for each chunk of voxels

	// Particle Data Variables
	const unsigned int maxParticles = 100000; // Max particles that can exist in the game;
	unsigned int particleCount = 0; // Total number of current particles
	particle * particleData; // Particle data for created particles


	// Note: The clock system would not make sense to implement due to the amount of claculations needed to get each bit
	// In the future maybe look into the boost::dynamic_bitset
	
	// Note: For the clock system to be useful, we need to be able to only clear certain spans of bits and not the entire array.
	// using the raytracing index would be nessesary.
	//unsigned char * clock; // To check if an element was iterated on or not
	//unsigned int getBit(int index)
	//{
	//	unsigned int byteIndex = index >> 3; // Divide by 3
	//	unsigned int bitLocation = index - byteIndex * 8;
	//	unsigned char mask = 1 << bitLocation;
	//	return clock[byteIndex] & mask;
	//}
	//void setBit(int index)
	//{
	//	unsigned int byteIndex = index >> 3; // Divide by 3
	//	unsigned int bitLocation = index - byteIndex * 8;
	//	unsigned char mask = 1 << bitLocation;
	//
	//	clock[byteIndex] |= mask;
	//}

	// The current and last index3dA that was generated
	unsigned int currentIndex = 0;
	unsigned int lastIndex = 0;

	// Gets the index of a voxel. Note: this function is zero indexed (starts at (0,0,0))
	unsigned int index3dA(unsigned int x, unsigned int y, unsigned int z)
	{
		lastIndex = currentIndex;

		unsigned int chunkX = x >> chunkExp; // Same as x / chunkBounds
		unsigned int chunkY = y >> chunkExp;
		unsigned int chunkZ = z >> chunkExp;

		unsigned int dX = x - (chunkX << chunkExp); // Same as x * chunkBounds
		unsigned int dY = y - (chunkY << chunkExp);
		unsigned int dZ = z - (chunkZ << chunkExp);

		/*currentIndex = chunkVoxelNum * (chunkX + chunkLength * (chunkY + chunkLength * chunkZ))
			+ (dX + chunkBounds * (dY + chunkBounds * dZ));*/

		currentIndex = chunkVoxelNum * (chunkX + chunkLength * (chunkY + chunkLength * chunkZ))
			+ (dX + ((dY + (dZ << chunkExp)) << chunkExp));

		return currentIndex;
	}
	unsigned int index3dB(unsigned int x, unsigned int y, unsigned int z)
	{
		return x + bounds * (y + bounds * z);
	}
	unsigned int index3dC(unsigned int x, unsigned int y, unsigned int z, unsigned int bound)
	{
		return x + bound * (y + bound * z);
	}

	// Temporary scroll wheel element selector
	int elementSelect;
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	// Perlin noise
	FastNoiseLite noise;

	// Reset mouseOffset
	bool resetMouseOffset = false;

	// Current Scene that the game is in
	scene currentScene = splashScreen;
	scene previousScene = splashScreen;
	bool isSceneUpdated = true;
	void changeScene(scene newScene)
	{
		previousScene = currentScene;
		currentScene = newScene;

		// isSceneUpdated will help us for UpdateScene function
		isSceneUpdated = false;
		// resetMouseOffset is so the camera doesn't jitter
		game::resetMouseOffset = true;
	}
	void updateScene();

	// Callback Definitions
	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void ProcessInput(GLFWwindow* window);
	unsigned int InitRayTracerShaders();

	// For Opengl Debugging
#if OPENGL_DEBUG
	void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

		std::cout << "---------------" << std::endl;
		std::cout << "Debug message (" << id << "): " << message << std::endl;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
		} std::cout << std::endl;

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
		} std::cout << std::endl;

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
		} std::cout << std::endl;
		std::cout << std::endl;
	}
#endif

	// Calls the initial boilerplate for the GLFW window (GLFW, OpenGL, GLAD bindings)
	void GLFWBoilerplate()
	{
		// Initialize glfw
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#if OPENGL_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

		// Initialize glfw window
		game::window = glfwCreateWindow(game::width, game::height, game::title, NULL, NULL);

		if (game::window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glfwMakeContextCurrent(game::window);

		// Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			exit(EXIT_FAILURE);
		}

		// Setting OpenGL viewport
		glViewport(0, 0, game::width, game::height);

		// Setting callbakc for input
		glfwSetKeyCallback(window, KeyCallback);

		// Setting callback for window resize
		glfwSetFramebufferSizeCallback(game::window, game::FramebufferSizeCallback);

		// Set Clear Color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// Allow blending (makes alpha transparent)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if OPENGL_DEBUG
		glDebugMessageCallback(MessageCallback, 0);
#endif

		// TODO: This part is temporary, remove after implementation of UI element selection
		glfwSetScrollCallback(game::window, scroll_callback);

		// This makes vsync turn on.
		//glfwSwapInterval(1);
	}

	// Prints FPS every second
	void fpsCheck()
	{
		fpsFrameCounter++;
		// If a second has passed.
		if (currentTime - fpsTimeCounter >= 1.0)
		{
			// Display the frame count here any way you want.
			std::cout << fpsFrameCounter << " fps (" << (1.0f / fpsFrameCounter * 1000) << " ms)" << " | particle count: " << particleCount << std::endl;

			fpsFrameCounter = 0;
			fpsTimeCounter = currentTime;
		}
	}

	// Initializes time variables
	void InitTime()
	{
		previousTime = 0.0;
		previousTime = glfwGetTime();
		frameCount = 1;
		elementUpdateTick = 0.0;
#if PRINT_FPS
		fpsTimeCounter = 0.0;
		fpsFrameCounter = 0;
#endif
	}

	// Updates Time variables
	void UpdateTime()
	{
		game::previousTime = game::currentTime;

		// setting currentTime to the current time of the window
		game::currentTime = glfwGetTime();

		// Calculating deltaTime of the frame
		game::deltaTime = game::currentTime - game::previousTime;

		// Calculating element updateTick
		game::elementUpdateTick += game::deltaTime;

		// Printing the fps
#if PRINT_FPS
		game::fpsCheck();
#endif
	}
}
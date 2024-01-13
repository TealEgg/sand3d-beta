#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <random> 

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>
#include <gtx/euler_angles.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <FastNoiseLite.h>
#include "util.h"
#include "shader.h"
#include "structs.h"

#define PRINT_FPS true
#define DEBUG_CHUNKS false
#define RANDOM_GEN true
#define OPENGL_DEBUG true

#include "game.h"
#include "elements.h"
#include "ui.h"
#include "simulation.h"

int main()
{
	game::GLFWBoilerplate();

	simulation::InitRaytrace();

	#if DEBUG_CHUNKS == true
	simulation::InitChunkDebug();
	#endif

	simulation::InitSimulation();
	ui::InitUI();

	// Disable the crosshair visibility 
	ui::solids[ui::crossHair].visible = false;

	bool disableSplash = true;

	// Splash Screen
	for (float time = 0, start = glfwGetTime(); time < 3.0f && !disableSplash; time = glfwGetTime() - start)
	{
		// Make screen black and draw splash
		glClear(GL_COLOR_BUFFER_BIT);
		ui::solids[ui::splashScreen].color.a = 255 * pow(std::min(1.0, time / 2.0), 2); // quadratic looks nicer than linear
		ui::DrawUI();

		glfwSwapBuffers(game::window);
		glfwPollEvents();
	}

	// This needs to be in while loop to look cool
	// Fade out
	//for (float time = 0, start = glfwGetTime(); time < 2.0f; time = glfwGetTime() - start)
	//{
	//	// Make alpha of screen and splash approach 0.0
	//	glClearColor(0.0f, 0.0f, 0.0f, pow((2.0 - time) / 2.0, 2));
	//	glClear(GL_COLOR_BUFFER_BIT);
	//	ui::solids[ui::splashScreen].color.a = 255 * pow((2.0 - time) / 2.0, 2);
	//	ui::DrawUI();
	//	glfwSwapBuffers(game::window);
	//	glfwPollEvents();
	//}

	// Disable the splash visibility 
	// Enable the crossHair visibility 
	ui::solids[ui::splashScreen].visible = false;
	//ui::solids[ui::crossHair].visible = true;
	// game::resetMouseOffset = true;
	changeScene(game::mainMenu);

	// Game Loop
	while (!glfwWindowShouldClose(game::window))
	{
		game::updateScene();

		game::UpdateTime();

		simulation::UpdateSimulation();

		game::ProcessInput(game::window);

		simulation::RenderRaytrace();

		// UpdateButtons and Draw UI should be fine to include
		// We might also want to include UpdateSimulation and RenderRaytrace for a default main menu render

		ui::UpdateButtons();

		ui::DrawUI();

		glfwSwapBuffers(game::window);
		glfwPollEvents();
	}

	//shader::deleteShader(computeTest);
	
	game::changeScene(game::gameExit);
	std::cout << "ending program" << std::endl;

	simulation::DisposeRaytrace();

	#if DEBUG_CHUNKS == true
	simulation::DisposeChunkDebug();
	#endif
	
	simulation::DisposeSimulation();

	ui::DisposeUI();
	
	glfwTerminate();
}

// Callback for when the screen size is changed
void game::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	game::currentWidth = width;
	game::currentHeight = height;

	glViewport(0, 0, width, height);

	// Update resolution in raytracer
	shader::useShader(simulation::raytraceProgram);
	glUniform2f(simulation::uniformResolution, width, height);

	//// Update compute resolution to raytracer
	//simulation::computeWidth = (width + (1 << simulation::pixelSizeExp) - 1) >> simulation::pixelSizeExp;
	//simulation::computeHeight = (height + (1 << simulation::pixelSizeExp) - 1) >> simulation::pixelSizeExp;
	//glUniform2f(simulation::uniformComputeResolution, simulation::computeWidth, simulation::computeHeight);
	//// Update prediction buffer size
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, simulation::predictionSSBO);
	//// Initialize SSBO with NULL because dat will be updating in shader
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int) * simulation::computeWidth * simulation::computeHeight, NULL, GL_DYNAMIC_DRAW);

#if DEBUG_CHUNKS == true
	shader::useShader(simulation::debugProgram);
	float fov = 2 * atan(tan(glm::radians(90.0f) * 0.5f) / ((float)width / (float)height));
	simulation::projection = glm::perspective(fov, (float)width / (float)height, 0.1f, 1000.0f);
	glUniformMatrix4fv(simulation::uniformProjection, 1, GL_FALSE, glm::value_ptr(simulation::projection));
#endif

	// Update orthoProjection for UI
	ui::orthoProjection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -10.0f, 10.0f);

	ui::ResizeUI(width, height);
}

// Input for precise key presses
void game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// If we are in either splash or main menu scenes, we are not alowed to used input
	if (game::currentScene == splashScreen || 
		game::currentScene == mainMenu) return;


	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		ui::choosingElement = !ui::choosingElement;
		ui::EnableMenu(ui::choosingElement);
	}

	if (key == GLFW_KEY_0)
	{
		game::addTrauma(0.25);
	}

#if DEBUG_CHUNKS == true
	if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS)
	{
		simulation::pause = !simulation::pause;
	}
	if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
	{
		simulation::increment = true;
	}
#endif
}

// Input for every frame
void game::ProcessInput(GLFWwindow* window)
{
	if (ui::choosingElement) return;

	// Put screen shake code here
	float maxAngle = glm::radians(1.0f); // 1 is much more calmer than 5
	game::noise.SetSeed(111);
	float yawOffset = maxAngle * game::trauma * game::trauma * game::noise.GetNoise(game::currentTime * 800, 0.0);
	game::noise.SetSeed(112);
	float pitchOffset = maxAngle * game::trauma * game::trauma * game::noise.GetNoise(game::currentTime * 800, 0.0);

	game::updateTrauma();

	// Calculating camera directions
	glm::mat4 viewMatrix = glm::eulerAngleYX(yaw + yawOffset, pitch + pitchOffset);

#if DEBUG_CHUNKS == true
	shader::useShader(simulation::debugProgram);
	// Left handed coordinates to right handed
	glm::vec3 p = glm::vec3(game::positionVec.x, game::positionVec.y, -game::positionVec.z);
	glm::vec3 v = glm::vec3(game::front.x, game::front.y, -game::front.z);
	simulation::view = glm::lookAt(p, p + v, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(simulation::uniformView, 1, GL_FALSE, glm::value_ptr(simulation::view));
#endif

	game::front = (viewMatrix * glm::vec4(0, 0, 1, 0)).xyz;
	game::right = (viewMatrix * glm::vec4(1, 0, 0, 0)).xyz;
	game::up = (viewMatrix * glm::vec4(0, 1, 0, 0)).xyz;

	// speed per seconds / frames per second
	float deltaSpeed = speed * deltaTime;

	// If shift is pressed than double the speed
	bool shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	float speed = deltaSpeed * (1 + shift);

	// This is to make sure keys don't do anything when in main menu
	if (game::currentScene == mainMenu) speed = 0.0f;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		game::positionVec.y += speed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		game::positionVec.y -= speed;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		game::positionVec += right * speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		game::positionVec -= right * speed;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		game::positionVec += front * speed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		game::positionVec -= front * speed;

	// Getting cursor position
	glfwGetCursorPos(window, &game::mouseX, &game::mouseY);

	// Calculating mouseoffset from last mouse position
	game::mouseOffsetX = game::mouseX - game::lastMouseX;
	game::mouseOffsetY = game::mouseY - game::lastMouseY;

	// setting last mouse position to current mouse position
	game::lastMouseX = game::mouseX;
	game::lastMouseY = game::mouseY;

	// TODO: This also seems to be happening when you exit the element slection menu (This happens extremely rarely though)

	if (game::resetMouseOffset || game::currentScene == mainMenu)
	{
		game::resetMouseOffset = false;
		game::mouseOffsetX = 0;
		game::mouseOffsetY = 0;
	}

	// Caclulating yaw and pitch of the camera
	game::yaw += game::mouseOffsetX * mouseSensitivity;
	game::pitch += game::mouseOffsetY * mouseSensitivity;

	// Clamping pitch value
	const float maxRotation = 3.14159f / 2;
	if (game::pitch > maxRotation) game::pitch = maxRotation;
	else if (game::pitch < -maxRotation) game::pitch = -maxRotation;

	// Sending direction vectors to shader
	shader::useShader(simulation::raytraceProgram);
	glUniform3fv(simulation::uniformFront, 1, glm::value_ptr(game::front));
	glUniform3fv(simulation::uniformRight, 1, glm::value_ptr(game::right));
	glUniform3fv(simulation::uniformUp, 1, glm::value_ptr(game::up));

	// Sending player position to shader
	glUniform3fv(simulation::uniformPlayerPosition, 1, glm::value_ptr(game::positionVec));

	// Sending direction vectors to shader
	//shader::useShader(simulation::predictionProgram);
	//glUniform3fv(simulation::predictionFront, 1, glm::value_ptr(game::front));
	//glUniform3fv(simulation::predictionRight, 1, glm::value_ptr(game::right));
	//glUniform3fv(simulation::predictionUp, 1, glm::value_ptr(game::up));
	//// Sending player position to shader
	//glUniform3fv(simulation::predictionPlayerPosition, 1, glm::value_ptr(game::positionVec));

}

// Temporary
void game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	elements::setElement(0, game::bounds - 1, 0, elements::identification["sand"]);
	game::voxData[game::index3dA(0, game::bounds - 1, 0)] = util::RGBA(255, 0, 255, 255);
	elements::setParticle(0, game::bounds - 1, 0, 70.0 / 32.0, 0, 0);

#if DEBUG_CHUNKS == true

	int maxRaySteps = 512;

#pragma region Bounding Box

	glm::vec3 rayPos = game::positionVec;
	glm::vec3 rayDir = game::front;

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
			if (elements::getElement(mapPos.x, mapPos.y, mapPos.z).elementID != 0 || game::voxData[game::index3dA(mapPos.x, mapPos.y, mapPos.z)] != 0)
			{
				std::cout << "Element Data at location (" << mapPos.x << ", " << mapPos.y << ", " << mapPos.z << ")" << std::endl;
				std::cout << "elementID: " << elements::descriptionList[game::elementData[game::currentIndex].elementID].name << std::endl;
				std::cout << "color: " << std::hex << game::voxData[game::currentIndex] << std::endl;
				std::cout << "register A: " << game::elementData[game::currentIndex].registerA << std::endl;
				std::cout << "register B: " << game::elementData[game::currentIndex].registerB << std::endl;
				std::cout << "register C: " << game::elementData[game::currentIndex].registerC << std::endl;
				std::cout << "register D: " << game::elementData[game::currentIndex].registerD << std::endl;
				std::cout << "register E: " << game::elementData[game::currentIndex].registerE << std::endl;
				std::cout << "register F: " << game::elementData[game::currentIndex].registerF << std::endl;
				std::cout << "register G: " << game::elementData[game::currentIndex].registerG << std::endl;
				std::cout << "register H: " << game::elementData[game::currentIndex].registerH << std::endl;
				std::cout << "register I: " << game::elementData[game::currentIndex].registerI << std::endl;
				std::cout << "register J: " << game::elementData[game::currentIndex].registerJ << std::endl;
				std::cout << "register K: " << game::elementData[game::currentIndex].registerK << std::endl;
				std::cout << "register L: " << game::elementData[game::currentIndex].registerL << std::endl;


				break;
			}
		}
	}

#pragma endregion

#endif
}

// Update the Scene if it changed
void game::updateScene()
{
	//     splash
	//     |
	//     v
	// +-- main <-+
	// |   |      |
	// |   v      |
	// |   game --+
	// |   |
	// |   v
	// +-> exit

	if (game::currentScene == game::previousScene) { isSceneUpdated = true; return; }

	if (game::currentScene == game::mainMenu && game::previousScene == game::splashScreen && !isSceneUpdated)
	{
		glfwSetInputMode(game::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// Initialize the main menu map
		// Do stuff

		isSceneUpdated = true;
	}
	else if (game::currentScene == game::mainMenu && game::previousScene == game::gameLoop && !isSceneUpdated)
	{
		// Initilize the main menu map
		// Make all game loop elements not visible

		// Disable the element selection menu if it is open
		if (ui::choosingElement)
		{
			ui::choosingElement = false;
			ui::EnableMenu(ui::choosingElement);
		}

		isSceneUpdated = true;
	}
	else if (game::currentScene == gameLoop && !isSceneUpdated) // If current scene is game loop, it always means it game from main menu
	{
		ui::solids[ui::crossHair].visible = true;

		// Initilize the game loop map
		// Make all menu elements not visible

		isSceneUpdated = true;
	}
}
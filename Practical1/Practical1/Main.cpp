#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> 
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <thread>

#include "ShaderUtility.h"
#include "Cloth.h"

const int SIMULATION_ITERATIONS_PER_FRAME = 32;

Cloth * cloth;

IntegrationScheme currentIntegrationScheme = verlet;

bool windEnabled = true;
bool renderParticlesAndSprings = false;
bool increasingStiffness = false;
bool decreasingStiffness = false;
bool increasingTimeStepSize = false;
bool decreasingTimeStepSize = false;

bool printElapsedTime = true;

// key input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_1:
			currentIntegrationScheme = forwardEuler;
			cloth->reinitialize(currentIntegrationScheme);
			std::cout << "Switched to Forward Euler" << std::endl;
			break;
		case GLFW_KEY_2:
			currentIntegrationScheme = semiImplicitEuler;
			cloth->reinitialize(currentIntegrationScheme);
			std::cout << "Switched to Semi-implicit Euler" << std::endl;
			break;
		case GLFW_KEY_3:
			currentIntegrationScheme = leapfrog;
			cloth->reinitialize(currentIntegrationScheme);
			std::cout << "Switched to Leapfrog" << std::endl;
			break;
		case GLFW_KEY_4:
			currentIntegrationScheme = verlet;
			cloth->reinitialize(currentIntegrationScheme);
			std::cout << "Switched to Verlet" << std::endl;
			break;
		case GLFW_KEY_SPACE:
			windEnabled = !windEnabled;
			std::cout << (std::string("Turned wind ") + (windEnabled ? "on" : "off")).c_str() << std::endl;
			break;
		case GLFW_KEY_R:
			renderParticlesAndSprings = !renderParticlesAndSprings;
			std::cout << "Switched rendering mode " << std::endl;
			break;
		case GLFW_KEY_X:
			increasingStiffness = true;
			break;
		case GLFW_KEY_Z:
			decreasingStiffness = true;
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (key) {
		case GLFW_KEY_X:
			increasingStiffness = false;
			break;
		case GLFW_KEY_Z:
			decreasingStiffness = false;
			break;
		case GLFW_KEY_M:
			increasingTimeStepSize = false;
			break;
		case GLFW_KEY_N:
			decreasingTimeStepSize = false;
			break;
		}
	}
}

int main(void) {
	GLFWwindow* window;

	//Initialize the glfw library:
	if (!glfwInit())
		return -1;

	int width = 1280;
	int height = 720;

	//Create a windowed mode window and its OpenGL context:
	window = glfwCreateWindow(width, height, "Practical 1", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	//Make the window's context current:
	glfwMakeContextCurrent(window);
	//Enable vsync. This limits the frame rate to the refresh rate of the screen (not required):
	//glfwSwapInterval(1);	
	glfwSwapInterval(0);

	glfwSetKeyCallback(window, key_callback);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		// Problem: glewInit failed, something is seriously wrong.
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		return -1;
	}

	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SMOOTH);

	GLhandleARB shaderProgramId;
	loadShader("./phong.vert", "./phong.frag", shaderProgramId);

	glUseProgram(shaderProgramId);
	GLint mvLocation = glGetUniformLocation(shaderProgramId, "mv");
	GLint mvpLocation = glGetUniformLocation(shaderProgramId, "mvp");
	GLint normalMatrixLocation = glGetUniformLocation(shaderProgramId, "normalMatrix");

	glm::mat4 modelViewMatrix;
	glm::mat4 perspectiveProjection;
	glm::mat4 modelViewProjectionMatrix;
	glm::mat4 normalTransformationMatrix;

	cloth = new Cloth(14, 10, 45, 35, currentIntegrationScheme, shaderProgramId);

	std::cout << "Press buttons 1-4 to change integration method (1: Forward Euler, 2: Semi-implicit Euler, 3: Leapfrog, 4: Verlet)" 
		<< std::endl;
	std::cout << "Press space to switch wind on/off" << std::endl;
	std::cout << "Press z and x to decrease/increase the spring stiffness force" << std::endl;
	std::cout << "Press n and m to decrease/increase the time step size" << std::endl;
	std::cout << "Press r to switch between mesh rendering and particle and springs rendering" << std::endl;

	//Loop until the user closes the window 
	while (!glfwWindowShouldClose(window)) {

		auto startTime = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < SIMULATION_ITERATIONS_PER_FRAME; i++) {
			//advance the simulation one time step (in a more efficient implementation this should be done in a separate thread to decouple rendering frame rate from simulation rate):
			cloth->addForce(vec3(0, -9.81, 0)); // add gravity each frame, pointing downwards
			if (windEnabled)
				cloth->windForce(vec3(1.0, 0, 1.0)); // generate some wind each frame
			cloth->timeStep(timeStepSizes[currentIntegrationScheme]); // calculate the particle positions of the current frame
		}

		//render:
		glfwGetFramebufferSize(window, &width, &height);
		float ratio = width / static_cast<float>(height);
		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (height == 0) {
			perspectiveProjection = glm::perspective(glm::radians(80.0f), static_cast<float>(width), 1.0f, 5000.0f);
		}
		else {
			perspectiveProjection = glm::perspective(glm::radians(80.0f), static_cast<float>(width) / 
				static_cast<float>(height), 1.0f, 5000.0f);
		}

		modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-6.5f, 6, -10));
		modelViewMatrix = glm::rotate(modelViewMatrix, glm::radians(25.0f), glm::vec3(0, 1, 0));
		normalTransformationMatrix = glm::inverse(glm::transpose(modelViewMatrix));
		
		modelViewProjectionMatrix = perspectiveProjection * modelViewMatrix;
		glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalTransformationMatrix));

		cloth->draw(renderParticlesAndSprings);
		
		//Swap front and back buffers 
		glfwSwapBuffers(window);

		//Poll for and process events 
		glfwPollEvents();

		if (increasingStiffness) {
			cloth->changeStiffness(0.01*INITIAL_SPRING_STIFFNESS);
			std::cout << "Spring stiffness: " << cloth->getSpringStiffness() << std::endl;
		}
		if (decreasingStiffness) {
			cloth->changeStiffness(-0.01 *INITIAL_SPRING_STIFFNESS);
			std::cout << "Spring stiffness: " << cloth->getSpringStiffness() << std::endl;
		}

		auto endTime = std::chrono::high_resolution_clock::now();

		std::chrono::duration<float> fs = endTime - startTime;
		std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(fs);

		if (printElapsedTime)
			std::cout << d.count() << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(16) - d);
	}

	delete cloth;

	glfwTerminate();
	return 0;
}
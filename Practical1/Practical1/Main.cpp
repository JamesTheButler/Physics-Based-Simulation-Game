#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> 
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderUtility.h"
#include "Cloth.h"

Cloth * cloth;

IntegrationScheme currentIntegrationScheme = verlet;
SCALAR timeStepSizeOffset = 0.0;
SCALAR springStiffness = 4.0;

bool windEnabled = true;
bool renderParticlesAndSprings = false;
bool increasingStiffness = false;
bool decreasingStiffness = false;
bool increasingTimeStepSize = false;
bool decreasingTimeStepSize = false;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_1:
			timeStepSizeOffset = 0;
			currentIntegrationScheme = forwardEuler;
			cloth->reinitialize(currentIntegrationScheme);
			std::cout << "Switched to Forward Euler" << std::endl;
			break;
		case GLFW_KEY_2:
			timeStepSizeOffset = 0;
			currentIntegrationScheme = semiImplicitEuler;
			cloth->reinitialize(currentIntegrationScheme);
			std::cout << "Switched to Semi-implicit Euler" << std::endl;
			break;
		case GLFW_KEY_3:
			timeStepSizeOffset = 0;
			currentIntegrationScheme = leapfrog;
			cloth->reinitialize(currentIntegrationScheme);
			std::cout << "Switched to Leapfrog" << std::endl;
			break;
		case GLFW_KEY_4:
			timeStepSizeOffset = 0;
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
		case GLFW_KEY_M:
			increasingTimeStepSize = true;
			break;
		case GLFW_KEY_N:
			decreasingTimeStepSize = true;
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

	cloth = new Cloth(14, 10, 55, 45, currentIntegrationScheme, shaderProgramId);

	std::cout << "Press buttons 1-4 to change integration method (1: Forward Euler, 2: Semi-implicit Euler, 3: Leapfrog, 4: Verlet)" 
		<< std::endl;
	std::cout << "Press space to switch wind on/off" << std::endl;
	std::cout << "Press z and x to decrease/increase the spring stiffness force" << std::endl;
	std::cout << "Press n and m to decrease/increase the time step size" << std::endl;
	std::cout << "Press r to switch between mesh rendering and particle and springs rendering" << std::endl;

	//Loop until the user closes the window 
	while (!glfwWindowShouldClose(window)) {
		//advance the simulation one time step (in a more efficient implementation this should be done in a separate thread to decouple rendering frame rate from simulation rate):
		cloth->addForce(vec3(0, -0.2, 0)*static_cast<SCALAR>(0.0625)); // add gravity each frame, pointing downwards
		if (windEnabled)
			cloth->windForce(vec3(0.5, 0, 0.2)*static_cast<SCALAR>(0.0625)); // generate some wind each frame
		cloth->timeStep(intialTimeStepSizes[currentIntegrationScheme] + timeStepSizeOffset, springStiffness); // calculate the particle positions of the current frame

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
			springStiffness += 0.001;
			std::cout << "Spring stiffness: " << springStiffness << std::endl;
		}
		if (decreasingStiffness) {
			springStiffness -= 0.001;
			std::cout << "Spring stiffness: " << springStiffness << std::endl;
		}
		if (increasingTimeStepSize) {
			timeStepSizeOffset += 0.00001;
			std::cout << "Time step size: " << intialTimeStepSizes[currentIntegrationScheme] + timeStepSizeOffset 
				<< std::endl;
		}
		if (decreasingTimeStepSize) {
			timeStepSizeOffset -= 0.00001;
			std::cout << "Time step size: " << intialTimeStepSizes[currentIntegrationScheme] + timeStepSizeOffset 
				<< std::endl;
		}
	}

	delete cloth;

	glfwTerminate();
	return 0;
}
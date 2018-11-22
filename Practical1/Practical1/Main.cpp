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

//#define DOUBLE_PRECISION			//Uncomment this line to switch to double precision
#include "Cloth.h"
#include "RagDoll.h"
#include "Chain.h"

const SCALAR INITIAL_TIME_STEP_SIZE = 0.008;
const SCALAR DRAG_CONSTANT = 0.7;
const int CONSTRAINT_ITERATIONS = 2;

const int SIMULATION_ITERATIONS_PER_FRAME = 2;

Cloth * cloth;
RagDoll * ragDoll;
Chain * chain;

IntegrationScheme currentIntegrationScheme = verlet;

bool windEnabled = true;
bool renderParticlesAndConstraints = false;
bool printElapsedTime = false;
bool drawCloth = true;
bool drawChain = true;
bool drawRagDoll = true;
bool dragEnabled = true;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_1:
			drawCloth = true;
			drawChain = false;
			drawRagDoll = false;
			std::cout << "Switched to drawing cloth" << std::endl;
			break;
		case GLFW_KEY_2:
			drawCloth = false;
			drawChain = true;
			drawRagDoll = false;
			std::cout << "Switched to drawing chain" << std::endl;
			break;
		case GLFW_KEY_3:
			drawCloth = false;
			drawChain = false;
			drawRagDoll = true;
			std::cout << "Switched to drawing rag doll" << std::endl;
			break;
		case GLFW_KEY_A:
			drawCloth = true;
			drawChain = true;
			drawRagDoll = true;
			std::cout << "Switched to drawing all objects" << std::endl;
			break;
		case GLFW_KEY_I:
			cloth->reinitialize(currentIntegrationScheme);
			ragDoll->reinitialize(currentIntegrationScheme);
			chain->reinitialize(currentIntegrationScheme);
			std::cout << "Reinitialized the simulation" << std::endl;
			break;
		case GLFW_KEY_SPACE:
			windEnabled = !windEnabled;
			std::cout << (std::string("Turned wind ") + (windEnabled ? "on" : "off")).c_str() << std::endl;
			break;
		case GLFW_KEY_R:
			renderParticlesAndConstraints = !renderParticlesAndConstraints;
			std::cout << "Switched rendering mode " << std::endl;
			break;
		case GLFW_KEY_P:
			printElapsedTime = !printElapsedTime;
			std::cout << "Switched printing of elapsed time" << std::endl;
			break;
		case GLFW_KEY_D:
			dragEnabled = !dragEnabled;
			std::cout << (std::string("Turned drag ") + (dragEnabled ? "on" : "off")).c_str() << std::endl;
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

	const SCALAR timeStepSize = INITIAL_TIME_STEP_SIZE;
	const SCALAR dragConstant = DRAG_CONSTANT;
	const int constraintIterations = CONSTRAINT_ITERATIONS;

	cloth = new Cloth(14, 10, 55, 45, currentIntegrationScheme, shaderProgramId);
	cloth->solver->setConstraintIterations(constraintIterations);
	cloth->solver->setDragConstant(dragConstant);

	chain = new Chain(currentIntegrationScheme, shaderProgramId);
	chain->solver->setConstraintIterations(constraintIterations);
	chain->solver->setDragConstant(dragConstant);

	ragDoll = new RagDoll(currentIntegrationScheme, shaderProgramId);
	ragDoll->solver->setConstraintIterations(constraintIterations);
	ragDoll->solver->setDragConstant(dragConstant);

	std::cout << "Press 1 to draw cloth, 2 to draw chain, 3 to draw rag doll, and press a to render all of them" << std::endl;
	std::cout << "Press space to switch wind on/off" << std::endl;
	std::cout << "Press r to switch between mesh rendering and particle and constraints rendering" << std::endl;
	std::cout << "Press i to reinitialize the simulation" << std::endl;
	std::cout << "Press p to turn printing of elapsed time on or off" << std::endl;

	//Loop until the user closes the window 
	while (!glfwWindowShouldClose(window)) {

		auto startTime = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < SIMULATION_ITERATIONS_PER_FRAME; i++) {
			//advance the simulation one time step (in a more efficient implementation this should be done in a separate thread to decouple rendering frame rate from simulation rate):
			cloth->addForce(vec3(0, -9.81, 0)); // add gravity each frame, pointing downwards
			if (windEnabled)
				cloth->windForce(vec3(8.0, 0, 7.0)); // generate some wind each frame
			cloth->timeStep(timeStepSize, dragEnabled); // calculate the particle positions of the current frame

			chain->addForce(vec3(0, -9.81, 0)); // add gravity each frame, pointing downwards
			chain->timeStep(timeStepSize, dragEnabled); // calculate the particle positions of the current frame

			ragDoll->addForce(vec3(0, -9.81, 0)); // add gravity each frame, pointing downwards
			ragDoll->timeStep(timeStepSize, dragEnabled); // calculate the particle positions of the current frame
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

		if (drawCloth) {
			modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-6.5f, 6, -10));
			modelViewMatrix = glm::rotate(modelViewMatrix, glm::radians(25.0f), glm::vec3(0, 1, 0));
			normalTransformationMatrix = glm::inverse(glm::transpose(modelViewMatrix));

			modelViewProjectionMatrix = perspectiveProjection * modelViewMatrix;
			glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
			glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
			glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalTransformationMatrix));

			cloth->calculateNormals();
			cloth->renderer->draw(renderParticlesAndConstraints);
		}

		if (drawChain) {
			modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0, -5));
			normalTransformationMatrix = glm::inverse(glm::transpose(modelViewMatrix));

			modelViewProjectionMatrix = perspectiveProjection * modelViewMatrix;
			glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
			glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
			glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalTransformationMatrix));

			chain->renderer->draw(true);
		}

		if (drawRagDoll) {
			modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0, -5));
			normalTransformationMatrix = glm::inverse(glm::transpose(modelViewMatrix));

			modelViewProjectionMatrix = perspectiveProjection * modelViewMatrix;
			glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
			glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
			glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalTransformationMatrix));

			ragDoll->renderer->draw(true);
		}
		
		//Swap front and back buffers 
		glfwSwapBuffers(window);

		//Poll for and process events 
		glfwPollEvents();

		auto endTime = std::chrono::high_resolution_clock::now();

		std::chrono::duration<float> fs = endTime - startTime;
		std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(fs);
		
		if (printElapsedTime)
			std::cout << d.count() << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(16) - d);
	}

	delete cloth;
	delete ragDoll;

	glfwTerminate();
	return 0;
}
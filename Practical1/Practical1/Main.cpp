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

#include "ParticleNetworkRenderer.h"
#include "Particle.h"
#include "PlaneRenderer.h"
#include "PlaneCollider.h"
#include "Collider.h"
#include "RopeManager.h"
#include "Character.h"

const SCALAR INITIAL_TIME_STEP_SIZE = 0.008;
const SCALAR DRAG_CONSTANT = 0.0;
const int CONSTRAINT_ITERATIONS = 2;

const float CHAR_SIZE = 0.1f;
const float CHAR_ARM_LENGTH = 3.5f;
const float ROPE_SIZE = 0.25f;
const float GRAVITY = -4.f;
const int SIMULATION_ITERATIONS_PER_FRAME = 3;
const float CONNECTION_THRESHOLD = .2f;

Character * character;
Rope * rope;
RopeManager * ropeMgr;
PlaneCollider * topPlaneCollider;
PlaneCollider * bottomPlaneCollider;

IntegrationScheme currentIntegrationScheme = verlet;

bool windEnabled = false;
bool renderParticlesAndConstraints = false;
bool printElapsedTime = false;
bool dragEnabled = true;
bool isPlayerGravityEnabled = false;
bool areArmsSticky = false;
float timer = 0.0f;
float viewingAngle = 25.0f;

void startGame() {
	isPlayerGravityEnabled = true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_I:
			character->reinitialize(currentIntegrationScheme);
			rope->reinitialize(currentIntegrationScheme);
			timer = 0.0f;
			std::cout << "Reinitialized the simulation" << std::endl;
			break;
		case GLFW_KEY_SPACE:
			windEnabled = !windEnabled;
			std::cout << (std::string("Turned wind ") + (windEnabled ? "on" : "off")).c_str() << std::endl;
			break;
		case GLFW_KEY_P:
			printElapsedTime = !printElapsedTime;
			std::cout << "Switched printing of elapsed time" << std::endl;
			break;
		case GLFW_KEY_D:
			dragEnabled = !dragEnabled;
			std::cout << (std::string("Turned drag ") + (dragEnabled ? "on" : "off")).c_str() << std::endl;
			break;
		case GLFW_KEY_1:
			startGame();
			break;
		case GLFW_KEY_2:
			areArmsSticky=!areArmsSticky;
			if (!areArmsSticky)
				std::cout << "NOT ";
			std::cout << "arms sticky\n";
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
	window = glfwCreateWindow(width, height, "Practical 3", NULL, NULL);
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

	character = new Character(currentIntegrationScheme, shaderProgramId, CHAR_SIZE, CHAR_ARM_LENGTH, vec3(3,4,0));
	character->solver->setConstraintIterations(constraintIterations);
	character->solver->setDragConstant(dragConstant);
	
	std::vector<Collider *> colliders;

	topPlaneCollider = new PlaneCollider(vec3(0, 1, 0), vec3(0, -1, 0), shaderProgramId);
	topPlaneCollider->setActive(true);
	colliders.push_back(topPlaneCollider);

	bottomPlaneCollider = new PlaneCollider(vec3(0, 1, 0), vec3(0, 1, 0), shaderProgramId);
	bottomPlaneCollider->setActive(true);
	colliders.push_back(bottomPlaneCollider);

	ropeMgr = new RopeManager( shaderProgramId, constraintIterations, dragConstant, ROPE_SIZE);

	std::cout << "Press i to reinitialize the simulation" << std::endl;
	std::cout << "Press p to turn printing of elapsed time on or off" << std::endl;
	std::cout << "Press c to turn capsule collider on/off" << std::endl;
	std::cout << "Press v to turn sphere collider on/off" << std::endl;
	std::cout << "Press b to turn plane collider on/off" << std::endl;

	//Loop until the user closes the window 
	while (!glfwWindowShouldClose(window)) {
		auto startTime = std::chrono::high_resolution_clock::now();

		timer++;
		if (areArmsSticky)
			character->applyConnectorConstraints(ropeMgr, CONNECTION_THRESHOLD);

		for (int i = 0; i < SIMULATION_ITERATIONS_PER_FRAME; i++) {
			//advance the simulation one time step (in a more efficient implementation this should be done in a separate thread to decouple rendering frame rate from simulation rate):
			if (isPlayerGravityEnabled) {
				character->addForce(vec3(0, GRAVITY, 0));
				character->timeStep(timeStepSize, dragEnabled);
			}
			ropeMgr->timeStep(GRAVITY, timeStepSize);
		}
		character->removeConnectorConstraints();

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

		//draw rope
		modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0, -5));
		normalTransformationMatrix = glm::inverse(glm::transpose(modelViewMatrix));

		modelViewProjectionMatrix = perspectiveProjection * modelViewMatrix;
		glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalTransformationMatrix));

		ropeMgr->draw();
		character->renderer->draw();

		
		//draw planes
		//TODO: put into own function
		//top
		/*modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-6.5f, 6, -10));
		modelViewMatrix = glm::rotate(modelViewMatrix, glm::radians(viewingAngle), glm::vec3(0, 1, 0));
		modelViewMatrix = glm::translate(modelViewMatrix, topPlaneCollider->getPosition());
		normalTransformationMatrix = glm::inverse(glm::transpose(modelViewMatrix));

		modelViewProjectionMatrix = perspectiveProjection * modelViewMatrix;
		glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalTransformationMatrix));

		topPlaneCollider->renderer->draw();
		//bottom
		modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-6.5f, 6, -10));
		modelViewMatrix = glm::rotate(modelViewMatrix, glm::radians(viewingAngle), glm::vec3(0, 1, 0));
		modelViewMatrix = glm::translate(modelViewMatrix, bottomPlaneCollider->getPosition());
		normalTransformationMatrix = glm::inverse(glm::transpose(modelViewMatrix));

		modelViewProjectionMatrix = perspectiveProjection * modelViewMatrix;
		glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalTransformationMatrix));

		bottomPlaneCollider->renderer->draw();*/

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

	delete character;
	ropeMgr->deleteRopes();

	glfwTerminate();
	return 0;


}

 
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
#include "Collider.h"

#include "PlaneCollider.h"
#include "AABBRenderer.h"
#include "AABBCollider.h"

#include "RopeManager.h"
#include "Character.h"

const SCALAR INITIAL_TIME_STEP_SIZE = 0.008;
const SCALAR DRAG_CONSTANT = 0.0;
const int CONSTRAINT_ITERATIONS = 2;

const float CHAR_SIZE = 0.12f;
const float CHAR_ARM_LENGTH = 3.5f;
const float BOX_SIZE = 1.f;
const float ROPE_SIZE = 0.25f;
const float GRAVITY = -4.f;
const float INPUT_POWER = 4.0f;
const int SIMULATION_ITERATIONS_PER_FRAME = 3;
const float CONNECTION_THRESHOLD = .1f;

Character * character;
Rope * rope;
RopeManager * ropeMgr;
PlaneCollider * leftPlaneCollider;
PlaneCollider * rightPlaneCollider;
PlaneCollider * bottomPlaneCollider;
AABBCollider * destinationBox;
AABBCollider * obstacleBox;


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

void moveRight() {
	character->addForce(vec3(INPUT_POWER, 0, 0));
}

void moveLeft() {
	character->addForce(vec3(-INPUT_POWER, 0, 0));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_SPACE:
			windEnabled = !windEnabled;
			std::cout << (std::string("Turned wind ") + (windEnabled ? "on" : "off")).c_str() << std::endl;
			break;
		case GLFW_KEY_P:
			printElapsedTime = !printElapsedTime;
			std::cout << "Switched printing of elapsed time" << std::endl;
			break;
		case GLFW_KEY_O:
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
		case GLFW_KEY_A:
			moveLeft();
			break;
		case GLFW_KEY_D:
			moveRight();
			break;
		}
	}

	if (action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_A:
			moveLeft();
			break;
		case GLFW_KEY_D:
			moveRight();
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

	glClearColor(1.f, 1.f, 1.f, 0.5f);
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

	std::vector<Collider *> colliders;

	leftPlaneCollider = new PlaneCollider(vec3(-11.25, 0, 0), vec3(1, 0, 0), shaderProgramId);
	leftPlaneCollider->setActive(true);
	colliders.push_back(leftPlaneCollider);

	rightPlaneCollider = new PlaneCollider(vec3(5, 0, 0), vec3(-1, 0, 0), shaderProgramId);
	rightPlaneCollider->setActive(true);
	colliders.push_back(rightPlaneCollider);

	bottomPlaneCollider = new PlaneCollider(vec3(3, 0, 0), vec3(0, 1, 0), shaderProgramId);
	bottomPlaneCollider->setActive(true);
	colliders.push_back(bottomPlaneCollider);

	destinationBox = new AABBCollider(vec3(-10.0f, 0.25f, 0), 3, .5f, shaderProgramId);
	destinationBox->setActive(true);
	colliders.push_back(destinationBox);

	obstacleBox = new AABBCollider(vec3(-8.6715f, .5f, 0), 0.5f, 1, shaderProgramId);
	obstacleBox->setActive(true);
	colliders.push_back(obstacleBox);

	character = new Character(currentIntegrationScheme, shaderProgramId, CHAR_SIZE, CHAR_ARM_LENGTH, vec3(3,4,0));
	character->solver->setConstraintIterations(constraintIterations);
	character->solver->setDragConstant(dragConstant);
	character->solver->setColliders(colliders);

	ropeMgr = new RopeManager( shaderProgramId, constraintIterations, dragConstant, ROPE_SIZE, vec3(0,4.f,0));

	std::cout << "Press 1 to start the game." << std::endl;
	std::cout << "Press 2 to make arms sticky/unsticky." << std::endl;

	//Loop until the user closes the window 
	while (!glfwWindowShouldClose(window)) {
		auto startTime = std::chrono::high_resolution_clock::now();
		timer++;

		// enable connectors
		if (areArmsSticky)
			character->tryConnectorConstraint(ropeMgr, CONNECTION_THRESHOLD);

		for (int i = 0; i < SIMULATION_ITERATIONS_PER_FRAME; i++) {
			//advance the simulation one time step (in a more efficient implementation this should be done in a separate thread to decouple rendering frame rate from simulation rate):
			if (isPlayerGravityEnabled) {
				character->addForce(vec3(0, GRAVITY, 0));
				character->timeStep(timeStepSize, dragEnabled);
			}
			ropeMgr->timeStep(GRAVITY, timeStepSize);
		}
		// delete all connectors if arms are not sticky
		if(!areArmsSticky)
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

		//set up 2D rendering
		//all objects are drawn with the same model view projection matrix
		modelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0, -5));
		normalTransformationMatrix = glm::inverse(glm::transpose(modelViewMatrix));

		modelViewProjectionMatrix = perspectiveProjection * modelViewMatrix;
		glUniformMatrix4fv(mvLocation, 1, GL_FALSE, glm::value_ptr(modelViewMatrix));
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalTransformationMatrix));

		//draw rope
		ropeMgr->draw();
		character->renderer->draw();

		//draw planes + boxes
		leftPlaneCollider->renderer->draw();
		rightPlaneCollider->renderer->draw();
		bottomPlaneCollider->renderer->draw();

		destinationBox->renderer->draw();
		obstacleBox->renderer->draw();

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

 
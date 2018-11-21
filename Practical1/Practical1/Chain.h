#pragma once

#include "PositionBasedObject.h"

class Chain : public PositionBasedObject {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> normals;
	int numberOfParticles;

	//--------------------------------------- Private methods ----------------------------------------------------
	void initializePositions() {
		positions[0] = glm::vec3(0, 0, 0);
		positions[1] = glm::vec3(0.5, 0.5, 0);
		positions[2] = glm::vec3(1, 0, 0);
		positions[3] = glm::vec3(0.5, -0.5, 0);
		positions[4] = glm::vec3(1.5, 0.5, 0);
		positions[5] = glm::vec3(2, 0, 0);
		positions[6] = glm::vec3(1.5, -0.5, 0);
		positions[7] = glm::vec3(2.5, 0.5, 0);
		positions[8] = glm::vec3(3, 0, 0);
		positions[9] = glm::vec3(2.5, -0.5, 0);
		positions[10] = glm::vec3(3.5, 0.5, 0);
		positions[11] = glm::vec3(4, 0, 0);
		positions[12] = glm::vec3(3.5, -0.5, 0);
		positions[13] = glm::vec3(4.5, 0.5, 0);
		positions[14] = glm::vec3(5, 0, 0);
		positions[15] = glm::vec3(4.5, -0.5, 0);
	}

	void initializeConstraints() {
		//first block
		makeConstraint(0, 1);
		makeConstraint(0, 2);
		makeConstraint(0, 3);
		makeConstraint(1, 2);
		makeConstraint(1, 3);
		makeConstraint(2, 3);
		
		makeConstraint(2, 4);
		makeConstraint(2, 5);
		makeConstraint(2, 6);
		makeConstraint(4, 5);
		makeConstraint(4, 6);
		makeConstraint(5, 6);
		
		makeConstraint(5, 7);
		makeConstraint(5, 8);
		makeConstraint(5, 9);
		makeConstraint(7, 8);
		makeConstraint(7, 9);
		makeConstraint(8, 9);
		
		makeConstraint(8, 10);
		makeConstraint(8, 11);
		makeConstraint(8, 12);
		makeConstraint(10, 11);
		makeConstraint(10, 12);
		makeConstraint(11, 12);

		makeConstraint(11, 13);
		makeConstraint(11, 14);
		makeConstraint(11, 15);
		makeConstraint(13, 14);
		makeConstraint(13, 15);
		makeConstraint(14, 15);

		isMovables[0] = false;
	}

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	Chain(IntegrationScheme integrationScheme, GLhandleARB shaderProgramId) :
		PositionBasedObject() {

		numberOfParticles = 16;

		positions.resize(numberOfParticles);
		oldPositions.resize(numberOfParticles);
		accelerations.resize(numberOfParticles, vec3(0, 0, 0));

		masses.resize(numberOfParticles, 1);
		isMovables.resize(numberOfParticles, true);
		velocities.resize(numberOfParticles, vec3(0, 0, 0));

		solver = new Solver(integrationScheme, positions, oldPositions, velocities, accelerations, masses, isMovables, constraints);
		renderer = new Renderer(shaderProgramId, positions, constraints, trianglesIndices, numberOfParticles);

		initializePositions();
		for (int i = 0; i < positions.size(); i++) {
			oldPositions[i] = positions[i];
		}
		initializeConstraints();

		isMovables[0] = false;		//Set the top particle be pinned/unmovable.

		normals.resize(numberOfParticles, vec3(0, 0, 0));
		renderer->setupOpenGLBuffers();
		renderer->setNormals(normals);
	}

	void reinitialize(IntegrationScheme integrationScheme) {
		solver->setIntegrationScheme(integrationScheme);
		solver->setToFirstTimeStep();

		std::fill(accelerations.begin(), accelerations.end(), vec3(0, 0, 0));
		std::fill(velocities.begin(), velocities.end(), vec3(0, 0, 0));

		oldPositions.resize(numberOfParticles);

		initializePositions();

		for (int i = 0; i < positions.size(); i++) {
			oldPositions[i] = positions[i];
		}
	}
};
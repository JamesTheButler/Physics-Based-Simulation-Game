#pragma once

#include "PositionBasedObject.h"

class RagDoll : public PositionBasedObject {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> normals;
	int numberOfParticles;

	//--------------------------------------- Private methods ----------------------------------------------------
	void initializePositions() {
		positions[0] = vec3(-1.0, 0.0, 0.0);
		positions[1] = vec3(-1.0, 1.0, 0.0);
		positions[2] = vec3(-0.75, 2.0, 0.0);
		positions[3] = vec3(-0.5, 2.5, 0.0);
		positions[4] = vec3(-1.0, 3.5, 0.0);
		positions[5] = vec3(-1.25, 2.75, 0.0);
		positions[6] = vec3(-1.25, 2.1, 0.0);
		positions[7] = vec3(0.0, 4.0, 0.0);
		positions[8] = vec3(0.0, 4.5, 0.0);
		positions[9] = vec3(1.0, 3.5, 0.0);
		positions[10] = vec3(1.25, 2.75, 0.0);
		positions[11] = vec3(1.25, 2.1, 0.0);
		positions[12] = vec3(0.5, 2.5, 0.0);
		positions[13] = vec3(0.75, 2.0, 0.0);
		positions[14] = vec3(1.0, 1.0, 0.0);
		positions[15] = vec3(1.0, 0.0, 0.0);
	}

	void initializeConstraints() {
		//write your code here...
	}

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	RagDoll(IntegrationScheme integrationScheme, GLhandleARB shaderProgramId) :
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

		isMovables[6] = false;		//Set the left hand to be pinned/unmovable.

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
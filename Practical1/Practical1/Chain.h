#pragma once

#include "PositionBasedObject.h"

class Chain : public PositionBasedObject {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> normals;
	int numberOfParticles;

	//--------------------------------------- Private methods ----------------------------------------------------
	void initializePositions() {
		//write your code here...
	}

	void initializeConstraints() {
		//write your code here...
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
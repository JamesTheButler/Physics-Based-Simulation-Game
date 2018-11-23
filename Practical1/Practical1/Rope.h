#pragma once
#pragma once

#include "PositionBasedObject.h"

class Rope : public PositionBasedObject {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> normals;
	int numberOfParticles;
	vec3 anchor;
	float size;

	//--------------------------------------- Private methods ----------------------------------------------------
	void initializePositions() {
		for(int i=0; i<numberOfParticles; i++)
			positions[i] = anchor + vec3((float)i*size, 0.f, 0.f);
		isMovables[0] = false;
	}

	void initializeConstraints() {
		for(int i=0; i<numberOfParticles-1; i++)
			makeConstraint(i, i + 1);
	}

public:
	//--------------------------------------- Public methods -----------------------------------------------------
	Rope(IntegrationScheme integrationScheme, GLhandleARB shaderProgramId, float size, vec3 anchor) :
		PositionBasedObject() {
		this->size = size;
		this->anchor = anchor;
		numberOfParticles = 10;

		positions.resize(numberOfParticles);
		oldPositions.resize(numberOfParticles);
		accelerations.resize(numberOfParticles, vec3(0, 0, 0));

		masses.resize(numberOfParticles, 1);
		isMovables.resize(numberOfParticles, true);
		velocities.resize(numberOfParticles, vec3(0, 0, 0));

		solver = new Solver(integrationScheme, positions, oldPositions, velocities, accelerations, masses, isMovables, constraints);
		renderer = new ParticleNetworkRenderer(shaderProgramId, positions, constraints, numberOfParticles);

		initializePositions();
		for (int i = 0; i < positions.size(); i++) {
			oldPositions[i] = positions[i];
		}
		initializeConstraints();

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
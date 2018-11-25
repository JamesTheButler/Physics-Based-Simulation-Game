#pragma once
#pragma once

#include "PositionBasedObject.h"
#include <ctime>

class Rope : public PositionBasedObject {
private:

	std::vector<vec3> normals;
	int numberOfParticles;
	vec3 anchor;
	float size;

	//TODO: radnom particle displacement
	void initializePositions(float angle) {
		for (int i = 0; i < numberOfParticles; i++) {
			positions[i] = anchor + vec3((float)i*(sin(angle)*size), i*-cos(angle)*size, 0.f);
		}
		isMovables[0] = false;
	}

	void initializeConstraints() {
		for(int i=0; i<numberOfParticles-1; i++)
			makeConstraint(i, i + 1);
	}

public:
	Rope(IntegrationScheme integrationScheme, GLhandleARB shaderProgramId, float size, vec3 anchor, float angle) :
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

		initializePositions(angle);
		for (int i = 0; i < positions.size(); i++) {
			oldPositions[i] = positions[i];
		}
		initializeConstraints();

		normals.resize(numberOfParticles, vec3(0, 0, 0));
		renderer->setupOpenGLBuffers();
		renderer->setNormals(normals);
	}

	void reinitialize(IntegrationScheme currentIntegrationScheme) {
		solver->setIntegrationScheme(currentIntegrationScheme);
		solver->setToFirstTimeStep();

		std::fill(accelerations.begin(), accelerations.end(), vec3(0, 0, 0));
		std::fill(velocities.begin(), velocities.end(), vec3(0, 0, 0));

		oldPositions.resize(numberOfParticles);

		initializePositions(0);

		for (int i = 0; i < positions.size(); i++) {
			oldPositions[i] = positions[i];
		}
	}
};
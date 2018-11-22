#pragma once

#include "PositionBasedObject.h"

class Chain : public PositionBasedObject {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> normals;
	int numberOfParticles;

	//--------------------------------------- Private methods ----------------------------------------------------
	void initializePositions() {
		positions[0] = vec3(0.0, 0.0, 0.0);
		positions[1] = vec3(0.25, 0.25, 0.0);
		positions[2] = vec3(0.25, -0.25, 0.0);
		positions[3] = vec3(0.5, 0.0, 0.0);

		positions[4] = vec3(0.75, 0.25, 0.0);
		positions[5] = vec3(0.75, -0.25, 0.0);
		positions[6] = vec3(1.0, 0.0, 0.0);

		positions[7] = vec3(1.25, 0.25, 0.0);
		positions[8] = vec3(1.25, -0.25, 0.0);
		positions[9] = vec3(1.5, 0.0, 0.0);

		positions[10] = vec3(1.75, 0.25, 0.0);
		positions[11] = vec3(1.75, -0.25, 0.0);
		positions[12] = vec3(2.0, 0.0, 0.0);

		positions[13] = vec3(2.25, 0.25, 0.0);
		positions[14] = vec3(2.25, -0.25, 0.0);
		positions[15] = vec3(2.5, 0.0, 0.0);
	}

	void initializeConstraints() {
		makeConstraint(0, 1);
		makeConstraint(0, 2);
		makeConstraint(0, 3);
		makeConstraint(1, 2);
		makeConstraint(1, 3);
		makeConstraint(2, 3);

		makeConstraint(3, 4);
		makeConstraint(3, 5);
		makeConstraint(3, 6);
		makeConstraint(4, 5);
		makeConstraint(4, 6);
		makeConstraint(5, 6);

		makeConstraint(6, 7);
		makeConstraint(6, 8);
		makeConstraint(6, 9);
		makeConstraint(7, 8);
		makeConstraint(7, 9);
		makeConstraint(8, 9);

		makeConstraint(9, 10);
		makeConstraint(9, 11);
		makeConstraint(9, 12);
		makeConstraint(10, 11);
		makeConstraint(10, 12);
		makeConstraint(11, 12);

		makeConstraint(12, 13);
		makeConstraint(12, 14);
		makeConstraint(12, 15);
		makeConstraint(13, 14);
		makeConstraint(13, 15);
		makeConstraint(14, 15);
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
		renderer = new ParticleNetworkRenderer(shaderProgramId, positions, constraints, numberOfParticles);

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
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
		makeConstraint(0, 1);
		makeConstraint(1, 2);
		makeConstraint(2, 3);
		makeConstraint(15, 14);
		makeConstraint(14, 13);
		makeConstraint(13, 12);
		makeConstraint(2, 12);
		makeConstraint(2, 13);
		makeConstraint(3, 12);
		makeConstraint(3, 13);
		makeConstraint(3, 4);
		makeConstraint(3, 7);
		makeConstraint(3, 9);
		makeConstraint(4, 9);
		makeConstraint(4, 12);
		makeConstraint(4, 5);
		makeConstraint(5, 6);
		makeConstraint(4, 7);
		makeConstraint(7, 8);
		makeConstraint(7, 9);
		makeConstraint(7, 12);
		makeConstraint(9, 12);
		makeConstraint(9, 10);
		makeConstraint(10, 11);
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
		renderer = new ParticleNetworkRenderer(shaderProgramId, positions, constraints, numberOfParticles);

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
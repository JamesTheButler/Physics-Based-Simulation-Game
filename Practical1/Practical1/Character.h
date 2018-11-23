#pragma once
#pragma once

#include "PositionBasedObject.h"

class Character : public PositionBasedObject {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> normals;
	int numberOfParticles;
	float size;
	float armLength;

	//--------------------------------------- Private methods ----------------------------------------------------
	void initializePositions() {
		//body
		positions[0] = vec3(0.0, size, 0.0);			// A
		positions[1] = vec3(size, 0.0, 0.0);			// B
		positions[2] = vec3(0.0, -size, 0.0);			// C
		positions[3] = vec3(-size, 0.0, 0.0);			// D

		positions[4] = vec3(.6f*size, .6f*size, 0.0);		// E
		positions[5] = vec3(.6f*size, -.6f*size, 0.0);		// F
		positions[6] = vec3(-.6f*size, -.6f*size, 0.0);		// G
		positions[7] = vec3(-.6f*size, .6f*size, 0.0);		// H
		//arms
		positions[8] = vec3(0.0, armLength * size, 0.0);
		positions[9] = vec3(armLength * size, 0.0, 0.0);
		positions[10] = vec3(0.0, -armLength * size, 0.0);
		positions[11] = vec3(-armLength * size, 0.0, 0.0);
	}

	void initializeConstraints() {
		// A,B,C,D
		makeConstraint(0, 1);
		makeConstraint(0, 2);
		makeConstraint(0, 3);
		makeConstraint(0, 4);
		makeConstraint(0, 7);
		makeConstraint(1, 2);
		makeConstraint(1, 3);
		makeConstraint(1, 4);
		makeConstraint(1, 5);
		makeConstraint(2, 3);
		makeConstraint(2, 5);
		makeConstraint(2, 6);
		makeConstraint(3, 6);
		makeConstraint(3, 7);
		// E, F, G, H
		makeConstraint(4, 5);
		makeConstraint(4, 6);
		makeConstraint(4, 7);
		makeConstraint(5, 6);
		makeConstraint(5, 7);
		makeConstraint(6, 7);
		//arms
		makeConstraint(8, 0);
		makeConstraint(8, 4);
		makeConstraint(8, 7);
		makeConstraint(9, 1);
		makeConstraint(9, 4);
		makeConstraint(9, 5);
		makeConstraint(10, 2);
		makeConstraint(10, 5);
		makeConstraint(10, 6);
		makeConstraint(11, 3);
		makeConstraint(11, 6);
		makeConstraint(11, 7);

	}

public:
	//--------------------------------------- Public methods -----------------------------------------------------
	Character(IntegrationScheme integrationScheme, GLhandleARB shaderProgramId, float size, float armLength) :
		PositionBasedObject() {
		this->size = size;
		this->armLength = armLength;
		numberOfParticles = 12;

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
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
	vec3 startCenter;

	std::vector<bool> armConenctions;

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
		
		for (int i = 0; i < positions.size(); i++) {
			positions[i] += startCenter;
		}
	}

	void initializeConstraints() {
		// A,B,C,D
		PositionBasedObject::makeConstraint(0, 1);
		PositionBasedObject::makeConstraint(0, 2);
		PositionBasedObject::makeConstraint(0, 3);
		PositionBasedObject::makeConstraint(0, 4);
		PositionBasedObject::makeConstraint(0, 7);
		PositionBasedObject::makeConstraint(1, 2);
		PositionBasedObject::makeConstraint(1, 3);
		PositionBasedObject::makeConstraint(1, 4);
		PositionBasedObject::makeConstraint(1, 5);
		PositionBasedObject::makeConstraint(2, 3);
		PositionBasedObject::makeConstraint(2, 5);
		PositionBasedObject::makeConstraint(2, 6);
		PositionBasedObject::makeConstraint(3, 6);
		PositionBasedObject::makeConstraint(3, 7);
		// E, F, G, H
		PositionBasedObject::makeConstraint(4, 5);
		PositionBasedObject::makeConstraint(4, 6);
		PositionBasedObject::makeConstraint(4, 7);
		PositionBasedObject::makeConstraint(5, 6);
		PositionBasedObject::makeConstraint(5, 7);
		PositionBasedObject::makeConstraint(6, 7);
		//arms
		PositionBasedObject::makeConstraint(8, 0);
		PositionBasedObject::makeConstraint(8, 4);
		PositionBasedObject::makeConstraint(8, 7);
		PositionBasedObject::makeConstraint(9, 1);
		PositionBasedObject::makeConstraint(9, 4);
		PositionBasedObject::makeConstraint(9, 5);
		PositionBasedObject::makeConstraint(10, 2);
		PositionBasedObject::makeConstraint(10, 5);
		PositionBasedObject::makeConstraint(10, 6);
		PositionBasedObject::makeConstraint(11, 3);
		PositionBasedObject::makeConstraint(11, 6);
		PositionBasedObject::makeConstraint(11, 7);
	}

public:
	//--------------------------------------- Public methods -----------------------------------------------------
	Character(IntegrationScheme integrationScheme, GLhandleARB shaderProgramId, float size, float armLength, vec3 startCenter) :
		PositionBasedObject() {
		this->size = size;
		this->armLength = armLength;
		this->startCenter = startCenter;
		numberOfParticles = 12;

		positions.resize(numberOfParticles);
		oldPositions.resize(numberOfParticles);
		accelerations.resize(numberOfParticles, vec3(0, 0, 0));

		masses.resize(numberOfParticles, 1);
		isMovables.resize(numberOfParticles, true);
		velocities.resize(numberOfParticles, vec3(0, 0, 0));

		solver = new Solver(integrationScheme, positions, oldPositions, velocities, accelerations, masses, isMovables, constraints);
		renderer = new ParticleNetworkRenderer(shaderProgramId, positions, constraints, numberOfParticles);
		armConenctions.resize(4, false);


		initializePositions();
		for (int i = 0; i < positions.size(); i++) {
			oldPositions[i] = positions[i];
		}
		initializeConstraints();

		normals.resize(numberOfParticles, vec3(0, 0, 0));
		renderer->setupOpenGLBuffers();
		renderer->setNormals(normals);

		std::cout << "Character: created at ("<<startCenter.x<<", "<<startCenter.y<<", "<< startCenter.z <<")\n";
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

	void makeConstraint(int p1, std::vector<vec3> & positions1, std::vector<bool> & isMovables1, int p2, std::vector<vec3> & positions2, std::vector<bool> & isMovables2) {
		constraints.push_back(Constraint(p1, positions1, isMovables1, p2, positions2, isMovables2));
	}

	void applyConnectorConstraints(RopeManager* ropeMgr, float connectionThreshold) {
		//check each arm for closest particle
		for (int i = 0; i < 4; i++) {
			//if arm is not already connected
			if (!armConenctions[i]) {
				Particle closestParticle = ropeMgr->getClosestParticle(positions[i + 8], connectionThreshold);
				// make constraint between arm and rope particle, if possible
				if (closestParticle.id != -1) {
					makeConstraint(closestParticle.id,*closestParticle.positions, *closestParticle.isMovables, i + 8, positions, isMovables);
					//makeConstraint(i + 8, positions, isMovables, closestParticle.id, *closestParticle.positions, *closestParticle.isMovables);
					armConenctions[i] = true;
					//if (constraints[constraints.size()-1].isConnector())
					std::cout << "made constraint\n";
				}
			}
		}
	}

	void removeConnectorConstraints() {
		int size = constraints.size();
		if (size > 32) {
			constraints.erase(constraints.begin() + 32, constraints.end());
			armConenctions.resize(4, false);
		}
	}
};
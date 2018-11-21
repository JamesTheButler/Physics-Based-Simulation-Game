#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Constraint.h"

enum IntegrationScheme { verlet };

class Solver {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> & positions;
	std::vector<vec3> & oldPositions;
	std::vector<vec3> & velocities;
	std::vector<vec3> & accelerations;
	std::vector<SCALAR> & masses;
	std::vector<bool> & isMovables;

	std::vector<Constraint> & constraints;

	bool firstTimeStep = true;
	IntegrationScheme integrationScheme;

	int constraintIterations;
	SCALAR dragConstant;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	Solver(IntegrationScheme integrationScheme,
		std::vector<vec3> & positions,
		std::vector<vec3> & oldPositions,
		std::vector<vec3> & velocities,
		std::vector<vec3> & accelerations,
		std::vector<SCALAR> & masses,
		std::vector<bool> & isMovables,
		std::vector<Constraint> & constraints) :
		positions(positions),
		oldPositions(oldPositions),
		velocities(velocities),
		accelerations(accelerations),
		masses(masses),
		isMovables(isMovables),
		constraints(constraints) {

		this->integrationScheme = integrationScheme;
	}

	void evaluateVerlet(SCALAR timeStepSize, bool dragEnabled) {
		if (firstTimeStep) {
			//Integration:
			for (int i = 0; i < positions.size(); i++) {
				if (isMovables[i]) {
					velocities[i] = velocities[i] + accelerations[i] * timeStepSize;
					positions[i] = positions[i] + velocities[i] * timeStepSize;
					accelerations[i] = vec3(0, 0, 0);
				}
			}
			firstTimeStep = false;
		}

		else {
			//Integration:
			for (int i = 0; i < positions.size(); i++) {
				if (isMovables[i]) {
					if (dragEnabled) {
						//write your code here...
					}
					vec3 temp = positions[i];
					positions[i] = positions[i] + positions[i] - oldPositions[i] + accelerations[i] * pow(timeStepSize, 2);
					oldPositions[i] = temp;
					accelerations[i] = vec3(0, 0, 0);
				}
			}
		}

		//Constraint solving:
		for (int i = 0; i < constraintIterations; i++) {
			for (Constraint constraint : constraints) {
				constraint.solveConstraints();
			}
		}

		//Velocity correction:
		for (int i = 0; i < positions.size(); i++) {
			if (isMovables[i]) {
				velocities[i] = (positions[i] - oldPositions[i] ) / timeStepSize;
			}
		}
	}

	void setDragConstant(int dragConstant) {
		this->dragConstant = dragConstant;
	}

	void setConstraintIterations(int constraintIterations) {
		this->constraintIterations = constraintIterations;
	}

	void setIntegrationScheme(IntegrationScheme integrationScheme) {
		this->integrationScheme = integrationScheme;
	}

	void setToFirstTimeStep() {
		firstTimeStep = true;
	}
};
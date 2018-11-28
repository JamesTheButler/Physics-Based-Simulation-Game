#pragma once

#include "Solver.h"
#include "Renderer.h"

#ifdef DOUBLE_PRECISION
typedef glm::dvec3 vec3;
#define SCALAR double
#define GL_SCALAR GL_DOUBLE
#else
typedef glm::vec3 vec3;
#define SCALAR float
#define GL_SCALAR GL_FLOAT
#endif


class PositionBasedObject {
protected:
	//The cloth consists of a grid of particles. The particle properties are stored in the following vectors:
	std::vector<vec3> positions;
	std::vector<vec3> oldPositions;
	std::vector<vec3> velocities;
	std::vector<vec3> accelerations;
	std::vector<SCALAR> masses;
	std::vector<bool> isMovables;
	std::vector<Constraint> constraints; // constraints between the particles

	//Method for calculating the triangle normal:
	vec3 calcTriangleNormal(int p1, int p2, int p3) {
		vec3 pos1 = positions[p1];
		vec3 pos2 = positions[p2];
		vec3 pos3 = positions[p3];

		vec3 v1 = pos2 - pos1;
		vec3 v2 = pos3 - pos1;

		return glm::cross(v1, v2);
	}

	//Method used by windForce() to calculate the wind force for a single triangle:
	void addWindForcesToTriangle(int p1, int p2, int p3, const vec3 direction) {
		vec3 normal = calcTriangleNormal(p1, p2, p3);
		vec3 d = glm::normalize(normal);
		vec3 force = normal * (glm::dot(d, direction));

		accelerations[p1] += force / masses[p1];
		accelerations[p2] += force / masses[p2];
		accelerations[p3] += force / masses[p3];
	}

	void makeConstraint(int p1, int p2) {
		constraints.push_back(Constraint(p1, p2, positions, isMovables));
	}


	void makeConstraint(int p1, std::vector<vec3> & positions1, std::vector<bool> & isMovables1, int p2, std::vector<vec3> & positions2, std::vector<bool> & isMovables2) {
		constraints.push_back(Constraint(p1, positions1, isMovables1, p2, positions2, isMovables2));
	}

	void makeConstraint(int p1, std::vector<vec3> & positions1, std::vector<bool> & isMovables1, int p2, std::vector<vec3> & positions2, std::vector<bool> & isMovables2, SCALAR restDist) {
		constraints.push_back(Constraint(p1, positions1, isMovables1, p2, positions2, isMovables2, restDist));
	}

public:
	Solver * solver;
	Renderer * renderer;
	
	PositionBasedObject() {
	}

	~PositionBasedObject() {
		delete(solver);
		delete(renderer);
	}

	//Advance the simulation one time step:
	void timeStep(SCALAR timeStepSize, bool dragEnabled) {
		solver->evaluateVerlet(timeStepSize, dragEnabled);
	}
	
	//Adds a force uniformly to all particles:
	void addForce(const vec3 direction) {
		for (int i = 0; i < accelerations.size(); i++) {
			accelerations[i] += direction / masses[i];
		}
	}

	virtual void reinitialize(IntegrationScheme integrationScheme) = 0;
};
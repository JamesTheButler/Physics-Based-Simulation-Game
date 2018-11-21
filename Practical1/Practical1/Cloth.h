#pragma once

#include "PositionBasedObject.h"

class Cloth : public PositionBasedObject {
private:
	//--------------------------------------- Private member variables -------------------------------------------
	int width, height;

	const int numParticlesWidth;
	const int numParticlesHeight;  //total number of particles is num_particles_width*num_particles_height

	std::vector<vec3> accumulatedNormals;

	//--------------------------------------- Private methods ----------------------------------------------------
	inline int getIndex(int x, int y) { return y * numParticlesWidth + x; }

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	Cloth(float width, float height, const int numParticlesWidth, const int numParticlesHeight, IntegrationScheme integrationScheme, GLhandleARB shaderProgramId) :
		PositionBasedObject(), numParticlesWidth(numParticlesWidth), numParticlesHeight(numParticlesHeight) {

		this->width = width;
		this->height = height;

		positions.resize(numParticlesWidth*numParticlesHeight);
		oldPositions.resize(numParticlesWidth*numParticlesHeight);
		accelerations.resize(numParticlesWidth*numParticlesHeight, vec3(0, 0, 0));

		masses.resize(numParticlesWidth*numParticlesHeight, 1);
		isMovables.resize(numParticlesWidth*numParticlesHeight, true);
		velocities.resize(numParticlesWidth*numParticlesHeight, vec3(0, 0, 0));

		solver = new Solver(integrationScheme, positions, oldPositions, velocities, accelerations, masses, isMovables, constraints);
		renderer = new Renderer(shaderProgramId, positions, constraints, trianglesIndices, numParticlesWidth*numParticlesHeight);

		// Creating particles in a grid from (0, 0, 0) to (width, -height, 0):
		for (int x = 0; x < numParticlesWidth; x++) {
			for (int y = 0; y < numParticlesHeight; y++) {
				vec3 pos = vec3(width * (x / static_cast<float>(numParticlesWidth)), -height *
					(y / static_cast<float>(numParticlesHeight)), 0);

				// insert particle in column x at y'th row:
				positions[y*numParticlesWidth + x] = pos;
				oldPositions[y*numParticlesWidth + x] = pos;
			}
		}

		// Connecting immediate neighbor particles with constraints (distance 1 and sqrt(2) in the grid):
		for (int x = 0; x < numParticlesWidth; x++) {
			for (int y = 0; y < numParticlesHeight; y++) {
				if (x < numParticlesWidth - 1)
					makeConstraint(getIndex(x, y), getIndex(x + 1, y));

				if (y < numParticlesHeight - 1)
					makeConstraint(getIndex(x, y), getIndex(x, y + 1));

				if (x < numParticlesWidth - 1 && y < numParticlesHeight - 1)
					makeConstraint(getIndex(x, y), getIndex(x + 1, y + 1));

				if (x < numParticlesWidth - 1 && y < numParticlesHeight - 1)
					makeConstraint(getIndex(x + 1, y), getIndex(x, y + 1));
			}
		}

		// Connecting secondary neighbors with constraints (distance 2 and sqrt(4) in the grid):
		for (int x = 0; x < numParticlesWidth; x++) {
			for (int y = 0; y < numParticlesHeight; y++) {
				if (x < numParticlesWidth - 2)
					makeConstraint(getIndex(x, y), getIndex(x + 2, y));

				if (y < numParticlesHeight - 2)
					makeConstraint(getIndex(x, y), getIndex(x, y + 2));

				if (x < numParticlesWidth - 2 && y < numParticlesHeight - 2)
					makeConstraint(getIndex(x, y), getIndex(x + 2, y + 2));

				if (x < numParticlesWidth - 2 && y < numParticlesHeight - 2)
					makeConstraint(getIndex(x + 2, y), getIndex(x, y + 2));
			}
		}

		// making the upper left most three and right most three particles unmovable:
		for (int i = 0; i < 3; i++) {
			isMovables[getIndex(0 + i, 0)] = false;
			isMovables[getIndex(numParticlesWidth - 1 - i, 0)] = false;
		}

		for (int x = 0; x < numParticlesWidth - 1; x++) {
			for (int y = 0; y < numParticlesHeight - 1; y++) {
				trianglesIndices.push_back(getIndex(x + 1, y));
				trianglesIndices.push_back(getIndex(x, y));
				trianglesIndices.push_back(getIndex(x, y + 1));

				trianglesIndices.push_back(getIndex(x + 1, y + 1));
				trianglesIndices.push_back(getIndex(x + 1, y));
				trianglesIndices.push_back(getIndex(x, y + 1));
			}
		}

		accumulatedNormals.resize(numParticlesWidth*numParticlesHeight, vec3(0, 0, 0));

		renderer->setupOpenGLBuffers();

		renderer->setNormals(accumulatedNormals);
	}

	void reinitialize(IntegrationScheme integrationScheme) {
		solver->setIntegrationScheme(integrationScheme);
		solver->setToFirstTimeStep();

		std::fill(accelerations.begin(), accelerations.end(), vec3(0, 0, 0));
		std::fill(velocities.begin(), velocities.end(), vec3(0, 0, 0));

		oldPositions.resize(numParticlesWidth*numParticlesHeight);

		// Creating particles in a grid from (0, 0, 0) to (width, -height, 0):
		for (int x = 0; x < numParticlesWidth; x++) {
			for (int y = 0; y < numParticlesHeight; y++) {
				vec3 pos = vec3(width * (x / (float)numParticlesWidth), -height * (y / (float)numParticlesHeight), 0);

				// insert particle in column x at y'th row:
				positions[y*numParticlesWidth + x] = pos;
				oldPositions[y*numParticlesWidth + x] = pos;
			}
		}
	}

	void calculateNormals() {
		std::fill(accumulatedNormals.begin(), accumulatedNormals.end(), vec3(0));

		//Create smooth per-particle normals by adding up all the triangle normals that each particle is part of:
		for (int x = 0; x < numParticlesWidth - 1; x++) {
			for (int y = 0; y < numParticlesHeight - 1; y++) {
				vec3 normal = calcTriangleNormal(getIndex(x + 1, y), getIndex(x, y), getIndex(x, y + 1));
				accumulatedNormals[getIndex(x + 1, y)] += glm::normalize(normal);
				accumulatedNormals[getIndex(x, y)] += glm::normalize(normal);
				accumulatedNormals[getIndex(x, y + 1)] += glm::normalize(normal);

				normal = calcTriangleNormal(getIndex(x + 1, y + 1), getIndex(x + 1, y), getIndex(x, y + 1));
				accumulatedNormals[getIndex(x + 1, y + 1)] += glm::normalize(normal);
				accumulatedNormals[getIndex(x + 1, y)] += glm::normalize(normal);
				accumulatedNormals[getIndex(x, y + 1)] += glm::normalize(normal);
			}
		}

		renderer->setNormals(accumulatedNormals);
	}

	//Adds wind forces to all particles. The force is added for each triangle since the final force is proportional to the triangle area as seen from the wind direction:
	void windForce(const vec3 direction) {
		for (int x = 0; x < numParticlesWidth - 1; x++) {
			for (int y = 0; y < numParticlesHeight - 1; y++) {
				addWindForcesToTriangle(getIndex(x + 1, y), getIndex(x, y), getIndex(x, y + 1), direction);
				addWindForcesToTriangle(getIndex(x + 1, y + 1), getIndex(x + 1, y), getIndex(x, y + 1), direction);
			}
		}
	}
};

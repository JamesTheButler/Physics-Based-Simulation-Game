#pragma once

#include <vector>
#include <glm/glm.hpp>

//#define DOUBLE_PRECISION			//Uncomment this line to switch to double precision

#ifdef DOUBLE_PRECISION
typedef glm::dvec3 vec3;
#define SCALAR double
#define GL_SCALAR GL_DOUBLE
#else
typedef glm::vec3 vec3;
#define SCALAR float
#define GL_SCALAR GL_FLOAT
#endif

enum IntegrationScheme { forwardEuler, semiImplicitEuler, leapfrog, verlet };
const SCALAR FORWARD_INITIAL_EULER_TIME_STEP_SIZE = 0.001;
const SCALAR SEMI_IMPLICIT_EULER_INITIAL_TIME_STEP_SIZE = 0.001;
const SCALAR LEAPFROG_INITIAL_TIME_STEP_SIZE = 0.001;
const SCALAR VERLET_INITIAL_TIME_STEP_SIZE = 0.001;

const SCALAR INITIAL_SPRING_STIFFNESS = 4000.0;

SCALAR timeStepSizes[] = {
	FORWARD_INITIAL_EULER_TIME_STEP_SIZE,
	SEMI_IMPLICIT_EULER_INITIAL_TIME_STEP_SIZE,
	LEAPFROG_INITIAL_TIME_STEP_SIZE,
	VERLET_INITIAL_TIME_STEP_SIZE
};

class Spring {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	SCALAR restDistance; // the length between particle p1 and p2 in rest configuration
	int p1, p2; // the two particles that are connected through this spring
	std::vector<vec3> & positions;
	std::vector<vec3> & accelerations;
	std::vector<bool> & isMovables;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	Spring(int p1, int p2, std::vector<vec3> & positions, std::vector<vec3> & accelerations, std::vector<bool> & 
		isMovables) : positions(positions), accelerations(accelerations), isMovables(isMovables) {

		this->p1 = p1;
		this->p2 = p2;

		vec3 vec = positions[p1] - positions[p2];
		restDistance = glm::length(vec);
	}

	//This method is called by Cloth.timeStep():
	void addSpringForcesToParticles(SCALAR springStiffness) {
		vec3 p1ToP2 = positions[p2] - positions[p1]; // vector from p1 to p2
		SCALAR currentDistance = glm::length(p1ToP2); // current distance between p1 and p2
		vec3 force = springStiffness * (currentDistance - restDistance) * (p1ToP2 / currentDistance);
		
		if (isMovables[p1])
			accelerations[p1] += force;
		if (isMovables[p2])
			accelerations[p2] -= force;
	}

	inline int getP1() { return p1; }
	inline int getP2() { return p2; }
};

class Cloth {

private:

	//--------------------------------------- Private member variables -------------------------------------------
	int numParticlesWidth, numParticlesHeight;  //total number of particles is num_particles_width*num_particles_height

	//The cloth consists of a grid of particles. The particle properties are stored in the following vectors:
	std::vector<vec3> positions;
	std::vector<vec3> oldPositions;      //used only for verlet integration
	std::vector<vec3> velocities;
	std::vector<vec3> accelerations;
	std::vector<SCALAR> masses;
	std::vector<bool> isMovables;

	std::vector<vec3> accumulatedNormals;

	std::vector<Spring> springs; // springs between the particles
	SCALAR springStiffness;

	IntegrationScheme integrationScheme;

	int width, height;

	bool firstTimeStep = true;

	//OpenGL related member variables:
	std::vector<unsigned int> trianglesIndices;
	GLhandleARB shaderProgramId;
	GLuint vertexPosBufferHandle, vertexNormalBufferHandle, trianglesIndexBufferHandle, linesIndexBufferHandle;
	GLuint vertexPosAttribLocation, vertexNormalAttribLocation;
	GLint colorLocation;


	//--------------------------------------- Private methods ----------------------------------------------------
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

	inline int getIndex(int x, int y) { return y * numParticlesWidth + x; }
	inline void makeSpring(int p1, int p2, std::vector<vec3> & positions, std::vector<bool> & isMovables) { 
		springs.push_back(Spring(p1, p2, positions, accelerations, isMovables)); 
	}


	void evaluateForwardEuler(SCALAR timeStepSize) {
		for (Spring spring : springs) {
			spring.addSpringForcesToParticles(springStiffness); // evaluate and add spring forces to connected particles
		}

		for (int i = 0; i < positions.size(); i++) {
			if (isMovables[i]) {
				positions[i] = positions[i] + velocities[i] * timeStepSize;
				velocities[i] = velocities[i] + accelerations[i] * timeStepSize;
				accelerations[i] = vec3(0, 0, 0);
			}
		}
	}

	void evaluateSemiImplicitEuler(SCALAR timeStepSize) {
		for (Spring spring : springs) {
			spring.addSpringForcesToParticles(springStiffness); // evaluate and add spring forces to connected particles
		}

		for (int i = 0; i < positions.size(); i++) {
			if (isMovables[i]) {
				//write your code here...
			}
		}
	}

	void evaluateVerlet(SCALAR timeStepSize) {
		for (Spring spring : springs) {
			spring.addSpringForcesToParticles(springStiffness); // evaluate and add spring forces to connected particles
		}

		if (firstTimeStep) {
			//Special case for the first time step:
			for (int i = 0; i < positions.size(); i++) {
				if (isMovables[i]) {
					//write your code here...
				}
			}
		}

		else {
			for (int i = 0; i < positions.size(); i++) {
				if (isMovables[i]) {
					//write your code here...
				}
			}
		}
	}

	void evaluateLeapfrog(SCALAR timeStepSize) {
		if (firstTimeStep) {
			//Special case for the first time step:
			for (Spring spring : springs) {
				spring.addSpringForcesToParticles(springStiffness); // evaluate and add spring forces to connected particles
			}

			for (int i = 0; i < positions.size(); i++) {
				if (isMovables[i]) {
					//write your code here...
				}
			}
		}

		else {
			for (int i = 0; i < positions.size(); i++) {
				if (isMovables[i]) {
					//write your code here...
				}
			}	

			for (Spring spring : springs) {
				spring.addSpringForcesToParticles(springStiffness); // evaluate and add spring forces to connected particles
			}

			for (int i = 0; i < positions.size(); i++) {
				if (isMovables[i]) {
					//write your code here...
				}
			}
		}
	}

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	Cloth(float width, float height, int numParticlesWidth, int numParticlesHeight, 
		IntegrationScheme integrationScheme, GLhandleARB shaderProgramId) {

		this->width = width;
		this->height = height;
		this->numParticlesWidth = numParticlesWidth;
		this->numParticlesHeight = numParticlesHeight;
		this->integrationScheme = integrationScheme;
		this->shaderProgramId = shaderProgramId;

		springStiffness = INITIAL_SPRING_STIFFNESS;

		positions.resize(numParticlesWidth*numParticlesHeight);
		accelerations.resize(numParticlesWidth*numParticlesHeight, vec3(0, 0, 0));
		accumulatedNormals.resize(numParticlesWidth*numParticlesHeight, vec3(0,0,0));
		masses.resize(numParticlesWidth*numParticlesHeight, 1);
		isMovables.resize(numParticlesWidth*numParticlesHeight, true);
		velocities.resize(numParticlesWidth*numParticlesHeight, vec3(0, 0, 0));

		if (integrationScheme == verlet) {
			oldPositions.resize(numParticlesWidth*numParticlesHeight);
		}

		// Creating particles in a grid from (0, 0, 0) to (width, -height, 0):
		for (int x = 0; x < numParticlesWidth; x++) {
			for (int y = 0; y < numParticlesHeight; y++) {
				vec3 pos = vec3(width * (x / static_cast<float>(numParticlesWidth)), -height * 
					(y / static_cast<float>(numParticlesHeight)), 0);

				// insert particle in column x at y'th row:
				positions[y*numParticlesWidth + x] = pos;  
				if (integrationScheme == verlet)
					oldPositions[y*numParticlesWidth + x] = pos;
			}
		}

		// Connecting immediate neighbor particles with springs (distance 1 and sqrt(2) in the grid):
		for (int x = 0; x < numParticlesWidth; x++) {
			for (int y = 0; y < numParticlesHeight; y++) {
				if (x < numParticlesWidth - 1) 
					makeSpring(getIndex(x, y), getIndex(x + 1, y), positions, isMovables);

				if (y < numParticlesHeight - 1) 
					makeSpring(getIndex(x, y), getIndex(x, y + 1), positions, isMovables);

				if (x < numParticlesWidth - 1 && y < numParticlesHeight - 1) 
					makeSpring(getIndex(x, y), getIndex(x + 1, y + 1), positions, isMovables);

				if (x < numParticlesWidth - 1 && y < numParticlesHeight - 1) 
					makeSpring(getIndex(x + 1, y), getIndex(x, y + 1), positions, isMovables);
			}
		}

		// Connecting secondary neighbors with springs (distance 2 and sqrt(4) in the grid):
		for (int x = 0; x < numParticlesWidth; x++) {
			for (int y = 0; y < numParticlesHeight; y++) {
				if (x < numParticlesWidth - 2) 
					makeSpring(getIndex(x, y), getIndex(x + 2, y), positions, isMovables);

				if (y < numParticlesHeight - 2) 
					makeSpring(getIndex(x, y), getIndex(x, y + 2), positions, isMovables);

				if (x < numParticlesWidth - 2 && y < numParticlesHeight - 2) 
					makeSpring(getIndex(x, y), getIndex(x + 2, y + 2), positions, isMovables);

				if (x < numParticlesWidth - 2 && y < numParticlesHeight - 2) 
					makeSpring(getIndex(x + 2, y), getIndex(x, y + 2), positions, isMovables);
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

		glGenBuffers(1, &trianglesIndexBufferHandle);
		glGenBuffers(1, &linesIndexBufferHandle);
		glGenBuffers(1, &vertexPosBufferHandle);
		glGenBuffers(1, &vertexNormalBufferHandle);

		vertexPosAttribLocation = glGetAttribLocation(shaderProgramId, "vertexPos");
		vertexNormalAttribLocation = glGetAttribLocation(shaderProgramId, "vertexNormal");

		colorLocation = glGetUniformLocation(shaderProgramId, "color");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trianglesIndexBufferHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, trianglesIndices.size() * sizeof(unsigned int), 
			&(trianglesIndices[0]), GL_STATIC_DRAW);

		std::vector<unsigned int> springVec;
		for (Spring spring : springs) {
			springVec.push_back(spring.getP1());
			springVec.push_back(spring.getP2());
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesIndexBufferHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, springVec.size() * sizeof(unsigned int), 
			&(springVec[0]), GL_STATIC_DRAW);

	}

	void reinitialize(IntegrationScheme integrationScheme) {
		this->integrationScheme = integrationScheme;

		firstTimeStep = true;
		springStiffness = INITIAL_SPRING_STIFFNESS;
			   
		std::fill(accelerations.begin(), accelerations.end(), vec3(0, 0, 0));
		std::fill(velocities.begin(), velocities.end(), vec3(0, 0, 0));

		if (integrationScheme == verlet) {
			oldPositions.resize(numParticlesWidth*numParticlesHeight);
		}
		else {
			oldPositions.resize(0);
		}

		// Creating particles in a grid from (0, 0, 0) to (width, -height, 0):
		for (int x = 0; x < numParticlesWidth; x++) {
			for (int y = 0; y < numParticlesHeight; y++) {
				vec3 pos = vec3(width * (x / (float)numParticlesWidth), -height * (y / (float)numParticlesHeight), 0);

				// insert particle in column x at y'th row:
				positions[y*numParticlesWidth + x] = pos;
				if (integrationScheme == verlet)
					oldPositions[y*numParticlesWidth + x] = pos;
			}
		}
	}

	/*The cloth consists of triangles pairs of four particles in the grid as follows:

	(x,y)   *--* (x+1,y)
			| /|
			|/ |
	(x,y+1) *--* (x+1,y+1)

	*/
	void draw(bool renderParticlesAndSprings) {
		for (vec3 & accumulatedNormal : accumulatedNormals) {
			accumulatedNormal = vec3();
		}

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

		glEnableVertexAttribArray(vertexPosAttribLocation);
		glEnableVertexAttribArray(vertexNormalAttribLocation);

		glBindBuffer(GL_ARRAY_BUFFER, vertexPosBufferHandle);
		glVertexAttribPointer(vertexPosAttribLocation, 3, GL_SCALAR, GL_FALSE, 0, NULL);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &(positions[0]), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBufferHandle);
		glVertexAttribPointer(vertexNormalAttribLocation, 3, GL_SCALAR, GL_FALSE, 0, NULL);
		glBufferData(GL_ARRAY_BUFFER, accumulatedNormals.size() * sizeof(vec3), 
			&(accumulatedNormals[0]), GL_DYNAMIC_DRAW);

		if (renderParticlesAndSprings) {
			//Render the particles and springs of the cloth:
			glUniform4f(colorLocation, 0, 1, 0, 1);
			glDrawArrays(GL_POINTS, 0, numParticlesWidth*numParticlesHeight);

			glUniform4f(colorLocation, 1, 1, 1, 1);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesIndexBufferHandle);
			glDrawElements(GL_LINES, springs.size() * 2, GL_UNSIGNED_INT, NULL);			
		}
		else {
			//Render the cloth as a triangle mesh:
			glUniform4f(colorLocation, 1, 0, 0, 1);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trianglesIndexBufferHandle);
			glDrawElements(GL_TRIANGLES, trianglesIndices.size(), GL_UNSIGNED_INT, NULL);
		}
	}

	//Advance the simulation one time step:
	void timeStep(SCALAR timeStepSize) {
		switch (integrationScheme) {
		case verlet:
			evaluateVerlet(timeStepSize);
			break;
		case semiImplicitEuler:
			evaluateSemiImplicitEuler(timeStepSize);
			break;
		case forwardEuler:
			evaluateForwardEuler(timeStepSize);
			break;
		case leapfrog:
			evaluateLeapfrog(timeStepSize);
			break;
		}

		firstTimeStep = false;
	}

	//Adds a force uniformly to all particles:
	void addForce(const vec3 direction) {
		for (int i = 0; i < accelerations.size(); i++) {
			accelerations[i] += direction / masses[i];
		}
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

	inline void changeStiffness(SCALAR delta) {
		springStiffness += delta;
	}

	inline SCALAR getSpringStiffness() { return springStiffness; }
};
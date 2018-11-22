#pragma once

#ifdef DOUBLE_PRECISION
typedef glm::dvec3 vec3;
#define SCALAR double
#define GL_SCALAR GL_DOUBLE
#else
typedef glm::vec3 vec3;
#define SCALAR float
#define GL_SCALAR GL_FLOAT
#endif

class Constraint {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	SCALAR restDistance; // the length between particle p1 and p2 in rest configuration
	int p1, p2; // the two particles that are connected through this constraint
	std::vector<vec3> & positions;
	std::vector<bool> & isMovables;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	Constraint(int p1, int p2, std::vector<vec3> & positions, std::vector<bool> &
		isMovables) : positions(positions), isMovables(isMovables) {

		this->p1 = p1; //particle index 1
		this->p2 = p2; //particle index 2

		vec3 vec = positions[p1] - positions[p2];
		restDistance = glm::length(vec);
	}

	void solveConstraints() {
		glm::vec3 vec = positions[p1] - positions[p2];

		if (isMovables[p1])
			positions[p1] += -0.5f *(glm::length(vec) - restDistance) * glm::normalize(vec);
		if (isMovables[p2])
			positions[p2] += 0.5f *(glm::length(vec) - restDistance) * glm::normalize(vec);
	}


	inline int getP1() { return p1; }
	inline int getP2() { return p2; }
};
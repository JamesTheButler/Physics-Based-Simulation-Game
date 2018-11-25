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

class ConnectorConstraint{
private:

	//--------------------------------------- Private member variables -------------------------------------------
	SCALAR restDistance; // the length between particle p1 and p2 in rest configuration
	int p1, p2; // the two particles that are connected through this constraint
	std::vector<vec3> & positions;
	std::vector<vec3> & positions2;
	std::vector<bool> & isMovables;
	std::vector<bool> & isMovables2;
public:
	//--------------------------------------- Public methods -----------------------------------------------------
	ConnectorConstraint(int p1, std::vector<vec3> & positions, std::vector<bool> & isMovables, int p2, std::vector<vec3> & positions2, std::vector<bool> & isMovables2)
		: positions(positions), positions2(positions2), isMovables(isMovables), isMovables2(isMovables2) {

		this->p1 = p1;
		this->p2 = p2;

		vec3 vec = positions[p1] - positions[p2];
		restDistance = glm::length(vec);
	}

	void solveConstraints() {
		glm::vec3 vec = positions[p1] - positions2[p2];

		if (isMovables[p1])
			positions[p1] += -0.5f *(glm::length(vec) - restDistance) * glm::normalize(vec);
		if (isMovables2[p2])
			positions2[p2] += 0.5f *(glm::length(vec) - restDistance) * glm::normalize(vec);
	}


	inline int getP1() { return p1; }
	inline int getP2() { return p2; }
};
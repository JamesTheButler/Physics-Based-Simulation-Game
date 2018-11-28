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
	SCALAR restDistance; // the length between particle p1 and p2 in rest configuration
	int p1, p2; // the two particles that are connected through this constraint
	std::vector<vec3> & positions1, & positions2;
	std::vector<bool> & isMovables1, & isMovables2;

public:
	Constraint(int p1, int p2, std::vector<vec3> & positions, std::vector<bool> & isMovables)
		: positions1(positions), isMovables1(isMovables),
		positions2(positions), isMovables2(isMovables) {

		this->p1 = p1;
		this->p2 = p2;

		vec3 vec = positions1[p1] - positions2[p2];
		restDistance = glm::length(vec);
	}



	Constraint(	int p1, std::vector<vec3> & positions1, std::vector<bool> & isMovables1, 
				int p2, std::vector<vec3> & positions2, std::vector<bool> & isMovables2)
				:	positions1(positions1), isMovables1(isMovables1),
					positions2(positions2), isMovables2(isMovables2) {

		this->p1 = p1;
		this->p2 = p2;

		vec3 vec = positions1[p1] - positions2[p2];
		restDistance = glm::length(vec);
	}

	Constraint(int p1, std::vector<vec3> & positions1, std::vector<bool> & isMovables1,
		int p2, std::vector<vec3> & positions2, std::vector<bool> & isMovables2, SCALAR restDistance)
		: positions1(positions1), isMovables1(isMovables1),
		positions2(positions2), isMovables2(isMovables2) {

		this->p1 = p1;
		this->p2 = p2;
		this->restDistance = restDistance;
	}

	Constraint& operator=(const Constraint& c) {
		this->p1 = c.p1;
		this->p2 = c.p2;
		this->positions1 = c.positions1;
		this->positions2 = c.positions2;
		this->isMovables1 = c.isMovables1;
		this->isMovables2 = c.isMovables2;
		return *this;
	}

	void solveConstraints() {
		glm::vec3 vec = positions1[p1] - positions2[p2];
		if (isMovables1[p1])
			positions1[p1] += - 0.5f *(glm::length(vec) - restDistance) * glm::normalize(vec);
		if(isMovables2[p2])
			positions2[p2] += 0.5f *(glm::length(vec) - restDistance) * glm::normalize(vec);
	}


	inline int getP1() { return p1; }
	inline int getP2() { return p2; }

	bool isConnector() {
		if (positions1 != positions2)
			return true;
		return false;
	}
};
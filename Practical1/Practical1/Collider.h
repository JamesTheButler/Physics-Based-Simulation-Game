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

class Collider {
private:
	bool active = true;

public: 
	Renderer * renderer;

	Collider() {}

	~Collider() {
		delete(renderer);
	}

	virtual void handleCollision(vec3 & particlePosition) {}

	bool isActive() {
		return active;
	}

	void setActive(bool active) {
		this->active = active;
	}
};
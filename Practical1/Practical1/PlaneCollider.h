#pragma once

#include "Collider.h"

class PlaneCollider : public Collider {
private:
	vec3 normal;
	vec3 position;

public:
	PlaneCollider(vec3 position, vec3 normal, GLhandleARB shaderProgramId) : Collider() {
		this->position = position;
		this->normal = normal;

		renderer = new PlaneRenderer(shaderProgramId, position, -normal);
		renderer->setupOpenGLBuffers();
	}

	void handleCollision(vec3 & particlePosition) {
		float dot = glm::dot((particlePosition - position), normal);
		if (dot < 0) {
			particlePosition -= normal * dot;
		}
	}

	vec3 getPosition() {
		return position;
	}

	void setPosition(vec3 position) {
		this->position = position;
	}
};
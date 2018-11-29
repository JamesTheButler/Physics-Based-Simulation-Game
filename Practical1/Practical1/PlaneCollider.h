#pragma once

#include "Collider.h"

class PlaneCollider : public Collider {
private:
	vec3 normal;
	vec3 position;
	bool isGameEndTrigger;
	bool isGameFailed;

public:
	PlaneCollider(vec3 position, vec3 normal, GLhandleARB shaderProgramId) : Collider() {
		this->position = position;
		this->normal = normal;
		this->isGameEndTrigger = isGameEndTrigger;
		
		this->isGameEndTrigger = false;
		this->isGameFailed = false;

		renderer = new PlaneRenderer(shaderProgramId, position, -normal);
		renderer->setupOpenGLBuffers();
	}

	PlaneCollider(vec3 position, vec3 normal, GLhandleARB shaderProgramId, bool isGameEndTrigger) : Collider() {
		this->position = position;
		this->normal = normal;
		this->isGameEndTrigger = isGameEndTrigger;

		renderer = new PlaneRenderer(shaderProgramId, position, -normal);
		renderer->setupOpenGLBuffers();
	}

	void handleCollision(vec3 & particlePosition) {
		float dot = glm::dot((particlePosition - position), normal);
		if (dot < 0) {
			particlePosition -= normal * dot;
			if (isGameEndTrigger && !isGameFailed) {
				std::cout << "!!! Game Lost !!!\n";
				isGameFailed = true;
			}
		}
	}

	vec3 getPosition() {
		return position;
	}

	void setPosition(vec3 position) {
		this->position = position;
	}
};
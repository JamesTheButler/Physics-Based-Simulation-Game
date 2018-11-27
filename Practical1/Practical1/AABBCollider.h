#pragma once

#include "Collider.h"

class AABBCollider : public Collider {
private:
	vec3 position;
	float width, height;

public:
	AABBCollider(vec3 position, float width, float height, GLhandleARB shaderProgramId) : Collider() {
		this->position = position;
		this->width = width;
		this->height = height;

		renderer = new AABBRenderer(shaderProgramId, position, width, height);
		renderer->setupOpenGLBuffers();
	}

	void handleCollision(vec3 & particlePosition) {
		float xDist, yDist;
		xDist = particlePosition.x - position.x;
		yDist = particlePosition.y - position.y;
		if (abs(xDist) < width / 2 && abs(yDist) < height / 2) {
			if (abs(xDist) <= abs(yDist))
				particlePosition.x = position.x + xDist / abs(xDist) * width*0.5f;
			else
				particlePosition.y = position.y + yDist / abs(yDist) * height*0.5f;
		}

	}

	vec3 getPosition() {
		return position;
	}

	void setPosition(vec3 position) {
		this->position = position;
	}
};
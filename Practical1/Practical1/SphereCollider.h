#pragma once

#include "Collider.h"

class SphereCollider : public Collider {

private:

	//--------------------------------------- Private member variables -------------------------------------------
	vec3 position;
	SCALAR radius;
	const float OFFSET = 0.05f;

public:
	
	//--------------------------------------- Public methods -----------------------------------------------------
	SphereCollider(vec3 position, SCALAR radius, GLhandleARB shaderProgramId) : Collider() {
		this->position = position;
		this->radius = radius;

		renderer = new SphereRenderer(shaderProgramId, radius);

		renderer->setupOpenGLBuffers();
	}

	void handleCollision(vec3 & particlePosition) {
		if(glm::distance(particlePosition, position)< radius)
			particlePosition = position + (radius+OFFSET) * glm::normalize(particlePosition - position);
	}

	vec3 getPosition() {
		return position;
	}

	void setPosition(vec3 position) {
		this->position = position;
	}
};
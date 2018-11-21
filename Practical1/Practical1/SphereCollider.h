#pragma once

#include "Collider.h"

class SphereCollider : public Collider {

private:

	//--------------------------------------- Private member variables -------------------------------------------
	vec3 position;
	SCALAR radius;

public:
	
	//--------------------------------------- Public methods -----------------------------------------------------
	SphereCollider(vec3 position, SCALAR radius, GLhandleARB shaderProgramId) : Collider() {
		this->position = position;
		this->radius = radius;

		renderer = new SphereRenderer(shaderProgramId, radius);

		renderer->setupOpenGLBuffers();
	}

	void handleCollision(vec3 & particlePosition) {
		//write your code here...
	}

	vec3 getPosition() {
		return position;
	}

	void setPosition(vec3 position) {
		this->position = position;
	}
};
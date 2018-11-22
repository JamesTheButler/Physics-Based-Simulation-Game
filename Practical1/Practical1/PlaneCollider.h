#pragma once

#include "Collider.h"

class PlaneCollider : public Collider {

private:

	//--------------------------------------- Private member variables -------------------------------------------
	vec3 normal;
	vec3 position;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	PlaneCollider(vec3 position, vec3 normal, GLhandleARB shaderProgramId) : Collider() {
		this->position = position;
		this->normal = normal;

		renderer = new PlaneRenderer(shaderProgramId, position, -normal);

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
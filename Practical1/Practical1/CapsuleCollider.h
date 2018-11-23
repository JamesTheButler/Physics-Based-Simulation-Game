#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>
#include "Collider.h"

class CapsuleCollider : public Collider {

private:

	//--------------------------------------- Private member variables -------------------------------------------
	SCALAR radius;
	
	vec3 endPoint;
	SCALAR length;
	vec3 orientation;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	CapsuleCollider(vec3 position, SCALAR radius, SCALAR length, vec3 orientation, GLhandleARB shaderProgramId) : Collider() {
		this->endPoint = position + orientation * length*0.5f;

		this->length = length;
		this->orientation = orientation;

		this->radius = radius;

		renderer = new CapsuleRenderer(shaderProgramId, endPoint, radius, length, orientation);

		renderer->setupOpenGLBuffers();
	}

	void handleCollision(vec3 & particlePosition) {
		
		vec3 & a = endPoint;
		vec3 b = a - orientation * length;
		
		float t = glm::dot((particlePosition - a), (b-a)/(glm::distance(b, a)*glm::distance(b, a)));
		if (t < 0.f) t = 0;
		if (t > 1.f) t = 1;
		vec3 c = a + t * (b-a);
		if(glm::distance(particlePosition, c)<radius)
			particlePosition = c + radius * glm::normalize(particlePosition - c);
	}

	vec3 getPosition() {
		return endPoint - orientation * length*0.5f;
	}

	void setPosition(vec3 position) {
		this->endPoint = position + orientation * length*0.5f;
	}

	SCALAR getLength() {
		return length;
	}
};
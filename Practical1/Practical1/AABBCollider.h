#pragma once

#include "Collider.h"

class AABBCollider : public Collider {
private:
	vec3 position;
	float width, height;
	bool isGameWonTrigger;
	bool isGameWon;

public:
	AABBCollider(vec3 position, float width, float height, GLhandleARB shaderProgramId) : Collider() {
		this->position = position;
		this->width = width;
		this->height = height;
		this->isGameWonTrigger = false;
		this->isGameWon = false;

		renderer = new AABBRenderer(shaderProgramId, position, width, height);
		renderer->setupOpenGLBuffers();
	}

	AABBCollider(vec3 position, float width, float height, GLhandleARB shaderProgramId, bool isTrigger) : AABBCollider(position, width, height, shaderProgramId) {
		this->isGameWonTrigger = isTrigger;
	}

	void handleCollision(vec3 & particlePosition) {
		float xDist, yDist;
		//determine if particle is inside box
		xDist = particlePosition.x - position.x;
		yDist = particlePosition.y - position.y;

		// if so, move it towards the position outside the box clostes to the current positions
		if (abs(xDist) < width / 2 && abs(yDist) < height / 2) {
			//find distances to edges
			float distToXEdge = width / 2.f - abs(xDist);
			float distToYEdge = height / 2.f - abs(yDist);

			//find pos on wall closest do currentparticle
			if (distToXEdge < distToYEdge)
				particlePosition.x = position.x + xDist / abs(xDist) * width*0.5f;
			else
				particlePosition.y = position.y + yDist / abs(yDist) * height*0.5f;
			if (isGameWonTrigger && !isGameWon) {
				std::cout << "!!! Game Won !!!\n";
				isGameWon = true;
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
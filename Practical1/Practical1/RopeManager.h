#include "Rope.h"

class RopeManager {
private:
	std::vector<Rope*> ropes;
	int ropeCount;
public:
#pragma once
	RopeManager(GLhandleARB shaderProgramId, int constraintIterations, int dragConstant, float ropeSize) {
		ropeCount = 5;
		float ropeDistance = 1.2f*ropeSize;
		for (int i = 0; i < ropeCount; i++) {
			Rope *rope = new Rope(verlet, shaderProgramId, ropeSize, vec3(2-i*ropeDistance*10, 4.f, 0.f), 70*(1-2*(i%2)));
			rope->solver->setConstraintIterations(constraintIterations);
			rope->solver->setDragConstant(dragConstant);
			ropes.push_back(rope);
		}
	}
	
	void timeStep(float gravity, float deltaTime){
		for (Rope* rope : ropes) {
			rope->addForce(vec3(0, gravity, 0));
			rope->timeStep(deltaTime, false);
		}
	}

	void draw() {
		for (Rope* rope : ropes) {
			rope->renderer->draw();
		}
	}

	void deleteRopes() {
		for (Rope* rope : ropes) {
			delete rope;
		}
		delete this;
	}
};
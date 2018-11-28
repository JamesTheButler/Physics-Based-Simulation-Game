#include "Rope.h"

class RopeManager {
private:
	std::vector<Rope*> ropes;
	int ropeCount;
public:
#pragma once
	RopeManager(GLhandleARB shaderProgramId, int constraintIterations, int dragConstant, float ropeSize, vec3 offset) {
		ropeCount = 5;
		float ropeDistance = 1.2f*ropeSize;
		for (int i = 0; i < ropeCount; i++) {
			Rope *rope = new Rope(verlet, shaderProgramId, ropeSize, offset + vec3(2-i*ropeDistance*10, 0,0), 70*(1-2*(i%2)));
			rope->solver->setConstraintIterations(constraintIterations);
			rope->solver->setDragConstant(dragConstant);
			ropes.push_back(rope);
		}
	}
	
	// find the particle in all ropes that is closest to the particle parameter
	Particle getClosestParticle(vec3 particle, float threshold) {
		Particle closestParticle;
		closestParticle.id =-1;

		float closestDistance = 10.f;
		for (Rope* rope : ropes) {
			std::vector<vec3> * ropePositions = rope->getPositions();
			std::vector<bool> * ropeIsMovables = rope->getIsMovables();
			int i = 0;
			for (vec3 pos : *ropePositions) {
				float dist = glm::distance(particle, pos);
				if (dist < threshold) {
					if (dist < closestDistance) {
						closestParticle = { i, ropePositions , ropeIsMovables };
						dist = closestDistance;
					}
				}
				i++;
			}
		}
		return closestParticle;
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
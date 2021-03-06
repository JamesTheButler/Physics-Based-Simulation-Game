#pragma once

#include "Renderer.h"

class PlaneRenderer : public Renderer {
private:
	std::vector<vec3> positions;

public:
	PlaneRenderer(GLhandleARB shaderProgramId, vec3 position, vec3 normal) :
		Renderer(shaderProgramId) {

		//draw line perpendicular to the normal through the position (i.e. draw the edge)
		positions.push_back(position + 50.f * vec3(normal.y, -normal.x, 0));
		positions.push_back(position - 50.f * vec3(normal.y, -normal.x, 0));

		numberOfVertices = positions.size();

		for (int i = 0; i < 2; i++) {
			normals.push_back(normal);
		}
	}

	void setupOpenGLBuffers() {
		glGenBuffers(1, &vertexPosBufferHandle);
		glGenBuffers(1, &vertexNormalBufferHandle);

		vertexPosAttribLocation = glGetAttribLocation(shaderProgramId, "vertexPos");
		vertexNormalAttribLocation = glGetAttribLocation(shaderProgramId, "vertexNormal");
		colorLocation = glGetUniformLocation(shaderProgramId, "color");
	}

	void draw() {

		glEnableVertexAttribArray(vertexPosAttribLocation);
		glEnableVertexAttribArray(vertexNormalAttribLocation);

		glBindBuffer(GL_ARRAY_BUFFER, vertexPosBufferHandle);
		glVertexAttribPointer(vertexPosAttribLocation, 3, GL_SCALAR, GL_FALSE, 0, NULL);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &(positions[0]), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBufferHandle);
		glVertexAttribPointer(vertexNormalAttribLocation, 3, GL_SCALAR, GL_FALSE, 0, NULL);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &(normals[0]), GL_DYNAMIC_DRAW);

		//Render the particles and constraints of the cloth:
		glUniform4f(colorLocation, 0, 1, 0, 1);
		glDrawArrays(GL_LINES, 0, numberOfVertices);

		glDisableVertexAttribArray(vertexNormalAttribLocation);
	}

};
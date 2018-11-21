#pragma once

#include "Renderer.h"

class PlaneRenderer : public Renderer {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> positions;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	PlaneRenderer(GLhandleARB shaderProgramId, vec3 position, vec3 normal) :
		Renderer(shaderProgramId) {

		positions.push_back(vec3(-50, 0, 50));
		positions.push_back(vec3(-50, 0, -50));
		positions.push_back(vec3(50, 0, 50));
		positions.push_back(vec3(50, 0, -50));

		numberOfVertices = positions.size();

		for (int i = 0; i < 4; i++) {
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
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3),
			&(normals[0]), GL_DYNAMIC_DRAW);

		//Render the cloth as a triangle mesh:
		glUniform4f(colorLocation, 0, 1, 0, 1);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, numberOfVertices);
	}

};
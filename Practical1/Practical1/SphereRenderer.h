#pragma once

#include "Renderer.h"

class SphereRenderer : public Renderer {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> positions;

	std::vector<unsigned int> trianglesIndices;
	GLuint trianglesIndexBufferHandle;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	SphereRenderer(GLhandleARB shaderProgramId, SCALAR radius) :
		Renderer(shaderProgramId) {

		int stackCount = 40;
		int sectorCount = 40;

		float sectorStep = 2 * glm::pi<double>() / sectorCount;
		float stackStep = glm::pi<double>() / stackCount;
		float sectorAngle, stackAngle;
		float lengthInv = 1.0f / radius;

		for (int i = 0; i <= stackCount; ++i)
		{
			stackAngle = glm::pi<double>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
			float xy = radius * cosf(stackAngle);             // r * cos(u)
			float z = radius * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sectorCount; ++j)
			{
				sectorAngle = j * sectorStep;

				// vertex position (x, y, z)
				float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)

				positions.push_back(vec3(x, y, z));

				// vertex normal (nx, ny, nz)
				float nx = x * lengthInv;
				float ny = y * lengthInv;
				float nz = z * lengthInv;
				normals.push_back(vec3(nx, ny, nz));
			}
		}

		numberOfVertices = positions.size();

		int k1, k2;
		for (int i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1);     // beginning of current stack
			k2 = k1 + sectorCount + 1;      // beginning of next stack

			for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding 1st and last stacks
				if (i != 0)
				{
					trianglesIndices.push_back(k1);
					trianglesIndices.push_back(k2);
					trianglesIndices.push_back(k1 + 1);
				}

				if (i != (stackCount - 1))
				{
					trianglesIndices.push_back(k1 + 1);
					trianglesIndices.push_back(k2);
					trianglesIndices.push_back(k2 + 1);
				}
			}
		}
	}

	void setupOpenGLBuffers() {
		glGenBuffers(1, &trianglesIndexBufferHandle);
		glGenBuffers(1, &vertexPosBufferHandle);
		glGenBuffers(1, &vertexNormalBufferHandle);

		vertexPosAttribLocation = glGetAttribLocation(shaderProgramId, "vertexPos");
		vertexNormalAttribLocation = glGetAttribLocation(shaderProgramId, "vertexNormal");

		colorLocation = glGetUniformLocation(shaderProgramId, "color");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trianglesIndexBufferHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, trianglesIndices.size() * sizeof(unsigned int),
			&(trianglesIndices[0]), GL_STATIC_DRAW);
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
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trianglesIndexBufferHandle);
		glDrawElements(GL_TRIANGLES, trianglesIndices.size(), GL_UNSIGNED_INT, NULL);

		glDisableVertexAttribArray(vertexNormalAttribLocation);
	}
};

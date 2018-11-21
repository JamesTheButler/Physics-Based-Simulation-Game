#pragma once

#include "Constraint.h"

#ifdef DOUBLE_PRECISION
typedef glm::dvec3 vec3;
#define SCALAR double
#define GL_SCALAR GL_DOUBLE
#else
typedef glm::vec3 vec3;
#define SCALAR float
#define GL_SCALAR GL_FLOAT
#endif

class Renderer {
private:

	//--------------------------------------- Private member variables -------------------------------------------
	std::vector<vec3> normals;
	std::vector<vec3> & positions;
	std::vector<Constraint> & constraints; // constraints between the particles

	std::vector<unsigned int> & trianglesIndices;

	const int numberOfParticles;

	//OpenGL related member variables:
	GLhandleARB shaderProgramId;
	GLuint vertexPosBufferHandle, vertexNormalBufferHandle, trianglesIndexBufferHandle, linesIndexBufferHandle;
	GLuint vertexPosAttribLocation, vertexNormalAttribLocation;
	GLint colorLocation;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	Renderer(GLhandleARB shaderProgramId, std::vector<vec3> & positions, 
		     std::vector<Constraint> & constraints, std::vector<unsigned int> & trianglesIndices, 
			 const int numberOfParticles) :
			     positions(positions), constraints(constraints), 
		         trianglesIndices(trianglesIndices), numberOfParticles(numberOfParticles) {

		this->shaderProgramId = shaderProgramId;
	}

	void setupOpenGLBuffers() {
		glGenBuffers(1, &trianglesIndexBufferHandle);
		glGenBuffers(1, &linesIndexBufferHandle);
		glGenBuffers(1, &vertexPosBufferHandle);
		glGenBuffers(1, &vertexNormalBufferHandle);

		vertexPosAttribLocation = glGetAttribLocation(shaderProgramId, "vertexPos");
		vertexNormalAttribLocation = glGetAttribLocation(shaderProgramId, "vertexNormal");

		colorLocation = glGetUniformLocation(shaderProgramId, "color");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trianglesIndexBufferHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, trianglesIndices.size() * sizeof(unsigned int),
			&(trianglesIndices[0]), GL_STATIC_DRAW);

		std::vector<unsigned int> constraintsVec;
		for (Constraint constraint : constraints) {
			constraintsVec.push_back(constraint.getP1());
			constraintsVec.push_back(constraint.getP2());
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesIndexBufferHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, constraintsVec.size() * sizeof(unsigned int),
			&(constraintsVec[0]), GL_STATIC_DRAW);
	}

	

	/*The cloth consists of triangles pairs of four particles in the grid as follows:

	(x,y)   *--* (x+1,y)
	| /|
	|/ |
	(x,y+1) *--* (x+1,y+1)

	*/
	void draw(bool renderParticlesAndConstraints) {
		glEnableVertexAttribArray(vertexPosAttribLocation);
		glEnableVertexAttribArray(vertexNormalAttribLocation);

		glBindBuffer(GL_ARRAY_BUFFER, vertexPosBufferHandle);
		glVertexAttribPointer(vertexPosAttribLocation, 3, GL_SCALAR, GL_FALSE, 0, NULL);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), &(positions[0]), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertexNormalBufferHandle);
		glVertexAttribPointer(vertexNormalAttribLocation, 3, GL_SCALAR, GL_FALSE, 0, NULL);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3),
			&(normals[0]), GL_DYNAMIC_DRAW);

		if (renderParticlesAndConstraints) {
			//Render the particles and constraints of the cloth:
			glUniform4f(colorLocation, 0, 1, 0, 1);
			glDrawArrays(GL_POINTS, 0, numberOfParticles);

			glUniform4f(colorLocation, 1, 1, 1, 1);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, linesIndexBufferHandle);
			glDrawElements(GL_LINES, constraints.size() * 2, GL_UNSIGNED_INT, NULL);
		}
		else {
			//Render the cloth as a triangle mesh:
			glUniform4f(colorLocation, 1, 0, 0, 1);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trianglesIndexBufferHandle);
			glDrawElements(GL_TRIANGLES, trianglesIndices.size(), GL_UNSIGNED_INT, NULL);
		}

		glDisableVertexAttribArray(vertexNormalAttribLocation);
	}

	void setNormals(std::vector<vec3> & normals) {
		this->normals = normals;
	}
};

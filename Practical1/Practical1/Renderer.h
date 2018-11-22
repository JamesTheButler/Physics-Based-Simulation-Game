#pragma once

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

protected:

	//--------------------------------------- Protected member variables -----------------------------------------
	std::vector<vec3> normals;

	//OpenGL related member variables:
	GLhandleARB shaderProgramId;
	GLuint vertexPosBufferHandle, vertexNormalBufferHandle;
	GLuint vertexPosAttribLocation, vertexNormalAttribLocation;
	GLint colorLocation;

	int numberOfVertices;

public:

	//--------------------------------------- Public methods -----------------------------------------------------
	Renderer(GLhandleARB shaderProgramId) : shaderProgramId(shaderProgramId) {

	}

	virtual void setupOpenGLBuffers() = 0;
	virtual void draw() = 0;

	void setNormals(std::vector<vec3> & normals) {
		this->normals = normals;
	}
};

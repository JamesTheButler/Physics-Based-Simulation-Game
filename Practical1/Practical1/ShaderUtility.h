#pragma once

std::string getFileContents(const char* filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "Sorry, can't open file: " << filename << std::endl;
		exit(0);
	}
	else {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
}

// Loading shader function
GLhandleARB loadShaderCode(const char* filename, unsigned int type) {
	GLhandleARB handle;

	// shader Compilation variable
	GLint result;				// Compilation code result
	GLint errorLoglength;
	char* errorLogText;
	GLsizei actualErrorLogLength;

	std::string file = getFileContents(filename);
	const char* cfile = file.c_str();

	handle = glCreateShaderObjectARB(type);
	if (!handle) {
		//We have failed creating the vertex shader object.
		std::cout << "Failed creating vertex shader object from file: " << filename << std::endl;
		exit(0);
	}

	glShaderSourceARB(
		handle, //The handle to our shader
		1, //The number of files.
		(const GLcharARB**)(&cfile), //An array of const char * data, which represents the source code of the shaders
		NULL);

	glCompileShaderARB(handle);

	//Compilation checking.
	glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

	// If an error was detected.
	if (!result) {
		//We failed to compile.
		std::cout << "Shader " << filename << " failed compilation" << std::endl;

		//Attempt to get the length of our error log.
		glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);

		//Create a buffer to read compilation error message
		errorLogText = (char*)malloc(sizeof(char)* errorLoglength);

		//Used to get the final length of the log.
		glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);

		// Display errors.
		std::cout << errorLogText << std::endl;

		// Free the buffer malloced earlier
		free(errorLogText);
	}

	return handle;
}

void loadShader(const char* vshader, const char* fshader, GLhandleARB & id) {
	GLhandleARB vertexShaderHandle = loadShaderCode(vshader, GL_VERTEX_SHADER);
	GLhandleARB fragmentShaderHandle = loadShaderCode(fshader, GL_FRAGMENT_SHADER);

	id = glCreateProgramObjectARB();

	glAttachObjectARB(id, vertexShaderHandle);
	glAttachObjectARB(id, fragmentShaderHandle);
	glLinkProgramARB(id);
}
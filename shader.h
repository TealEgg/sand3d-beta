#pragma once

namespace shader
{
	unsigned int createShader(std::string vertexPath, std::string fragPath)
	{
		// Create vertex and fragment shader id's
		unsigned int vertexID = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

		// Reading vert file into string
		std::string v = util::read(vertexPath);
		const char* vertexShader = v.c_str();

		// Read frag file into string
		std::string f = util::read(fragPath);
		const char* fragmentShader = f.c_str();

		// Compile Vertex Shader
		glShaderSource(vertexID, 1, &vertexShader, NULL);
		glCompileShader(vertexID);

		// Check for Vertex Shader errors
		int result = GL_FALSE;
		int infoLength;
		glGetShaderiv(vertexID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &infoLength);
		if (infoLength > 0)
		{
			char* buffer = new char[infoLength];
			glGetShaderInfoLog(vertexID, 1000, NULL, buffer);
			std::cout << "Vertex error" << std::endl;
			std::cout << buffer << std::endl;
			delete[] buffer;
		}

		// Compile Fragment Shader
		glShaderSource(fragmentID, 1, &fragmentShader, NULL);
		glCompileShader(fragmentID);

		// Check for Fragment Shader errors
		glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(fragmentID, GL_INFO_LOG_LENGTH, &infoLength);
		if (infoLength > 0)
		{
			char* buffer = new char[infoLength];
			glGetShaderInfoLog(fragmentID, 1000, NULL, buffer);
			std::cout << "Fragment error" << std::endl;
			std::cout << buffer << std::endl;
			delete[] buffer;
		}

		// Creating a program id for our shader program
		GLuint ProgramID = glCreateProgram();

		// Attaching the vertex and fragment shaders to the program
		glAttachShader(ProgramID, vertexID);
		glAttachShader(ProgramID, fragmentID);

		// Linking and using the program
		glLinkProgram(ProgramID);
		glUseProgram(ProgramID);

		// Detaching the vertex and fragment shaders to delete them
		glDetachShader(ProgramID, vertexID);
		glDetachShader(ProgramID, fragmentID);
		glDeleteShader(vertexID);
		glDeleteShader(fragmentID);

		return ProgramID;
	}

	unsigned int createComputeShader(std::string computePath)
	{
		// Create compute shader id
		unsigned int computeID = glCreateShader(GL_COMPUTE_SHADER);

		// Reading comp file into string
		std::string c = util::read(computePath);
		const char* computeShader = c.c_str();

		// Compile Vertex Shader
		glShaderSource(computeID, 1, &computeShader, NULL);
		glCompileShader(computeID);

		// Check for Compute Shader errors
		int result = GL_FALSE;
		int infoLength;
		glGetShaderiv(computeID, GL_COMPILE_STATUS, &result);
		glGetShaderiv(computeID, GL_INFO_LOG_LENGTH, &infoLength);
		if (infoLength > 0)
		{
			char* buffer = new char[infoLength];
			glGetShaderInfoLog(computeID, 1000, NULL, buffer);
			std::cout << "Compute error" << std::endl;
			std::cout << buffer << std::endl;
			delete[] buffer;
		}

		// Creating a program id for our shader program
		GLuint ProgramID = glCreateProgram();

		// Attaching the compute shader to the program
		glAttachShader(ProgramID, computeID);

		// Linking and using the program
		glLinkProgram(ProgramID);
		glUseProgram(ProgramID);

		// Detaching the vertex and fragment shaders to delete them
		glDetachShader(ProgramID, computeID);
		glDeleteShader(computeID);

		return ProgramID;
	}

	void useShader(unsigned int id)
	{
		glUseProgram(id);
	}

	void deleteShader(unsigned int id)
	{
		glDeleteProgram(id);
	}
}
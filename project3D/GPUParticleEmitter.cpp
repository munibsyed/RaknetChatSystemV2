#include "GPUParticleEmitter.h"
#include <fstream>

std::string LoadShader(const char * filename)
{
	std::ifstream myFile;
	std::string line;
	std::string output;

	myFile.open(filename);

	if (myFile.is_open())
	{
		while (!myFile.eof())
		{
			std::getline(myFile, line);
			output += '\n' + line;
		}
	}

	return output;
}


GPUParticleEmitter::GPUParticleEmitter()
{
	m_particles = nullptr;
	m_maxParticles = 0;
	m_drawShader = 0;
	m_updateShader = 0;
	m_lastDrawTime = 0;

	m_VAO[0] = 0;
	m_VAO[1] = 0;

	m_VBO[0] = 0;
	m_VAO[1] = 0;
}


GPUParticleEmitter::~GPUParticleEmitter()
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_VAO);
	glDeleteBuffers(2, m_VBO);

	glDeleteProgram(m_drawShader);
	glDeleteProgram(m_updateShader);
}

void GPUParticleEmitter::Initialize(unsigned int maxParticles, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, float gravity, const vec4 & startColour, const vec4 & endColour, vec3 position)
{
	m_maxParticles = maxParticles;
	m_gravity = gravity;
	m_lifespanMin = lifespanMin;
	m_lifespanMax = lifespanMax;
	m_velocityMin = velocityMin;
	m_velocityMax = velocityMax;
	m_startSize = startSize;
	m_endSize = endSize;
	m_startColour = startColour;
	m_endColour = endColour;
	m_position = position;

	m_particles = new GPUParticle[m_maxParticles];
	m_activeBuffer = 0; //starting ping-pong buffwe

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void GPUParticleEmitter::Draw(float time, const mat4 & cameraTransform, const mat4 & projectionView)
{
	//update the particles using transform feedback
	glUseProgram(m_updateShader);

	//bind time information
	int location = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(location, time);

	float deltaTime = time - m_lastDrawTime;
	m_lastDrawTime = time;

	location = glGetUniformLocation(m_updateShader, "deltaTime");
	glUniform1f(location, deltaTime);

	//bind emitter's position
	location = glGetUniformLocation(m_updateShader, "emitterPosition");
	glUniform3fv(location, 1, glm::value_ptr(m_position));

	//disable rasterization
	glEnable(GL_RASTERIZER_DISCARD);

	//bind the buffer we want to update
	glBindVertexArray(m_VAO[m_activeBuffer]);

	//work on other buffer
	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;

	//bind the buffer we will update into as points and begin transform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_VBO[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	//disable transform feedback and enable rasterization again
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK, 0, 0);

	glUseProgram(m_drawShader);
	
	location = glGetUniformLocation(m_drawShader, "projectionView"); 
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(projectionView));
	location = glGetUniformLocation(m_drawShader, "cameraTransform"); 
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(cameraTransform)); // draw particles in the "other" buffer
	glBindVertexArray(m_VAO[otherBuffer]); 
	glDrawArrays(GL_POINTS, 0, m_maxParticles); // swap for next frame m_activeBuffer = otherBuffer;

	//swap for next frame
	m_activeBuffer = otherBuffer;

}

void GPUParticleEmitter::CreateBuffers()
{
	//create OpenGL buffers
	glGenVertexArrays(2, m_VAO);
	glGenBuffers(2, m_VBO);

	//setup the first buffer
	glBindVertexArray(m_VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GPUParticle) * m_maxParticles, m_particles, GL_STREAM_DRAW);	

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0 + 12));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0 + 24));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0 + 28));

	//setup the second buffer
	glBindVertexArray(m_VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GPUParticle) * m_maxParticles, 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //velocity
	glEnableVertexAttribArray(2); //lifetime
	glEnableVertexAttribArray(3); //lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0 + 12));
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0 + 24));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0 + 28));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void GPUParticleEmitter::CreateUpdateShader()
{
	//create a vertex shader handle
	std::string gpuShaderStr = LoadShader("gpuParticleUpdate.vs");
	const char* gpuShader = gpuShaderStr.c_str();

	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, (const char**)&gpuShader, 0);
	glCompileShader(vs);
	//update shader contains the vs
	m_updateShader = glCreateProgram();
	glAttachShader(m_updateShader, vs);

	//specify the data the we will stream back
	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(m_updateShader);

	glDeleteShader(vs);

	glUseProgram(m_updateShader);

	//bind lifetime minimum and maximum
	int location = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(location, m_lifespanMin);
	location = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(location, m_lifespanMax);

	location = glGetUniformLocation(m_updateShader, "gravity");
	glUniform1f(location, m_gravity);

	location - glGetUniformLocation(m_updateShader, "gravitySource[0].location");
	glUniform3fv(location, 1, glm::value_ptr(vec3(0, 10, 0)));

	location = glGetUniformLocation(m_updateShader, "gravitySource[0].strength");
	glUniform1f(location, 1);

	int success = GL_FALSE;
	glGetProgramiv(m_updateShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_drawShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_drawShader, infoLogLength, 0, infoLog);
		std::cout << "Error: Failed to link shader program." << std::endl;
		std::cout << infoLog << std::endl;

		delete[] infoLog;
	}

}

void GPUParticleEmitter::CreateDrawShader()
{
	//load shaders from files and turn into GL shader handles
	std::string vsSourceStr = LoadShader("vsSourceParticle.vs");
	std::string fsSourceStr = LoadShader("fsSourceParticle.frag");
	std::string gsSourceStr = LoadShader("gsSourceParticle.geom");

	const char* vsSource = vsSourceStr.c_str();
	const char* fsSource = fsSourceStr.c_str();
	const char* gsSource = gsSourceStr.c_str();


	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	glShaderSource(geometryShader, 1, (const char**)&gsSource, 0);
	glCompileShader(geometryShader);

	m_drawShader = glCreateProgram();

	glAttachShader(m_drawShader, vertexShader);
	glAttachShader(m_drawShader, fragmentShader);
	glAttachShader(m_drawShader, geometryShader);
	glLinkProgram(m_drawShader);

	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	//bind the shader so that we can set some uniforms that don't change per frame
	glUseProgram(m_drawShader);

	//bind size information for interpolation that won't change
	int location = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(location, m_startSize);

	location = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(location, m_endSize);

	//bind colour information that won't change
	location = glGetUniformLocation(m_drawShader, "colourStart");
	glUniform4fv(location, 1, glm::value_ptr(m_startColour));
	location = glGetUniformLocation(m_drawShader, "colourEnd");
	glUniform4fv(location, 1, glm::value_ptr(m_endColour));

	glGetProgramiv(m_drawShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_drawShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_drawShader, infoLogLength, 0, infoLog);
		std::cout << "Error: Failed to link shader program." << std::endl;
		std::cout << infoLog << std::endl;

		delete[] infoLog;
	}

	
}

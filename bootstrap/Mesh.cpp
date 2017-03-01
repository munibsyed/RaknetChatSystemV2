#include "Mesh.h"

Mesh::Mesh(ParticleEmitter* emitter)
{
	m_tex = nullptr;
	m_emitter = emitter;
}

Mesh::Mesh(const char* filename, const char* normalMapFilename)
{
	m_emitter = nullptr;
	angleRotate = 0;
	//loads texture
	m_tex = new aie::Texture(filename, normalMapFilename);
	//m_texture = new MyTexture(filename);

	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	LoadAndAttachShaders("vsSourceTexture.vs", "fsSourceTexture.frag");
	GenerateGrid(10, 10);
	bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, "../Models/soulspear/soulspear/soulspear.obj");
	ObjLoader *objLoader = new ObjLoader();
	
	m_glInfo = objLoader->CreateOpenGLBuffers(attribs, shapes);
	lightPosition = vec3(0, 2, 2);
}


Mesh::~Mesh()
{
	//delete m_texture;
	if (m_tex != nullptr)
		delete m_tex;

	if (m_emitter != nullptr)
		delete m_emitter;
}

void Mesh::GenerateGrid(int rows, int columns)
{
	m_rows = rows;
	m_columns = columns;
	Vertex* vertexArray = new Vertex[rows*columns];
	unsigned int* indices = new unsigned int[(rows - 1) * (columns - 1) * 6];

	for (unsigned int i = 0; i < rows; i++)
	{
		for (unsigned int ii = 0; ii < columns; ii++)
		{
			vertexArray[i * columns + ii].position = vec4((float)ii, 0, (float)i, 1);

			vec3 colour = vec3(glm::sin((ii / (float)(columns - 1)) * (i / (float)(rows - 1))));
			//vertexArray[i * columns + ii].colour = vec4(colour, 1);
		}
	}

	unsigned int index = 0;
	for (unsigned int i = 0; i < rows - 1; i++)
	{
		for (unsigned int ii = 0; ii < columns - 1; ii++)
		{
			// triangle 1 
			indices[index++] = i * columns + ii;
			indices[index++] = (i + 1) * columns + ii;
			indices[index++] = (i + 1) * columns + (ii + 1);
			// triangle 2 
			indices[index++] = i * columns + ii;
			indices[index++] = (i + 1) * columns + (ii + 1);
			indices[index++] = i * columns + (ii + 1);
		}
	}

	//generate vertex buffer object
	glGenBuffers(1, &m_VBO);
	//generate index buffer object
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//bind and fill VBO + IBO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, (rows * columns) * sizeof(Vertex), vertexArray, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
	//added
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0)+16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (columns - 1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertexArray;
	delete[] indices;
}

void Mesh::LoadAndAttachShaders(const char * vsFilename, const char * fsFilename)
{
	std::string vsSourceStr = LoadShader(vsFilename);
	std::string fsSourceStr = LoadShader(fsFilename);
	
	const char* vsSource = vsSourceStr.c_str();
	const char* fsSource = fsSourceStr.c_str();

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		std::cout << "Error: Failed to link shader program." << std::endl;
		std::cout << infoLog << std::endl;
			
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Mesh::Draw(mat4 projectionView, mat4 cameraWorld)
{
	glUseProgram(m_programID);

	unsigned int projectionViewTransform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(projectionViewTransform, 1, GL_FALSE, glm::value_ptr(projectionView));
	
	//give world (view) matrix to vertex shader so normals can be calculated correctly (they can be moved according to the model's position, orientation etc.)

	unsigned int cameraWorldTransform = glGetUniformLocation(m_programID, "cameraWorld");
	glUniformMatrix4fv(cameraWorldTransform, 1, GL_FALSE, glm::value_ptr(cameraWorld));

	if (m_tex != nullptr)
	{
		//set texture slot
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex->getHandle());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_tex->getNormalHandle());
	}

	//tell the shader where it is
	unsigned int diffuse = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(diffuse, 0);

	//set normal sampler
	unsigned int normal = glGetUniformLocation(m_programID, "normal");
	glUniform1i(normal, 1);



	//set light direction uniform
	unsigned int lightDir = glGetUniformLocation(m_programID, "lightDirection");
	vec3 lightDirection(1, 0, 0);
	glUniform3fv(lightDir, 1, glm::value_ptr(lightDirection));
	aie::Input *input = aie::Input::getInstance();
	if (input->isKeyDown(aie::INPUT_KEY_UP))
	{
		lightPosition.y += 0.1f;
	}

	else if (input->isKeyDown(aie::INPUT_KEY_DOWN))
	{
		lightPosition.y -= 0.1f;
	}

	angleRotate = 0.01f;
	//set y rotation matrix
	unsigned int yRot = glGetUniformLocation(m_programID, "rotateOnY");
	yRotate = glm::rotate(yRotate, angleRotate, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(yRot, 1, GL_FALSE, glm::value_ptr(yRotate));

	//set light pos
	unsigned int lightPos = glGetUniformLocation(m_programID, "lightPos");
	//vec3 lightPosition(0, 2, 0);
	glUniform3fv(lightPos, 1, glm::value_ptr(lightPosition));

	//set light colour
	unsigned int lightColour = glGetUniformLocation(m_programID, "lightColour");
	vec3 lightColourVec(0, 1, 1);
	glUniform3fv(lightColour, 1, glm::value_ptr(lightColourVec));

	//set ambient light colour
	unsigned int ambientLight = glGetUniformLocation(m_programID, "ambientColour");
	vec3 ambientLightColour(1, 1, 1);
	glUniform3fv(ambientLight, 1, glm::value_ptr(ambientLightColour));

	//set ambient light scale
	unsigned int ambientLightScale = glGetUniformLocation(m_programID, "ambientColourScale");
	vec3 ambientScale(0.05f, 0.05f, 0.05f);
	glUniform3fv(ambientLightScale, 1, glm::value_ptr(ambientScale));

	//set cone angle
	unsigned int coneAngle = glGetUniformLocation(m_programID, "coneAngle");
	glUniform1f(coneAngle, 90.0f);

	//set attenuation
	unsigned int attenuation = glGetUniformLocation(m_programID, "attenuation");
	glUniform1f(attenuation, 3.0f);

	//set camera pos
	unsigned int cameraPos = glGetUniformLocation(m_programID, "cameraPos");
	glUniform3fv(cameraPos, 1, glm::value_ptr(vec3(cameraWorld[3][0], cameraWorld[3][1], cameraWorld[3][2])));
	
	//set specular power
	float specPower = 32.0f;
	unsigned int specularPower = glGetUniformLocation(m_programID, "specPower");
	glUniform1f(specularPower, specPower);	
/*
	for (auto& gl : m_glInfo)
	{
		glBindVertexArray(gl.m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount * 3);
	}*/
		
	m_emitter->Draw();

	//glBindVertexArray(m_VAO);
	//unsigned int indexCount = (m_rows - 1) * (m_columns - 1) * 6;

	//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::Draw(mat4 projectionView, float t, float height)
{
	glUseProgram(m_programID);

	unsigned int projectionViewTransform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	unsigned int heightScale = glGetUniformLocation(m_programID, "heightScale");
	unsigned int time = glGetUniformLocation(m_programID, "time");
	glUniformMatrix4fv(projectionViewTransform, 1, false, glm::value_ptr(projectionView));
	glUniform1f(heightScale, height);
	glUniform1f(time, t);

	glBindVertexArray(m_VAO);
	unsigned int indexCount = (m_rows - 1) * (m_columns - 1) * 6;

	//what shape to draw, number of triangles, datatype in index buffer, offset (where to start))
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

ParticleEmitter* Mesh::GetEmitter()
{
	// TODO: insert return statement here
	return m_emitter;
}

std::string Mesh::LoadShader(const char * filename)
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

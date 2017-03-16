#include "Mesh.h"

Mesh::Mesh()
{
	m_tex = nullptr;
	m_gpuEmitter = nullptr;
	m_emitter = nullptr;
	LoadAndAttachShaders("vsSourcePost.vs", "fsSourcePost.frag");

}

Mesh::Mesh(ParticleEmitter* emitter)
{
	m_tex = nullptr;
	m_gpuEmitter = nullptr;
	m_emitter = emitter;
}

Mesh::Mesh(GPUParticleEmitter * gpuEmitter)
{
	m_tex = nullptr;
	m_emitter = nullptr;
	m_gpuEmitter = gpuEmitter;
}

Mesh::Mesh(ParticleEmitter* emitter, const char* filename)
{
	m_emitter = emitter;
	m_tex = new aie::Texture(filename);
}

Mesh::Mesh(const char* filename, const char* normalMapFilename)
{
	noPostProcess = true;
	scanLines = false;
	grayScale = false;
	distort = false;

	spotLightOn = true;
	specLightOn = true;
	ambientLightOn = true;
	normalsOn = true;

	m_emitter = nullptr;
	m_gpuEmitter = nullptr;
	angleRotate = 0;
	//loads texture
	m_tex = new aie::Texture(filename, normalMapFilename);
	m_soulSpearAABB = new AABB();
	
	//m_texture = new MyTexture(filename);

	//soulspear
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	LoadAndAttachShaders("vsSourceTexture.vs", "fsSourceTexture.frag");
	bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, "../Models/soulspear/soulspear/soulspear.obj");
	ObjLoader *objLoader = new ObjLoader();
	m_glInfo = objLoader->CreateOpenGLBuffers(attribs, shapes);
	
	m_soulSpearAABB->Fit(attribs.vertices);

	lightPosition = vec3(0, 2, 2);

	//for hand animation
	tinyobj::attrib_t handAttribs[2]; 
	std::vector<tinyobj::shape_t> handShapes[2];
	std::vector<tinyobj::material_t> handMaterials[2];

	//load in hand mesh
	bool success0 = tinyobj::LoadObj(&handAttribs[0], &handShapes[0], &handMaterials[0], &err, "../Models/hand/hand_00.obj");
	bool success1 = tinyobj::LoadObj(&handAttribs[1], &handShapes[1], &handMaterials[1], &err, "../Models/hand/hand_37.obj");

	for (unsigned int i = 0; i < handShapes[0].size(); i++)
	{
		OpenGLInfo gl;
		std::vector<OpenGLInfo> firstVec = objLoader->CreateOpenGLBuffers(handAttribs[0], handShapes[0][i]);
		gl.m_VBO = firstVec[0].m_VBO;
		std::vector<OpenGLInfo> secondVec = objLoader->CreateOpenGLBuffers(handAttribs[1], handShapes[1][i]);
		gl.m_VBO2 = secondVec[0].m_VBO;

		//shapes have matching face counts
		gl.m_faceCount = handShapes[0][i].mesh.num_face_vertices.size();

		glGenVertexArrays(1, &gl.m_VAO);
		glBindVertexArray(gl.m_VAO);

		//bind first VBO
		glBindBuffer(GL_ARRAY_BUFFER, gl.m_VBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + 12);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + 24);

		//bind second VBO
		glBindBuffer(GL_ARRAY_BUFFER, gl.m_VBO2);
		glEnableVertexAttribArray(3); //position 2
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + 0); //32
		glEnableVertexAttribArray(5); //normal 2
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + 12); //44

		glBindVertexArray(0);

		m_glInfo.push_back(gl);
	}

	delete objLoader;
}


Mesh::~Mesh()
{
	//delete m_texture;
	if (m_tex != nullptr)
		delete m_tex;

	if (m_emitter != nullptr)
		delete m_emitter;

	if (m_gpuEmitter != nullptr)
		delete m_gpuEmitter;

	if (m_soulSpearAABB != nullptr)
		delete m_soulSpearAABB;
}

void Mesh::GenerateFrameBufferObject()
{
	//Post Processing Exercises
	glGenFramebuffers(1, &m_FBO);	
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_FBOTexture);
	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FBOTexture, 0);
	glGenRenderbuffers(1, &m_FBODepth);

	glBindRenderbuffer(GL_RENDERBUFFER, m_FBODepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_FBODepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer Error!" << std::endl;

	//unbind the FBO so we can render to the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//512x512 render target exercise

	////set up and bind a frame buffer
	//glGenFramebuffers(1, &m_FBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	////create a texture and bind it
	//glGenTextures(1, &m_FBOTexture);
	//glBindTexture(GL_TEXTURE_2D, m_FBOTexture);

	////specify texture format for storage
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	////attach it to the framebuffer as the first colour attachment
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FBOTexture, 0);

	////TO DO: attach render targets here
	//	
	////setup and bit a 24-bit depth buffer as a render buffer
	//glGenRenderbuffers(1, &m_FBODepth);
	//glBindRenderbuffer(GL_RENDERBUFFER, m_FBODepth);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	////while FBO is still bound
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_FBODepth);

	////while the FBO is bound
	//GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, drawBuffers);

	//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	//if (status != GL_FRAMEBUFFER_COMPLETE)
	//	std::cout << "Framebuffer Error!" << std::endl;

	////unbind the FBO so we can render to the back buffer
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Mesh::CreatePlane()
{
	//fullscreen quad
	vec2 halfTexel = 1.0f / vec2(1280, 720) * 0.5f;

	float vertexData[] =
	{
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,

		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y
	};

	unsigned int indexData[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW);	
	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //uvs

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::GenerateGrid(int rows, int columns)
{
	m_rows = rows;
	m_columns = columns;
	Vertex* vertexArray = new Vertex[rows*columns];
	unsigned int* indices = new unsigned int[(rows - 1) * (columns - 1) * 6];

	for (int i = 0; i < rows; i++)
	{
		for (int ii = 0; ii < columns; ii++)
		{
			vertexArray[i * columns + ii].position = vec4((float)ii, 0, (float)i, 1);

			vec3 colour = vec3(glm::sin((ii / (float)(columns - 1)) * (i / (float)(rows - 1))));
			//vertexArray[i * columns + ii].colour = vec4(colour, 1);
		}
	}

	unsigned int index = 0;
	for (int i = 0; i < rows - 1; i++)
	{
		for (int ii = 0; ii < columns - 1; ii++)
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

void ShaderSyntaxCheck(std::string source, std::vector<std::string> &warningMessages)
{
	char nums[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
	for (unsigned int i = 0; i < 10; i++)
	{
		const char* suffix_f = new const char[3] { nums[i], 'f', '\0' };
		const char* suffix_F = new const char[3]{ nums[i], 'F', '\0' };

		if (source.find(suffix_f) != -1)
		{
			std::stringstream ss;
			ss << "WARNING [" << warningMessages.size() << "] " << " 'f' suffix found after floating point value, non-standard GLSL." << '\n';
			warningMessages.push_back(ss.str());
		}

		if (source.find(suffix_F) != -1)
		{
			std::stringstream ss;
			ss << "WARNING [" << warningMessages.size() << "] " << " 'F' suffix found after floating point value, non-standard GLSL." << '\n';
			warningMessages.push_back(ss.str());
		}

		delete[] suffix_f;
		delete[] suffix_F;
	}
}

void Mesh::LoadPostShaders(const char * postShaderVs, const char * postShaderFs)
{
	std::string vsSourceStr = LoadShader(postShaderVs);
	std::string fsSourceStr = LoadShader(postShaderFs);

	const char* vsSource = vsSourceStr.c_str();
	const char* fsSource = fsSourceStr.c_str();

	int success = GL_FALSE;
	int compileSucess = GL_FALSE;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	m_postProgramID = glCreateProgram();
	glAttachShader(m_postProgramID, vertexShader);
	glAttachShader(m_postProgramID, fragmentShader);
	glLinkProgram(m_postProgramID);

	glGetProgramiv(m_postProgramID, GL_LINK_STATUS, &success);
	glGetProgramiv(m_postProgramID, GL_VALIDATE_STATUS, &compileSucess);

	if (compileSucess == GL_FALSE)
	{
		std::stringstream ss;
		std::cout << "Compile fail!" << std::endl;

		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader compile fail." << '\n';

		m_errorMessages.push_back(ss.str());
	}

	if (success == GL_FALSE)
	{
		std::stringstream ss;
		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader link fail." << '\n';
		m_errorMessages.push_back(ss.str());
		int infoLogLength = 0;
		glGetProgramiv(m_postProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_postProgramID, infoLogLength, 0, infoLog);
		std::cout << "Error: Failed to link shader program." << std::endl;
		std::cout << infoLog << std::endl;

		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Mesh::LoadMorphShaders(const char * morphShaderVs, const char * morphShaderFs)
{
	std::string vsSourceStr = LoadShader(morphShaderVs);
	std::string fsSourceStr = LoadShader(morphShaderFs);

	const char* vsSource = vsSourceStr.c_str();
	const char* fsSource = fsSourceStr.c_str();

	int success = GL_FALSE;
	int compileSucess = GL_FALSE;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	m_morphShader = glCreateProgram();
	glAttachShader(m_morphShader, vertexShader);
	glAttachShader(m_morphShader, fragmentShader);
	glLinkProgram(m_morphShader);

	glGetProgramiv(m_morphShader, GL_LINK_STATUS, &success);
	glGetProgramiv(m_morphShader, GL_VALIDATE_STATUS, &compileSucess);

	if (compileSucess == GL_FALSE)
	{
		std::stringstream ss;
		std::cout << "Compile fail!" << std::endl;

		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader compile fail." << '\n';

		m_errorMessages.push_back(ss.str());
	}

	if (success == GL_FALSE)
	{
		std::stringstream ss;
		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader link fail." << '\n';
		m_errorMessages.push_back(ss.str());
		int infoLogLength = 0;
		glGetProgramiv(m_morphShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_morphShader, infoLogLength, 0, infoLog);
		std::cout << "Error: Failed to link shader program." << std::endl;
		std::cout << infoLog << std::endl;

		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void Mesh::LoadAndAttachShaders(const char * vsFilename, const char * fsFilename)
{
	std::string vsSourceStr = LoadShader(vsFilename);
	std::string fsSourceStr = LoadShader(fsFilename);
	
	ShaderSyntaxCheck(vsSourceStr, m_warningMessages);
	ShaderSyntaxCheck(fsSourceStr, m_warningMessages);

	const char* vsSource = vsSourceStr.c_str();
	const char* fsSource = fsSourceStr.c_str();

	int success = GL_FALSE;
	int compileSucess = GL_TRUE;

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
	glGetProgramiv(m_programID, GL_VALIDATE_STATUS, &compileSucess);

	if (compileSucess == GL_FALSE)
	{
		std::cout << "Compile fail! Forward shaders" << std::endl;
		std::stringstream ss;
		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader compile fail." << '\n';

		m_errorMessages.push_back(ss.str());
	}

	if (success == GL_FALSE)
	{
		std::stringstream ss;
		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader link fail." << '\n';
		m_errorMessages.push_back(ss.str());
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

void Mesh::Draw(mat4 projectionView, float time)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, 1280, 720);

	vec4 clearColour(0.75f, 0.75f, 0.75f, 1);
		//ImGui::TextColored(ImVec4(1,0,0,1), "Some text");

	//glClearColor(clearColour.x, clearColour.y, clearColour.z, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(projectionView);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	ImGui::Begin("Error Console: ");
	for (unsigned int i = 0; i < m_errorMessages.size(); i++)
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), m_errorMessages[i].c_str());
	}

	for (unsigned int i = 0; i < m_warningMessages.size(); i++)
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), m_warningMessages[i].c_str());
	}

	ImGui::End();

	//glClearColor(0.25f, 0.25f, 0.25f, 1);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
	int location = glGetUniformLocation(m_programID, "target");
	glUniform1i(location, 0);

	location = glGetUniformLocation(m_programID, "time");
	glUniform1f(location, time);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Mesh::DrawForwardRendering(float time, mat4 &projectionView, mat4 &cameraWorld)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, 1280, 720);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(projectionView);


	glUseProgram(m_programID);

	int location = glGetUniformLocation(m_programID, "keyTime");
	glUniform1f(location, cosf(time) * 0.5f + 0.5f);

	unsigned int projectionViewTransform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(projectionViewTransform, 1, GL_FALSE, glm::value_ptr(projectionView));

	//give worrd (view) matrix to vertex shader so normals can be calculated correctly (they can be moved according to the model's position, orientation etc.)

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

	unsigned int light = glGetUniformLocation(m_programID, "spotLightOn");
	glUniform1i(light, spotLightOn);
	light = glGetUniformLocation(m_programID, "specOn");
	glUniform1i(light, specLightOn);
	light = glGetUniformLocation(m_programID, "ambientOn");
	glUniform1i(light, ambientLightOn);

	unsigned int normalOn = glGetUniformLocation(m_programID, "normalsOn");
	glUniform1i(normalOn, normalsOn);

	

	//draw soulspear
	glBindVertexArray(m_glInfo[0].m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, m_glInfo[0].m_faceCount * 3);


	glUseProgram(m_morphShader);

	location = glGetUniformLocation(m_morphShader, "keyTime");
	glUniform1f(location, cosf(time) * 0.5f + 0.5f);

	projectionViewTransform = glGetUniformLocation(m_morphShader, "projectionViewWorldMatrix");
	glUniformMatrix4fv(projectionViewTransform, 1, GL_FALSE, glm::value_ptr(projectionView));

	for (unsigned int i = 1; i < m_glInfo.size(); i++)
	{
		glBindVertexArray(m_glInfo[i].m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, m_glInfo[i].m_faceCount * 3);
	}

	//for (auto& gl : m_glInfo)
	//{
	//	glBindVertexArray(gl.m_VAO);
	//	glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount * 3);
	//	
	//}

}

void Mesh::DrawPost(float time, mat4 &projectionView)
{	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	ImGui::Begin("Error Console: ");
	for (unsigned int i = 0; i < m_errorMessages.size(); i++)
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), m_errorMessages[i].c_str());
	}

	for (unsigned int i = 0; i < m_warningMessages.size(); i++)
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), m_warningMessages[i].c_str());
	}
	ImGui::Checkbox("SpotLight On", &spotLightOn);
	ImGui::Checkbox("Spec On", &specLightOn);
	ImGui::Checkbox("Ambient On", &ambientLightOn);
	ImGui::Checkbox("Normals On", &normalsOn);

	ImGui::Checkbox("None", &noPostProcess);

	if (noPostProcess == true)
	{
		scanLines = false;
		grayScale = false;
		distort = false;
	}

	ImGui::Checkbox("Scan Lines", &scanLines);

	if (scanLines == true)
	{
		noPostProcess = false;
		grayScale = false;
		distort = false;
	}

	ImGui::Checkbox("Gray Scale", &grayScale);

	if (grayScale == true)
	{
		noPostProcess = false;
		scanLines = false;
		distort = false;
	}

	ImGui::Checkbox("Distort", &distort);
	if (distort == true)
	{
		noPostProcess = false;
		grayScale = false;
		scanLines = false;
	}

	ImGui::End();

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_postProgramID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
	int location = glGetUniformLocation(m_postProgramID, "target");
	glUniform1i(location, 0);
	location = glGetUniformLocation(m_postProgramID, "time");
	glUniform1f(location, time);

	unsigned int postLoc = glGetUniformLocation(m_postProgramID, "noPostProcess");
	glUniform1i(postLoc, noPostProcess);

	postLoc = glGetUniformLocation(m_postProgramID, "scanLines");
	glUniform1i(postLoc, scanLines);

	postLoc = glGetUniformLocation(m_postProgramID, "grayScale");
	glUniform1i(postLoc, grayScale);

	postLoc = glGetUniformLocation(m_postProgramID, "distort");
	glUniform1i(postLoc, distort);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Mesh::Draw(float time, mat4 &projectionView, mat4 &cameraWorld, FlyCamera *cam)
{

	DrawForwardRendering(time, projectionView, cameraWorld);
	DrawPost(time, projectionView);

	//m_emitter->Draw();
	//m_gpuEmitter->Draw(time, cameraWorld, projectionView);

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

GPUParticleEmitter * Mesh::GetGPUEmitter()
{
	return m_gpuEmitter;
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

		myFile.close();
	}

	else
	{
		std::stringstream ss;
		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader source file not found: " << "\"" << filename << "\"" << '\n';
		m_errorMessages.push_back(ss.str());
	}


	return output;
}

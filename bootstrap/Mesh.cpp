//#include "Mesh.h"
//
//Mesh::Mesh()
//{
//	m_tex = nullptr;
//	m_gpuEmitter = nullptr;
//	m_emitter = nullptr;
//	m_soulSpearAABB = nullptr;
//
//}
//
//Mesh::Mesh(ParticleEmitter* emitter)
//{
//	m_tex = nullptr;
//	m_gpuEmitter = nullptr;
//	m_emitter = emitter;
//}
//
//Mesh::Mesh(GPUParticleEmitter * gpuEmitter)
//{
//	m_tex = nullptr;
//	m_emitter = nullptr;
//	m_gpuEmitter = gpuEmitter;
//}
//
//Mesh::Mesh(ParticleEmitter* emitter, const char* filename)
//{
//	m_emitter = emitter;
//	m_tex = new aie::Texture(filename);
//}
//
//Mesh::Mesh(const char* filename, const char* normalMapFilename)
//{
//	noPostProcess = true;
//	scanLines = false;
//	grayScale = false;
//	distort = false;
//
//	spotLightOn = true;
//	specLightOn = true;
//	ambientLightOn = true;
//	normalsOn = true;
//
//	m_emitter = nullptr;
//	m_gpuEmitter = nullptr;
//	angleRotate = 0;
//	//loads texture
//	m_tex = new aie::Texture(filename, normalMapFilename);
//	m_soulSpearAABB = new AABB();
//	
//	//soulspear
//	tinyobj::attrib_t attribs;
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//	std::string err;
//	LoadAndAttachShaders("vsSourceTexture.vs", "fsSourceTexture.frag", "forwardShader");
//
//	bool success = tinyobj::LoadObj(&attribs, &shapes, &materials, &err, "../Models/soulspear/soulspear/soulspear.obj");
//	ObjLoader *objLoader = new ObjLoader();
//	m_glInfo = objLoader->CreateOpenGLBuffers(attribs, shapes);
//	
//	m_soulSpearAABB->Fit(attribs.vertices);
//
//	lightPosition = vec3(0, 2, 2);
//
//	//for hand animation
//	tinyobj::attrib_t handAttribs[2]; 
//	std::vector<tinyobj::shape_t> handShapes[2];
//	std::vector<tinyobj::material_t> handMaterials[2];
//
//	//load in hand mesh
//	bool success0 = tinyobj::LoadObj(&handAttribs[0], &handShapes[0], &handMaterials[0], &err, "../Models/hand/hand_00.obj");
//	bool success1 = tinyobj::LoadObj(&handAttribs[1], &handShapes[1], &handMaterials[1], &err, "../Models/hand/hand_37.obj");
//
//	for (unsigned int i = 0; i < handShapes[0].size(); i++)
//	{
//		OpenGLInfo gl;
//		std::vector<OpenGLInfo> firstVec = objLoader->CreateOpenGLBuffers(handAttribs[0], handShapes[0][i]);
//		gl.m_VBO = firstVec[0].m_VBO;
//		std::vector<OpenGLInfo> secondVec = objLoader->CreateOpenGLBuffers(handAttribs[1], handShapes[1][i]);
//		gl.m_VBO2 = secondVec[0].m_VBO;
//
//		//shapes have matching face counts
//		gl.m_faceCount = handShapes[0][i].mesh.num_face_vertices.size();
//
//		glGenVertexArrays(1, &gl.m_VAO);
//		glBindVertexArray(gl.m_VAO);
//
//		//bind first VBO
//		glBindBuffer(GL_ARRAY_BUFFER, gl.m_VBO);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), 0);
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + 12);
//		glEnableVertexAttribArray(2);
//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + 24);
//
//		//bind second VBO
//		glBindBuffer(GL_ARRAY_BUFFER, gl.m_VBO2);
//		glEnableVertexAttribArray(3); //position 2
//		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + 0); //32
//		glEnableVertexAttribArray(5); //normal 2
//		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), ((char*)0) + 12); //44
//
//		glBindVertexArray(0);
//
//		m_glInfo.push_back(gl);
//	}
//
//	delete objLoader;
//}
//
//
//Mesh::~Mesh()
//{
//	//delete m_texture;
//	if (m_tex != nullptr)
//		delete m_tex;
//
//	if (m_emitter != nullptr)
//		delete m_emitter;
//
//	if (m_gpuEmitter != nullptr)
//		delete m_gpuEmitter;
//
//	if (m_soulSpearAABB != nullptr)
//		delete m_soulSpearAABB;
//}
//
//void Mesh::GeneratePerlinValues()
//{
//	int dims = m_rows * m_columns;
//	float* perlinData = new float[dims*dims];
//
//	std::cout << dims * dims << std::endl;
//	std::cout << m_rows * dims + m_columns << std::endl;
//
//	float scale = (1.0f / dims) * 3;
//	for (int x = 0; x < m_columns; x++)
//	{
//		for (int y = 0; y < m_rows; y++)
//		{
//			perlinData[y * m_rows + x] = glm::perlin(vec2(x, y) *scale) * 0.5f + 0.5f;
//		}
//	}
//
//	glGenTextures(1, &m_perlinTexture);
//	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_rows, m_columns, 0, GL_RED, GL_FLOAT, perlinData);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	delete[] perlinData;
//}
//
//void Mesh::GenerateFrameBufferObject()
//{
//	//Post Processing Exercises
//	glGenFramebuffers(1, &m_FBO);	
//	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
//
//	glGenTextures(1, &m_FBOTexture);
//	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
//	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FBOTexture, 0);
//	glGenRenderbuffers(1, &m_FBODepth);
//
//	glBindRenderbuffer(GL_RENDERBUFFER, m_FBODepth);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_FBODepth);
//	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, drawBuffers);
//
//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	if (status != GL_FRAMEBUFFER_COMPLETE)
//		std::cout << "Framebuffer Error!" << std::endl;
//
//	//unbind the FBO so we can render to the back buffer
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	//512x512 render target exercise
//
//	////set up and bind a frame buffer
//	//glGenFramebuffers(1, &m_FBO);
//	//glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
//
//	////create a texture and bind it
//	//glGenTextures(1, &m_FBOTexture);
//	//glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
//
//	////specify texture format for storage
//	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	////attach it to the framebuffer as the first colour attachment
//	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FBOTexture, 0);
//
//	////TO DO: attach render targets here
//	//	
//	////setup and bit a 24-bit depth buffer as a render buffer
//	//glGenRenderbuffers(1, &m_FBODepth);
//	//glBindRenderbuffer(GL_RENDERBUFFER, m_FBODepth);
//	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
//
//	////while FBO is still bound
//	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_FBODepth);
//
//	////while the FBO is bound
//	//GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
//	//glDrawBuffers(1, drawBuffers);
//
//	//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	//if (status != GL_FRAMEBUFFER_COMPLETE)
//	//	std::cout << "Framebuffer Error!" << std::endl;
//
//	////unbind the FBO so we can render to the back buffer
//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//}
//
//void Mesh::GenerateShadowFrameBufferObject()
//{
//	//setup shadow map buffer
//	glGenFramebuffers(1, &m_FBOShadow);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_FBOShadow);
//
//	glGenTextures(1, &m_FBODepthShadow);
//	glBindTexture(GL_TEXTURE_2D, m_FBODepthShadow);
//
//	//texture uses a 16-bit depth component format
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	//attached as a depth attachment to capture depth not colour
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_FBODepthShadow, 0);
//
//	//no colour targets are used
//	glDrawBuffer(GL_NONE);
//
//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	if (status != GL_FRAMEBUFFER_COMPLETE)
//		printf("Framebuffer Error!\n");
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	m_lightDirection = glm::normalize(glm::vec3(1, 2.5f, 1));
//	mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
//
//	mat4 lightView = glm::lookAt(m_lightDirection, glm::vec3(0), glm::vec3(0, 1, 0));
//	
//	m_lightMatrix = lightProjection * lightView;
//
//
//}
//
//void Mesh::CreatePlane()
//{
//	//fullscreen quad
//	vec2 halfTexel = 1.0f / vec2(1280, 720) * 0.5f;
//
//	float vertexData[] =
//	{
//		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
//		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
//		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,
//
//		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
//		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
//		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y
//	};
//
//	unsigned int indexData[] =
//	{
//		0, 1, 2,
//		0, 2, 3
//	};
//
//	glGenVertexArrays(1, &m_VAO);
//	glBindVertexArray(m_VAO);
//	glGenBuffers(1, &m_VBO);
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW);	
//	glGenBuffers(1, &m_IBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indexData, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0); //position
//	glEnableVertexAttribArray(1); //uvs
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
//
//	glBindVertexArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}
//
//void Mesh::GenerateGrid(int rows, int columns)
//{
//	m_rows = rows;
//	m_columns = columns;
//	Vertex* vertexArray = new Vertex[rows*columns];
//	unsigned int* indices = new unsigned int[(rows - 1) * (columns - 1) * 6];
//
//	for (int i = 0; i < rows; i++)
//	{
//		for (int ii = 0; ii < columns; ii++)
//		{
//			vertexArray[i * columns + ii].position = vec4((float)ii, 0, (float)i, 1);
//			vertexArray[i * columns + ii].texCoord = vec2((float)ii, (float)i);
//		}
//	}
//
//
//	unsigned int index = 0;
//	for (int i = 0; i < rows - 1; i++)
//	{
//		for (int ii = 0; ii < columns - 1; ii++)
//		{
//			// triangle 1 
//			indices[index++] = i * columns + ii;
//			indices[index++] = (i + 1) * columns + ii;
//			indices[index++] = (i + 1) * columns + (ii + 1);
//			// triangle 2 
//			indices[index++] = i * columns + ii;
//			indices[index++] = (i + 1) * columns + (ii + 1);
//			indices[index++] = i * columns + (ii + 1);
//		}	
//	}
//
//	//generate vertex buffer object
//	glGenBuffers(1, &m_VBO);
//	//generate index buffer object
//	glGenBuffers(1, &m_IBO);
//
//	glGenVertexArrays(1, &m_VAO);
//	glBindVertexArray(m_VAO);
//
//	//bind and fill VBO + IBO
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//
//	glBufferData(GL_ARRAY_BUFFER, (rows * columns) * sizeof(Vertex), vertexArray, GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));
//	//added
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), ((char*)0)+16);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (columns - 1) * 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
//
//	glBindVertexArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	delete[] vertexArray;
//	delete[] indices;
//}
//
//void ShaderSyntaxCheck(std::string source, std::vector<std::string> &warningMessages)
//{
//	char nums[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
//	for (unsigned int i = 0; i < 10; i++)
//	{
//		const char* suffix_f = new const char[3] { nums[i], 'f', '\0' };
//		const char* suffix_F = new const char[3]{ nums[i], 'F', '\0' };
//
//		if (source.find(suffix_f) != -1)
//		{
//			std::stringstream ss;
//			ss << "WARNING [" << warningMessages.size() << "] " << " 'f' suffix found after floating point value, non-standard GLSL." << '\n';
//			warningMessages.push_back(ss.str());
//		}
//
//		if (source.find(suffix_F) != -1)
//		{
//			std::stringstream ss;
//			ss << "WARNING [" << warningMessages.size() << "] " << " 'F' suffix found after floating point value, non-standard GLSL." << '\n';
//			warningMessages.push_back(ss.str());
//		}
//
//		delete[] suffix_f;
//		delete[] suffix_F;
//	}
//}
//
//void Mesh::LoadShadowShaders(const char * shadowShaderVs, const char * shadowShaderFs, const char * shadowMapShaderVs, const char * shadowMapShaderFs)
//{
//	std::string vsSourceStr = LoadShader(shadowShaderVs);
//	std::string fsSourceStr = LoadShader(shadowShaderFs);
//
//	const char* vsSource = vsSourceStr.c_str();
//	const char* fsSource = fsSourceStr.c_str();
//
//	int success = GL_FALSE;
//	int compileSucess = GL_FALSE;
//
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//
//	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
//	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
//	glCompileShader(vertexShader);
//	glCompileShader(fragmentShader);
//
//	m_useShadowProgram = glCreateProgram();
//	glAttachShader(m_useShadowProgram, vertexShader);
//	glAttachShader(m_useShadowProgram, fragmentShader);
//	glLinkProgram(m_useShadowProgram);
//
//	glGetProgramiv(m_useShadowProgram, GL_LINK_STATUS, &success);
//	glGetProgramiv(m_useShadowProgram, GL_VALIDATE_STATUS, &compileSucess);
//
//	if (success == GL_FALSE)
//	{
//		std::stringstream ss;
//		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader link fail." << '\n';
//		m_errorMessages.push_back(ss.str());
//		int infoLogLength = 0;
//		glGetProgramiv(m_useShadowProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
//		char* infoLog = new char[infoLogLength];
//
//		glGetProgramInfoLog(m_useShadowProgram, infoLogLength, 0, infoLog);
//		std::cout << "Error: Failed to link shader program." << std::endl;
//		std::cout << infoLog << std::endl;
//
//		delete[] infoLog;
//	}
//
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//
//	//SHADOW MAP SHADERS
//	vsSourceStr = LoadShader(shadowMapShaderVs);
//	fsSourceStr = LoadShader(shadowMapShaderFs);
//
//	const char* vs = vsSourceStr.c_str();
//	const char* fs = fsSourceStr.c_str();
//
//	success = GL_FALSE;
//	compileSucess = GL_FALSE;
//
//	vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//
//	glShaderSource(vertexShader, 1, (const char**)&vs, 0);
//	glShaderSource(fragmentShader, 1, (const char**)&fs, 0);
//	glCompileShader(vertexShader);
//	glCompileShader(fragmentShader);
//	m_shadowGenProgram = glCreateProgram();
//	glAttachShader(m_shadowGenProgram, vertexShader);
//	glAttachShader(m_shadowGenProgram, fragmentShader);
//	glLinkProgram(m_shadowGenProgram);
//
//	glGetProgramiv(m_shadowGenProgram, GL_LINK_STATUS, &success);
//	glGetProgramiv(m_shadowGenProgram, GL_VALIDATE_STATUS, &compileSucess);
//
//	if (success == GL_FALSE)
//	{
//		std::stringstream ss;
//		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader link fail." << '\n';
//		m_errorMessages.push_back(ss.str());
//		int infoLogLength = 0;
//		glGetProgramiv(m_shadowGenProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
//		char* infoLog = new char[infoLogLength];
//
//		glGetProgramInfoLog(m_shadowGenProgram, infoLogLength, 0, infoLog);
//		std::cout << "Error: Failed to link shader program." << std::endl;
//		std::cout << infoLog << std::endl;
//
//		delete[] infoLog;	
//	}
//
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//}
//
//void Mesh::LoadAndAttachShaders(const char * vsFilename, const char * fsFilename, const char * shaderKey)
//{
//	std::string vsSourceStr = LoadShader(vsFilename);
//	std::string fsSourceStr = LoadShader(fsFilename);
//	
//	ShaderSyntaxCheck(vsSourceStr, m_warningMessages);
//	ShaderSyntaxCheck(fsSourceStr, m_warningMessages);
//
//	const char* vsSource = vsSourceStr.c_str();
//	const char* fsSource = fsSourceStr.c_str();
//
//	int success = GL_FALSE;
//	int compileSucess = GL_TRUE;
//
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
//	glCompileShader(vertexShader);
//	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
//	glCompileShader(fragmentShader);
//
//	m_shaderMap[shaderKey] = glCreateProgram();
//	glAttachShader(m_shaderMap[shaderKey], vertexShader);
//	glAttachShader(m_shaderMap[shaderKey], fragmentShader);
//	glLinkProgram(m_shaderMap[shaderKey]);
//
//	glGetProgramiv(m_shaderMap[shaderKey], GL_LINK_STATUS, &success);
//	glGetProgramiv(m_shaderMap[shaderKey], GL_VALIDATE_STATUS, &compileSucess);
//
//	if (compileSucess == GL_FALSE)
//	{
//		std::cout << "Compile fail! Forward shaders" << std::endl;
//		std::stringstream ss;
//		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader compile fail." << '\n';
//
//		m_errorMessages.push_back(ss.str());
//	}
//
//	if (success == GL_FALSE)
//	{
//		std::stringstream ss;
//		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader link fail." << '\n';
//		m_errorMessages.push_back(ss.str());
//		int infoLogLength = 0;
//		glGetProgramiv(m_shaderMap[shaderKey], GL_INFO_LOG_LENGTH, &infoLogLength);
//		char* infoLog = new char[infoLogLength];
//
//		glGetProgramInfoLog(m_shaderMap[shaderKey], infoLogLength, 0, infoLog);
//		std::cout << "Error: Failed to link shader program." << std::endl;
//		std::cout << infoLog << std::endl;
//			
//		delete[] infoLog;
//	}
//
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//}
//
//void Mesh::Draw(mat4 projectionView, float time)
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
//	glViewport(0, 0, 1280, 720);
//
//	vec4 clearColour(0.75f, 0.75f, 0.75f, 1);
//	//ImGui::TextColored(ImVec4(1,0,0,1), "Some text");
//
////glClearColor(clearColour.x, clearColour.y, clearColour.z, 1);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	Gizmos::draw(projectionView);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, 1280, 720);
//	ImGui::Begin("Error Console: ");
//	for (unsigned int i = 0; i < m_errorMessages.size(); i++)
//	{
//		ImGui::TextColored(ImVec4(1, 0, 0, 1), m_errorMessages[i].c_str());
//	}
//
//	for (unsigned int i = 0; i < m_warningMessages.size(); i++)
//	{
//		ImGui::TextColored(ImVec4(1, 1, 0, 1), m_warningMessages[i].c_str());
//	}
//
//	ImGui::End();
//
//	//glClearColor(0.25f, 0.25f, 0.25f, 1);
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//	glUseProgram(m_programID);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
//	int location = glGetUniformLocation(m_programID, "target");
//	glUniform1i(location, 0);
//
//	location = glGetUniformLocation(m_programID, "time");
//	glUniform1f(location, time);
//
//	glBindVertexArray(m_VAO);
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//}
//
//void Mesh::DrawForwardRendering(float time, mat4 &projectionView, mat4 &cameraWorld, const char* shaderKey, const char* morphShader)
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
//	glViewport(0, 0, 1280, 720);
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	Gizmos::draw(projectionView);
//
//	glUseProgram(m_shaderMap[shaderKey]);
//
//	int location = glGetUniformLocation(m_shaderMap[shaderKey], "keyTime");
//	glUniform1f(location, cosf(time) * 0.5f + 0.5f);
//
//	unsigned int projectionViewTransform = glGetUniformLocation(m_shaderMap[shaderKey], "projectionViewWorldMatrix");
//	glUniformMatrix4fv(projectionViewTransform, 1, GL_FALSE, glm::value_ptr(projectionView));
//
//	//give worrd (view) matrix to vertex shader so normals can be calculated correctly (they can be moved according to the model's position, orientation etc.)
//
//	unsigned int cameraWorldTransform = glGetUniformLocation(m_shaderMap[shaderKey], "cameraWorld");
//	glUniformMatrix4fv(cameraWorldTransform, 1, GL_FALSE, glm::value_ptr(cameraWorld));
//
//	if (m_tex != nullptr)
//	{
//		//set texture slot
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, m_tex->getHandle());
//
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, m_tex->getNormalHandle());
//	}
//
//	//tell the shader where it is
//	unsigned int diffuse = glGetUniformLocation(m_shaderMap[shaderKey], "diffuse");
//	glUniform1i(diffuse, 0);
//
//	//set normal sampler
//	unsigned int normal = glGetUniformLocation(m_shaderMap[shaderKey], "normal");
//	glUniform1i(normal, 1);
//
//	//set light direction uniform
//	unsigned int lightDir = glGetUniformLocation(m_shaderMap[shaderKey], "lightDirection");
//	vec3 lightDirection(1, 0, 0);
//	glUniform3fv(lightDir, 1, glm::value_ptr(lightDirection));
//	aie::Input *input = aie::Input::getInstance();
//	if (input->isKeyDown(aie::INPUT_KEY_UP))
//	{
//		lightPosition.y += 0.1f;
//	}
//
//	else if (input->isKeyDown(aie::INPUT_KEY_DOWN))
//	{
//		lightPosition.y -= 0.1f;
//	}
//
//	angleRotate = 0.01f;
//	//set y rotation matrix
//	unsigned int yRot = glGetUniformLocation(m_shaderMap[shaderKey], "rotateOnY");
//	yRotate = glm::rotate(yRotate, angleRotate, glm::vec3(0, 1, 0));
//	glUniformMatrix4fv(yRot, 1, GL_FALSE, glm::value_ptr(yRotate));
//
//	//set light pos
//	unsigned int lightPos = glGetUniformLocation(m_shaderMap[shaderKey], "lightPos");
//	//vec3 lightPosition(0, 2, 0);
//	glUniform3fv(lightPos, 1, glm::value_ptr(lightPosition));
//
//	//set light colour
//	unsigned int lightColour = glGetUniformLocation(m_shaderMap[shaderKey], "lightColour");
//	vec3 lightColourVec(0, 1, 1);
//	glUniform3fv(lightColour, 1, glm::value_ptr(lightColourVec));
//
//	//set ambient light colour
//	unsigned int ambientLight = glGetUniformLocation(m_shaderMap[shaderKey], "ambientColour");
//	vec3 ambientLightColour(1, 1, 1);
//	glUniform3fv(ambientLight, 1, glm::value_ptr(ambientLightColour));
//
//	//set ambient light scale
//	unsigned int ambientLightScale = glGetUniformLocation(m_shaderMap[shaderKey], "ambientColourScale");
//	vec3 ambientScale(0.05f, 0.05f, 0.05f);
//	glUniform3fv(ambientLightScale, 1, glm::value_ptr(ambientScale));
//
//	//set cone angle
//	unsigned int coneAngle = glGetUniformLocation(m_shaderMap[shaderKey], "coneAngle");
//	glUniform1f(coneAngle, 90.0f);
//
//	//set attenuation
//	unsigned int attenuation = glGetUniformLocation(m_shaderMap[shaderKey], "attenuation");
//	glUniform1f(attenuation, 3.0f);
//
//	//set camera pos
//	unsigned int cameraPos = glGetUniformLocation(m_shaderMap[shaderKey], "cameraPos");
//	glUniform3fv(cameraPos, 1, glm::value_ptr(vec3(cameraWorld[3][0], cameraWorld[3][1], cameraWorld[3][2])));
//
//	//set specular power
//	float specPower = 32.0f;
//	unsigned int specularPower = glGetUniformLocation(m_shaderMap[shaderKey], "specPower");
//	glUniform1f(specularPower, specPower);
//
//	unsigned int light = glGetUniformLocation(m_shaderMap[shaderKey], "spotLightOn");
//	glUniform1i(light, spotLightOn);
//	light = glGetUniformLocation(m_shaderMap[shaderKey], "specOn");
//	glUniform1i(light, specLightOn);
//	light = glGetUniformLocation(m_shaderMap[shaderKey], "ambientOn");
//	glUniform1i(light, ambientLightOn);
//
//	unsigned int normalOn = glGetUniformLocation(m_shaderMap[shaderKey], "normalsOn");
//	glUniform1i(normalOn, normalsOn);
//
//	
//	//draw soulspear
//	glBindVertexArray(m_glInfo[0].m_VAO);
//	glDrawArrays(GL_TRIANGLES, 0, m_glInfo[0].m_faceCount * 3);
//
//	glUseProgram(m_shaderMap[morphShader]);
//
//	ambientLightColour = vec3(0, 0, 1);
//	ambientLight = glGetUniformLocation(m_shaderMap[morphShader], "ambientColour");
//	glUniform3fv(ambientLight, 1, glm::value_ptr(ambientLightColour));
//
//	//set ambient light scale
//	ambientLightScale = glGetUniformLocation(m_shaderMap[morphShader], "ambientColourScale");
//	ambientScale = vec3(0.5f, 0.5f, 0.5f);
//	glUniform3fv(ambientLightScale, 1, glm::value_ptr(ambientScale));
//
//
//	location = glGetUniformLocation(m_shaderMap[morphShader], "keyTime");
//	glUniform1f(location, cosf(time) * 0.5f + 0.5f);
//
//	projectionViewTransform = glGetUniformLocation(m_shaderMap[morphShader], "projectionViewWorldMatrix");
//	glUniformMatrix4fv(projectionViewTransform, 1, GL_FALSE, glm::value_ptr(projectionView));
//
//	for (unsigned int i = 1; i < m_glInfo.size(); i++)
//	{
//		glBindVertexArray(m_glInfo[i].m_VAO);
//		glDrawArrays(GL_TRIANGLES, 0, m_glInfo[i].m_faceCount * 3);
//	}
//
//}
//
//void Mesh::DrawPost(float time, mat4 &projectionView, const char* postShaderKey)
//{	
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glViewport(0, 0, 1280, 720);
//	ImGui::Begin("Munib's application: ");
//	for (unsigned int i = 0; i < m_errorMessages.size(); i++)
//	{
//		ImGui::TextColored(ImVec4(1, 0, 0, 1), m_errorMessages[i].c_str());
//	}
//
//	for (unsigned int i = 0; i < m_warningMessages.size(); i++)
//	{
//		ImGui::TextColored(ImVec4(1, 1, 0, 1), m_warningMessages[i].c_str());
//	}
//	ImGui::Checkbox("SpotLight On", &spotLightOn);
//	ImGui::Checkbox("Spec On", &specLightOn);
//	ImGui::Checkbox("Ambient On", &ambientLightOn);
//	ImGui::Checkbox("Normals On", &normalsOn);
//
//	ImGui::Checkbox("None", &noPostProcess);
//
//	if (noPostProcess == true)
//	{
//		scanLines = false;
//		grayScale = false;
//		distort = false;
//	}
//
//	ImGui::Checkbox("Scan Lines", &scanLines);
//
//	if (scanLines == true)
//	{
//		noPostProcess = false;
//		grayScale = false;
//		distort = false;
//	}
//
//	ImGui::Checkbox("Gray Scale", &grayScale);
//
//	if (grayScale == true)
//	{
//		noPostProcess = false;
//		scanLines = false;
//		distort = false;
//	}
//
//	ImGui::Checkbox("Distort", &distort);
//	if (distort == true)
//	{
//		noPostProcess = false;
//		grayScale = false;
//		scanLines = false;
//	}
//
//	ImGui::End();
//
//	glClear(GL_DEPTH_BUFFER_BIT);
//
//	glUseProgram(m_shaderMap[postShaderKey]);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
//	int location = glGetUniformLocation(m_shaderMap[postShaderKey], "target");
//	glUniform1i(location, 0);
//	location = glGetUniformLocation(m_shaderMap[postShaderKey], "time");
//	glUniform1f(location, time);
//
//	unsigned int postLoc = glGetUniformLocation(m_shaderMap[postShaderKey], "noPostProcess");
//	glUniform1i(postLoc, noPostProcess);
//
//	postLoc = glGetUniformLocation(m_shaderMap[postShaderKey], "scanLines");
//	glUniform1i(postLoc, scanLines);
//
//	postLoc = glGetUniformLocation(m_shaderMap[postShaderKey], "grayScale");
//	glUniform1i(postLoc, grayScale);
//
//	postLoc = glGetUniformLocation(m_shaderMap[postShaderKey], "distort");
//	glUniform1i(postLoc, distort);
//
//	glBindVertexArray(m_VAO);
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//}
//
//
//void Mesh::Draw(float time, mat4 &projectionView, mat4 &cameraWorld, const char* forwardShaderKey, const char* postShaderKey, const char *morphShaderKey)
//{
//	DrawForwardRendering(time, projectionView, cameraWorld, forwardShaderKey, morphShaderKey);
//	DrawPost(time, projectionView, postShaderKey);
//
//}
//
//void Mesh::Draw(mat4 projectionView, float t, float height)
//{
//	glUseProgram(m_programID);
//
//	unsigned int projectionViewTransform = glGetUniformLocation(m_programID, "projectionViewWorldMatrix");
//	unsigned int heightScale = glGetUniformLocation(m_programID, "heightScale");
//	unsigned int time = glGetUniformLocation(m_programID, "time");
//	glUniformMatrix4fv(projectionViewTransform, 1, false, glm::value_ptr(projectionView));
//	glUniform1f(heightScale, height);
//	glUniform1f(time, t);
//
//	glBindVertexArray(m_VAO);
//	unsigned int indexCount = (m_rows - 1) * (m_columns - 1) * 6;
//
//	//what shape to draw, number of triangles, datatype in index buffer, offset (where to start))
//	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
//}
//
//void Mesh::DrawPlane(mat4 projectionView, const char* shaderKey)
//{
//	glUseProgram(m_shaderMap[shaderKey]);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_perlinTexture);
//	
//	unsigned int loc = glGetUniformLocation(m_shaderMap[shaderKey], "projectionViewWorldMatrix");
//	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(projectionView));
//
//	loc = glGetUniformLocation(m_shaderMap[shaderKey], "perlinTexture");
//	glUniform1i(loc, 0);
//
//	glBindVertexArray(m_VAO);
//	unsigned int indexCount = (m_rows - 1) * (m_columns - 1) * 6;
//	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
//
//}
//
//ParticleEmitter* Mesh::GetEmitter()
//{
//	// TODO: insert return statement here
//	return m_emitter;
//}
//
//GPUParticleEmitter * Mesh::GetGPUEmitter()
//{
//	return m_gpuEmitter;
//}
//
//std::string Mesh::LoadShader(const char * filename)
//{
//	std::ifstream myFile;
//	std::string line;
//	std::string output;
//
//	myFile.open(filename);
//
//	if (myFile.is_open())
//	{
//		while (!myFile.eof())
//		{
//			std::getline(myFile, line);
//			output += '\n' + line;
//		}
//
//		myFile.close();
//	}
//
//	else
//	{
//		std::stringstream ss;
//		ss << "ERROR [" << m_errorMessages.size() << "]" << " Shader source file not found: " << "\"" << filename << "\"" << '\n';
//		m_errorMessages.push_back(ss.str());
//	}
//
//
//	return output;
//}

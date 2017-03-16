#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
{
	m_particles = nullptr;
	m_firstDeadIndex = 0;
	m_maxParticles = 0;

	m_position = vec3(0, 0, 0);
	m_gravity = 0;

	m_vertexData = nullptr;
	m_VBO = 0;
	m_VAO = 0;
	m_IBO = 0;
}


ParticleEmitter::~ParticleEmitter()
{
	delete[] m_particles;
	delete[] m_vertexData;

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
}

void ParticleEmitter::Initialize(unsigned int maxParticles, float emitRate, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, float gravity, const vec4 &startColour, const vec4 &endColour)
{
	m_maxParticles = maxParticles;
	m_emitRate = 1/emitRate;
	m_emitTimer = 0;	
	m_lifespanMin = lifespanMin;
	m_lifespanMax = lifespanMax;
	m_velocityMin = velocityMin;
	m_velocityMax = velocityMax;
	m_startSize = startSize;
	m_endSize = endSize;
	m_startColour = startColour;
	m_endColour = endColour;
	m_gravity = gravity;

	//create particle array
	m_particles = new Particle[m_maxParticles];
	m_vertexData = new ParticleVertex[m_maxParticles * 4];

	//create index data
	unsigned int *indexData = new unsigned int[m_maxParticles * 6]; //two triangles per face, 3 indices per triangle
	for (unsigned int i = 0; i < m_maxParticles; i++)
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;

		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;	
	}

	//create OpenGL buffers
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(ParticleVertex), m_vertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 6 * sizeof(unsigned int), indexData, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 16);
	//particle vertex needs UVs

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;
}

void ParticleEmitter::Emit()
{
	if (m_firstDeadIndex >= m_maxParticles)
		return;

	Particle& particle = m_particles[m_firstDeadIndex++]; //get first dead then increment

	particle.m_position = m_position; //set to the position of the emit
	
	particle.m_lifetime = 0;
	particle.m_lifespan = (rand() / (float)RAND_MAX) * (m_lifespanMax - m_lifespanMin) + m_lifespanMin;
	
	particle.m_colour = m_startColour;
	particle.m_size = m_startSize;

	//randomise velocity and strength
	float velocity = (rand() / RAND_MAX) * (m_velocityMax - m_velocityMin) + m_velocityMin;

	particle.m_velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.m_velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.m_velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;

	particle.m_velocity = glm::normalize(particle.m_velocity) * velocity;
}

void ParticleEmitter::Update(float deltaTime, mat4 cameraTransform)
{
	m_emitTimer += deltaTime;
	while (m_emitTimer > m_emitRate) //while loop allows for multiple particles to spawn within one frame
	{
		Emit();
		m_emitTimer -= m_emitRate;
	}

	unsigned int quad = 0;

	//iterate through all particles
	for (unsigned int i = 0; i < m_firstDeadIndex; i++)
	{
		Particle* particle = &m_particles[i];
		particle->m_lifetime += deltaTime;
		if (particle->m_lifetime > particle->m_lifespan)
		{
			*particle = m_particles[m_firstDeadIndex - 1]; //swap with last alive
			m_firstDeadIndex--; //one more dead particle, so decrement
		}

		else
		{
			//move particle
			particle->m_position += particle->m_velocity * deltaTime;
			particle->m_position -= vec3(0, m_gravity, 0);
			particle->m_size = glm::mix(m_startSize, m_endSize, particle->m_lifespan / particle->m_lifespan);

			//colour particle
			particle->m_colour = glm::mix(m_startColour, m_endColour, particle->m_lifetime / particle->m_lifespan);
			
			//make a quad the correct size and colour
			float halfSize = particle->m_size * 0.5f;

			m_vertexData[quad * 4 + 0].position = vec4(halfSize, halfSize, 0, 1);   
			m_vertexData[quad * 4 + 0].colour = particle->m_colour;
			m_vertexData[quad * 4 + 1].position = vec4(-halfSize, halfSize, 0, 1);  
			m_vertexData[quad * 4 + 1].colour = particle->m_colour;
			m_vertexData[quad * 4 + 2].position = vec4(-halfSize, -halfSize, 0, 1);   
			m_vertexData[quad * 4 + 2].colour = particle->m_colour;
			m_vertexData[quad * 4 + 3].position = vec4(halfSize, -halfSize, 0, 1);   
			m_vertexData[quad * 4 + 3].colour = particle->m_colour;
		
			//create billboard transform
			vec3 zAxis = glm::normalize(vec3(cameraTransform[3]) - particle->m_position);  
			vec3 xAxis = glm::cross(vec3(cameraTransform[1]), zAxis);   
			vec3 yAxis = glm::cross(zAxis, xAxis);   
			glm::mat4 billboard(vec4(xAxis, 0), vec4(yAxis, 0), vec4(zAxis, 0), vec4(0, 0, 0, 1));
			m_vertexData[quad * 4 + 0].position = billboard * m_vertexData[quad * 4 + 0].position + vec4(particle->m_position, 0);
			m_vertexData[quad * 4 + 1].position = billboard * m_vertexData[quad * 4 + 1].position + vec4(particle->m_position, 0);   
			m_vertexData[quad * 4 + 2].position = billboard *  m_vertexData[quad * 4 + 2].position + vec4(particle->m_position, 0);  
			m_vertexData[quad * 4 + 3].position = billboard *  m_vertexData[quad * 4 + 3].position + vec4(particle->m_position, 0);
			quad++;
		}
	}
}

void ParticleEmitter::Test()
{
}

void ParticleEmitter::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDeadIndex * 4 * sizeof(ParticleVertex), m_vertexData);
		
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_firstDeadIndex * 6, GL_UNSIGNED_INT, 0);

}

void ParticleEmitter::InitializeCone(unsigned int maxParticles, float emitRate, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, float gravity, float coneAngle, const vec3 &coneDir, const vec4 & startColour, const vec4 & endColour)
{
	m_maxParticles = maxParticles;
	m_emitRate = 1 / emitRate;
	m_emitTimer = 0;
	m_lifespanMin = lifespanMin;
	m_lifespanMax = lifespanMax;
	m_velocityMin = velocityMin;
	m_velocityMax = velocityMax;
	m_startSize = startSize;
	m_endSize = endSize;
	m_startColour = startColour;
	m_endColour = endColour;
	m_gravity = gravity;
	m_coneDir = coneDir;
	//create particle array
	m_particles = new Particle[m_maxParticles];
	m_vertexData = new ParticleVertex[m_maxParticles * 4];

	m_coneDir = coneDir;
	m_coneAngle = coneAngle;

	//create index data
	unsigned int *indexData = new unsigned int[m_maxParticles * 6]; //two triangles per face, 3 indices per triangle
	for (unsigned int i = 0; i < m_maxParticles; i++)
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;

		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	//create OpenGL buffers
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * 4 * sizeof(ParticleVertex), m_vertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxParticles * 6 * sizeof(unsigned int), indexData, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleVertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;
}

void ParticleEmitterCone::Test()
{
	std::cout << "Worked!" << std::endl;
}

void ParticleEmitterCone::Emit()
{
	if (m_firstDeadIndex >= m_maxParticles)
		return;

	m_coneDir = vec3(0, 1, 0);

	Particle& particle = m_particles[m_firstDeadIndex++]; //get first dead then increment

	particle.m_position = m_position; //set to the position of the emit

	particle.m_lifetime = 0;
	particle.m_lifespan = (rand() / (float)RAND_MAX) * (m_lifespanMax - m_lifespanMin) + m_lifespanMin;

	particle.m_colour = m_startColour;
	particle.m_size = m_startSize;

	//randomise velocity and strength
	float velocity = (rand() / RAND_MAX) * (m_velocityMax - m_velocityMin) + m_velocityMin;

	particle.m_velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.m_velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.m_velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;

	float angleDiff = glm::acos(glm::dot(glm::normalize(m_coneDir), glm::normalize(particle.m_velocity)));
	while (angleDiff > m_coneAngle)
	{
		particle.m_velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
		particle.m_velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
		particle.m_velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;
		angleDiff = acos(glm::dot(glm::normalize(m_coneDir), glm::normalize(particle.m_velocity)));
	}


	particle.m_velocity = glm::normalize(particle.m_velocity) * velocity;
}

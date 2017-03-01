#include "ObjLoader.h"


ObjLoader::ObjLoader()
{
}


ObjLoader::~ObjLoader()
{
}

std::vector<OpenGLInfo> ObjLoader::CreateOpenGLBuffers(tinyobj::attrib_t & attribs, std::vector<tinyobj::shape_t>& shapes)
{
	m_glInfo.resize(shapes.size()); // grab each shape 
	int shapeIndex = 0;
	for (auto& shape : shapes)
	{
		// setup OpenGL data 
		glGenVertexArrays(1, &m_glInfo[shapeIndex].m_VAO);
		glGenBuffers(1, &m_glInfo[shapeIndex].m_VBO);
		glBindVertexArray(m_glInfo[shapeIndex].m_VAO);
		m_glInfo[shapeIndex].m_faceCount = shape.mesh.num_face_vertices.size();
		// collect triangle vertices 
		std::vector<OBJVertex> vertices;
		int index = 0;

		/*if (shapeIndex == 0)
		{
			shapeIndex++;
			continue;
		}*/
		for (auto face : shape.mesh.num_face_vertices)
		{
			for (int i = 0; i < 3; ++i)
			{
				tinyobj::index_t idx = shape.mesh.indices[index + i];
				OBJVertex v = { 0 }; // positions 
				v.x = attribs.vertices[3 * idx.vertex_index + 0];
				v.y = attribs.vertices[3 * idx.vertex_index + 1];
				v.z = attribs.vertices[3 * idx.vertex_index + 2];
				// normals 
				if (attribs.normals.size() > 0)
				{
					v.nx = attribs.normals[3 * idx.normal_index + 0];
					v.ny = attribs.normals[3 * idx.normal_index + 1];
					v.nz = attribs.normals[3 * idx.normal_index + 2];
				}

				//tangents

				// texture coordinates 

				if (attribs.texcoords.size() > 0)
				{
					v.u = attribs.texcoords[2 * idx.texcoord_index + 0];
					v.v = -attribs.texcoords[2 * idx.texcoord_index + 1];
				}
				vertices.push_back(v);
			}
			index += face;
		}
		CalculateTangents(vertices);

		// bind vertex data 
		glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[shapeIndex].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(OBJVertex), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		//position 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), 0);
		glEnableVertexAttribArray(1); //normal data 
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), (void*)12);
		glEnableVertexAttribArray(2); //texture data 
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (void*)24);
		glEnableVertexAttribArray(3); //tangent data
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (void*)32);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
		shapeIndex++;
	}

	return m_glInfo;
}

void ObjLoader::CalculateTangents(std::vector<OBJVertex>& vertices)
{
	unsigned int vertexCount = (unsigned int)vertices.size();
	glm::vec4* tan1 = new glm::vec4[vertexCount * 2];
	glm::vec4* tan2 = tan1 + vertexCount;
	memset(tan1, 0, vertexCount * sizeof(glm::vec4) * 2);

	unsigned int indexCount = (unsigned int)vertices.size();
	for (unsigned int a = 0; a < indexCount; a += 3) {
		//because our obj doesn't use indices we use the vertex index (each three vertices is a triangle)
		long i1 = a;
		long i2 = a + 1;
		long i3 = a + 2;

		const glm::vec4& v1 = glm::vec4(vertices[i1].x, vertices[i1].y, vertices[i1].z, 1);
		const glm::vec4& v2 = glm::vec4(vertices[i2].x, vertices[i2].y, vertices[i2].z, 1);
		const glm::vec4& v3 = glm::vec4(vertices[i3].x, vertices[i3].y, vertices[i3].z, 1);

		const glm::vec2& w1 = glm::vec2(vertices[i1].u, 1.0f - vertices[i1].v); //v component is upside down in soul spear
		const glm::vec2& w2 = glm::vec2(vertices[i2].u, 1.0f - vertices[i2].v);
		const glm::vec2& w3 = glm::vec2(vertices[i3].u, 1.0f - vertices[i3].v);

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		glm::vec4 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r, 0);
		glm::vec4 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r, 0);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (unsigned int a = 0; a < vertexCount; a++) {
		const glm::vec3& n = glm::vec3(vertices[a].nx, vertices[a].ny, vertices[a].nz);
		const glm::vec3& t = glm::vec3(tan1[a]);
		glm::vec4 tangent;
		// Gram-Schmidt orthogonalize
		tangent = glm::vec4(glm::normalize(t - n * glm::dot(n, t)), 0);

		// Calculate handedness (direction of bitangent)
		tangent.w = (glm::dot(glm::cross(glm::vec3(n), glm::vec3(t)), glm::vec3(tan2[a])) < 0.0F) ? 1.0F : -1.0F;
		vertices[a].tx = tangent.x;
		vertices[a].ty = tangent.y;
		vertices[a].tz = tangent.z;
		vertices[a].tw = tangent.w;
	}
	delete[] tan1;
}


void ObjLoader::Render()
{

};

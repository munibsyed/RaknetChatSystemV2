#include "MyTexture.h"


//
//MyTexture::MyTexture(const char* path)
//{
//	imageWidth = 0;
//	imageHeight = 0;
//	imageFormat = 0;
//
//	data = stbi_load(path, &imageWidth, &imageHeight, &imageFormat, STBI_default);
//	glGenTextures(1, &m_texture);
//	glBindTexture(GL_TEXTURE_2D, m_texture);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	stbi_image_free(data);
//}
//
//
//MyTexture::~MyTexture()
//{
//}

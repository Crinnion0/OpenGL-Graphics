#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "..\..\nclgl\mesh.h"

#define RAW_WIDTH 257
#define RAW_HEIGHT 257

#define HEIGHTMAP_X 32.0f
#define HEIGHTMAP_Z 32.0f
#define HEIGHTMAP_Y 5.25f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f

class HeightMap : public Mesh {
public:
	HeightMap(std::string name);
	~HeightMap(void) {};

	virtual void Draw();

	inline void setTexture2(GLuint texture2) { this->texture2 = texture2; }
	inline GLuint getTexture2() { return texture2; }

protected:
	GLuint texture2;

};

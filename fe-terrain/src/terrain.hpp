#include <vector>
#include "util/FastNoise.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct HeightMap{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	unsigned int quadNumber;
};

struct Vertex{
	GLfloat x;
	GLfloat y;
	GLfloat z;
	std::vector<unsigned int> adjacent_triangles;
	glm::vec3 vertex_normal;
};

struct Triangle{
	Vertex A;
	Vertex B;
	Vertex C;
	glm::vec3 surface_normal;
};


std::vector<unsigned char> noiseMap(unsigned int seed);

HeightMap generatePerlinNoiseMap(unsigned int width, unsigned int height, unsigned int xoffset, unsigned int zoffset, GLuint quadCount, GLfloat unitsize, unsigned int seed, unsigned int terraintype);

HeightMap horizontalTerrainSmoothener(unsigned int width, unsigned int height, unsigned int xoffset, unsigned int zoffset, GLuint quadCount, GLfloat unitsize, unsigned int seed, GLfloat leftScaling, GLfloat rightScaling);

HeightMap verticalTerrainSmoothener(unsigned int width, unsigned int height, unsigned int xoffset, unsigned int zoffset, GLuint quadCount, GLfloat unitsize, unsigned int seed, GLfloat leftScaling, GLfloat rightScaling);
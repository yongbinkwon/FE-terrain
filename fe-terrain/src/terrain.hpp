#include <vector>
#include "util/FastNoise.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

struct HeightMap{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
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

HeightMap generatePerlinNoiseMap(unsigned int width, unsigned int height, GLfloat unitsize, unsigned int seed);
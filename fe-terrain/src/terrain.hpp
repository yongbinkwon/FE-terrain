#include <vector>
#include "util/FastNoise.h"
#include <glad/glad.h>

struct HeightMap{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
};

std::vector<unsigned char> noiseMap(unsigned int seed);

HeightMap generatePerlinNoiseMap(unsigned int width, unsigned int height, GLfloat unitsize, unsigned int seed);
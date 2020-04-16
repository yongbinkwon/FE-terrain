#define _USE_MATH_DEFINES
#include "terrain.hpp"
#include <cmath>  

#include <iostream>


std::vector<GLfloat> gradient(GLfloat height) {
	std::vector<GLfloat> blendMap;
	//rgb where r is the 0 texture, g is 1 texture and b 2 texture
	if(height >= 0.75f) {
		blendMap.push_back(0.0f);
		blendMap.push_back(0.0f);
		blendMap.push_back(1.0f);
	}
	else if(height >= 0.5f) {
		GLfloat linfunc = 4.0f*(height-0.5f);
		blendMap.push_back(0.0f);
		blendMap.push_back(1.0f-linfunc);
		blendMap.push_back(linfunc);
	}
	else if(height > 0.25f) {
		GLfloat linfunc = 4.0f*(height-0.25f);
		blendMap.push_back(1.0f-linfunc);
		blendMap.push_back(linfunc);
		blendMap.push_back(0.0f);
	}
	else {
		blendMap.push_back(1.0f);
		blendMap.push_back(0.0f);
		blendMap.push_back(0.0f);
	}
	return blendMap;
}


std::vector<unsigned char> noiseMap(unsigned int seed) {
	FastNoise noise;
	noise.SetNoiseType(FastNoise::SimplexFractal);
	noise.SetFractalOctaves(5);
	noise.SetFractalGain(0.5f);
	noise.SetFrequency(0.01f);
	noise.SetSeed(seed);
	std::vector<unsigned char> pixels;
	for(unsigned int y=0; y<256; y++) {
		for(unsigned int x=0; x<256; x++) {
			unsigned char noise_val = 255*(((noise.GetNoise(x, y)+1)/4)+0.5);
			pixels.push_back(noise_val);
			pixels.push_back(noise_val);
			pixels.push_back(noise_val);
			pixels.push_back(255);
		}
	}
	return pixels;
}


//unitsize is how many coordinates each height/width unit refers to
HeightMap generatePerlinNoiseMap(unsigned int width, unsigned int height, GLfloat unitsize, unsigned int seed) {
	//number of vertices are (width+1)*(height+1) then you have 3 coordinates as well
	std::vector<GLfloat> vertices;
	GLfloat zoffset = 1.0f;
	GLfloat scalingFactor = 50.0f;
	GLuint quadNumber = 0;
	FastNoise noise;
	noise.SetNoiseType(FastNoise::SimplexFractal);
	noise.SetFractalOctaves(5);
	noise.SetFractalGain(0.5f);
	noise.SetSeed(seed);
	std::vector<GLuint> indices;
	for(unsigned int z=0; z<height; z++) {
		GLfloat t = (GLfloat)z / (GLfloat)height;
		for(unsigned int x=0; x<width; x++) {
			GLfloat s = (GLfloat)x / (GLfloat)width;
			//botLeft
			GLfloat x_coord = x*unitsize;
			GLfloat z_coord = -(z+zoffset)*unitsize;
			GLfloat noise_val = (noise.GetNoise(x_coord, z_coord)+1)/2;
			//xyz
			vertices.push_back(x_coord);
			vertices.push_back(scalingFactor*noise_val);
			vertices.push_back(z_coord);
			//uv
			vertices.push_back(0.0f);
			vertices.push_back(0.0f);
			//blendmap
			std::vector<GLfloat> blendMap = gradient(noise_val);
			vertices.push_back(blendMap.at(0));
			vertices.push_back(blendMap.at(1));
			vertices.push_back(blendMap.at(2));
			//st, aka noise texcoords
			vertices.push_back(s);
			vertices.push_back(t);
			
			//botRight
			x_coord = (x+1)*unitsize;
			z_coord = -(z+zoffset)*unitsize;
			noise_val = (noise.GetNoise(x_coord, z_coord)+1)/2;
			//xyz
			vertices.push_back(x_coord);
			vertices.push_back(scalingFactor*noise_val);
			vertices.push_back(z_coord);
			//uv
			vertices.push_back(1.0f);
			vertices.push_back(0.0f);
			//blendmap
			blendMap = gradient(noise_val);
			vertices.push_back(blendMap.at(0));
			vertices.push_back(blendMap.at(1));
			vertices.push_back(blendMap.at(2));
			//st
			vertices.push_back(s+(1/width));
			vertices.push_back(t);
			
			//topRight
			x_coord = (x+1)*unitsize;
			z_coord = -(z+1+zoffset)*unitsize;
			noise_val = (noise.GetNoise(x_coord, z_coord)+1)/2;
			//xyz
			vertices.push_back(x_coord);
			vertices.push_back(scalingFactor*noise_val);
			vertices.push_back(z_coord);
			//uv
			vertices.push_back(1.0f);
			vertices.push_back(1.0f);
			//blendmap
			blendMap = gradient(noise_val);
			vertices.push_back(blendMap.at(0));
			vertices.push_back(blendMap.at(1));
			vertices.push_back(blendMap.at(2));
			//st
			vertices.push_back(s+(1/width));
			vertices.push_back(t+(1/height));
			
			//topLeft
			x_coord = x*unitsize;
			z_coord = -(z+1+zoffset)*unitsize;
			noise_val = (noise.GetNoise(x_coord, z_coord)+1)/2;
			//xyz
			vertices.push_back(x_coord);
			vertices.push_back(scalingFactor*noise_val);
			vertices.push_back(z_coord);
			//uv
			vertices.push_back(0.0f);
			vertices.push_back(1.0f);
			//blendmap
			blendMap = gradient(noise_val);
			vertices.push_back(blendMap.at(0));
			vertices.push_back(blendMap.at(1));
			vertices.push_back(blendMap.at(2));
			//st
			vertices.push_back(s);
			vertices.push_back(t+(1/height));
			/*
			//rgb
			std::cout << noise_val << "-" << gradient(noise_val) << ",  ";
			GLfloat color = gradient(noise_val);
			vertices.push_back(color);
			vertices.push_back(1.0f);
			vertices.push_back(color);
			*/
			/*
			botLeft = 0
			botRight = 1
			topRight = 2
			topLeft = 3
			*/
			indices.push_back(4*quadNumber);
			indices.push_back(4*quadNumber + 1);
			indices.push_back(4*quadNumber + 2);
			indices.push_back(4*quadNumber + 2);
			indices.push_back(4*quadNumber + 3);
			indices.push_back(4*quadNumber);
			quadNumber++;
		}
		
	}
	HeightMap heightMap = {
		vertices,
		indices,
	};
	
	return heightMap;
}
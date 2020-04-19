#define _USE_MATH_DEFINES
#include "terrain.hpp"
#include <cmath>  

#include <iostream>


glm::vec3 gradient(GLfloat height) {
	glm::vec3 blendMap;
	//rgb where r is the 0 texture, g is 1 texture and b 2 texture
	if(height >= 0.75f) {
		blendMap = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	else if(height >= 0.5f) {
		GLfloat linfunc = 4.0f*(height-0.5f);
		blendMap = glm::vec3(0.0f, 1.0f-linfunc, linfunc);
	}
	else if(height > 0.25f) {
		GLfloat linfunc = 4.0f*(height-0.25f);
		blendMap = glm::vec3(1.0f-linfunc, linfunc, 0.0f);
	}
	else {
		blendMap = glm::vec3(1.0f, 0.0f, 0.0f);
	}
	return blendMap;
}


std::vector<unsigned char> noiseMap(unsigned int seed) {
	FastNoise noise;
	noise.SetNoiseType(FastNoise::SimplexFractal);
	noise.SetFractalOctaves(5);
	noise.SetFractalGain(0.7f);
	noise.SetFrequency(0.03f);
	noise.SetFractalLacunarity(3.0f);
	noise.SetSeed(seed);
	std::vector<unsigned char> pixels;
	for(unsigned int y=0; y<128; y++) {
		for(unsigned int x=0; x<128; x++) {
			//unsigned char noise_val = 255*((0.75*(noise.GetNoise(x, y)+1)/2)+0.25);
			unsigned char noise_val = 255*((0.4*(noise.GetNoise(x, y)+1)/2)+0.6);
			pixels.push_back(noise_val);
			pixels.push_back(noise_val);
			pixels.push_back(noise_val);
			pixels.push_back(255);
		}
	}
	return pixels;
}

glm::vec3 calculate_normal(Vertex A, Vertex B, Vertex C) {
	glm::vec3 surface_normal;
	GLfloat BA_x = B.x - A.x;
	GLfloat BA_y = B.y - A.y;
	GLfloat BA_z = B.z - A.z;
	
	GLfloat CA_x = C.x - A.x;
	GLfloat CA_y = C.y - A.y;
	GLfloat CA_z = C.z - A.z;
	
	surface_normal = glm::vec3((BA_y*CA_z - BA_z*CA_y), (BA_z*CA_x - BA_x*CA_z), (BA_x*CA_y - BA_y*CA_x));
	return surface_normal;
}

//unitsize is how many coordinates each height/width unit refers to
HeightMap generatePerlinNoiseMap(unsigned int width, unsigned int height, GLfloat unitsize, unsigned int seed) {
	
	std::vector<Vertex> vertices;
	std::vector<GLfloat> buffer;
	GLfloat scalingFactor = 40.0f*unitsize;
	GLuint quadNumber = 0;
	FastNoise noise;
	noise.SetNoiseType(FastNoise::SimplexFractal);
	noise.SetFractalOctaves(5);
	noise.SetFractalGain(0.4f);
	noise.SetSeed(seed);
	std::vector<GLuint> indices;
	std::vector<Triangle> triangles;
	glm::vec3 vertex_normal = glm::vec3(0.0f, 0.0f, 0.0f);
	
	//vertices
	for(unsigned int x=0; x<=width; x++) {
		GLfloat x_coord = x*unitsize;
		GLfloat z_coord = 0.0f;
		GLfloat y_coord = scalingFactor*(noise.GetNoise(x_coord, z_coord)+1)/2;
		std::vector<unsigned int> adjacent_triangles;
		if(x==0) {
			adjacent_triangles.push_back(0);
			adjacent_triangles.push_back(1);
		}
		else if(x==width) {
			adjacent_triangles.push_back(2*width - 1);
		}
		else {
			adjacent_triangles.push_back(2*x - 1);
			adjacent_triangles.push_back(2*x);
			adjacent_triangles.push_back(2*x + 1);
		}
		Vertex vertex = {
			x_coord,
			y_coord,
			z_coord,
			adjacent_triangles,
			vertex_normal,
		};
		vertices.push_back(vertex);	
	}
	
	for(unsigned int z=1; z<height; z++) {
		for(unsigned int x=0; x<=width; x++) {
			GLfloat x_coord = x*unitsize;
			GLfloat z_coord = - (GLfloat)z*unitsize;
			GLfloat y_coord = scalingFactor*(noise.GetNoise(x_coord, z_coord)+1)/2;
			std::vector<unsigned int> adjacent_triangles;
			if(x==0) {
				adjacent_triangles.push_back(2*width*(z-1));
				adjacent_triangles.push_back(2*width*z);
				adjacent_triangles.push_back(2*width*z + 1);
			}
			else if(x==width) {
				adjacent_triangles.push_back(2*(width*(z-1) + (width-1)));
				adjacent_triangles.push_back(2*(width*(z-1) + (width-1)) + 1);
				adjacent_triangles.push_back(2*(width*z + width) - 1);
			}
			else {
				adjacent_triangles.push_back(2*(width*(z-1) + (x-1)));
				adjacent_triangles.push_back(2*(width*(z-1) + (x-1)) + 1);
				adjacent_triangles.push_back(2*(width*(z-1) + (x-1)) + 2);
				adjacent_triangles.push_back(2*(width*z + (x-1)) + 1);
				adjacent_triangles.push_back(2*(width*z + x));
				adjacent_triangles.push_back(2*(width*z + x) + 1);
			}
			Vertex vertex = {
				x_coord,
				y_coord,
				z_coord,
				adjacent_triangles,
				vertex_normal,
			};
			vertices.push_back(vertex);	
		}
	}
	
	for(unsigned int x=0; x<=width; x++) {
		GLfloat x_coord = x*unitsize;
		GLfloat z_coord = -(GLfloat)height*unitsize;
		GLfloat y_coord = scalingFactor*(noise.GetNoise(x_coord, z_coord)+1)/2;
		std::vector<unsigned int> adjacent_triangles;
		if(x==0) {
			adjacent_triangles.push_back(2*(width*(height-1)));
		}
		else if(x==width) {
			adjacent_triangles.push_back(2*(width*(height-1) + (width-1)));
			adjacent_triangles.push_back(2*(width*(height-1) + (width-1)) + 1);
		}
		else {
			adjacent_triangles.push_back(2*(width*(height-1) + (x-1)));
			adjacent_triangles.push_back(2*(width*(height-1) + (x-1)) + 1);
			adjacent_triangles.push_back(2*(width*(height-1) + (x-1)) + 2);
		}
		Vertex vertex = {
			x_coord,
			y_coord,
			z_coord,
			adjacent_triangles,
			vertex_normal,
		};
		vertices.push_back(vertex);	
	}
	
	//triangles
	for(unsigned int z=0; z<height; z++) {	
		for(unsigned int x=0; x<width; x++) {
			Vertex botleft = vertices.at((width+1)*z + x);
			Vertex botright = vertices.at((width+1)*z + (x+1));
			Vertex topright = vertices.at((width+1)*(z+1) + (x+1));
			Vertex topleft = vertices.at((width+1)*(z+1) + x);
			
			Triangle triangle1 = {
				topright,
				topleft,
				botleft,
				calculate_normal(topright, topleft, botleft),
			};
			
			Triangle triangle2 = {
				botleft,
				botright,
				topright,
				calculate_normal(botleft, botright, topright),
			};
			
			triangles.push_back(triangle1);
			triangles.push_back(triangle2);
		}
	}
	
	
	for(unsigned int z=0; z<height; z++) {
		for(unsigned int x=0; x<width; x++) {
			GLuint noiseNum = std::rand() % 3;
			//odd is flip, even is not flip
			unsigned int u_flip = std::rand();
			unsigned int v_flip = std::rand();
			
			Triangle triangle1 = triangles.at(2*(width*z + x));
			Triangle triangle2 = triangles.at(2*(width*z + x) + 1);
			
			//topright
			Vertex topright = triangle2.C;
			//std::cout << "(" << topright.x << ", " << topright.z << ")";
			//xyz
			buffer.push_back(topright.x);
			buffer.push_back(topright.y);
			buffer.push_back(topright.z);
			/*
			test.push_back(topright.x);
			test.push_back(topright.y);
			test.push_back(topright.z);
			*/
			//uv
			buffer.push_back((1 + u_flip)%2);
			buffer.push_back((1 + v_flip)%2);
			//blendmap
			glm::vec3 blendMap = gradient(topright.y/scalingFactor);
			buffer.push_back(blendMap.x);
			buffer.push_back(blendMap.y);
			buffer.push_back(blendMap.z);
			//normal
			if(std::pow(topright.vertex_normal.x, 2) + std::pow(topright.vertex_normal.y, 2) + std::pow(topright.vertex_normal.z, 2) == 0.0f) {
				for(unsigned int i=0; i<topright.adjacent_triangles.size(); i++) {
					topright.vertex_normal.x += triangles.at(topright.adjacent_triangles.at(i)).surface_normal.x;
					topright.vertex_normal.y += triangles.at(topright.adjacent_triangles.at(i)).surface_normal.y;
					topright.vertex_normal.z += triangles.at(topright.adjacent_triangles.at(i)).surface_normal.z;
				}
			}
			buffer.push_back(topright.vertex_normal.x);
			buffer.push_back(topright.vertex_normal.y);
			buffer.push_back(topright.vertex_normal.z);
			/*
			test.push_back(topright.vertex_normal.at(0) + topright.x);
			test.push_back(topright.vertex_normal.at(1) + topright.y);
			test.push_back(topright.vertex_normal.at(2) + topright.z);
			*/
			//std::cout << "(" << topright.vertex_normal.at(0) << ", " << topright.vertex_normal.at(1)  << ", " << topright.vertex_normal.at(2) << ")		";
			//which noise
			buffer.push_back(noiseNum);
			
			//topleft
			Vertex topleft = triangle1.B;
			//std::cout << "(" << topleft.x << ", " << topleft.z << ")";
			//xyz
			buffer.push_back(topleft.x);
			buffer.push_back(topleft.y);
			buffer.push_back(topleft.z);
			/*
			test.push_back(topleft.x);
			test.push_back(topleft.y);
			test.push_back(topleft.z);
			*/
			//uv
			buffer.push_back((0 + u_flip)%2);
			buffer.push_back((1 + v_flip)%2);
			//blendmap
			blendMap = gradient(topleft.y/scalingFactor);
			buffer.push_back(blendMap.x);
			buffer.push_back(blendMap.y);
			buffer.push_back(blendMap.z);
			//normal
			if(std::pow(topleft.vertex_normal.x, 2) + std::pow(topleft.vertex_normal.y, 2) + std::pow(topleft.vertex_normal.z, 2) == 0.0f) {
				for(unsigned int i=0; i<topleft.adjacent_triangles.size(); i++) {
					topleft.vertex_normal.x += triangles.at(topleft.adjacent_triangles.at(i)).surface_normal.x;
					topleft.vertex_normal.y += triangles.at(topleft.adjacent_triangles.at(i)).surface_normal.y;
					topleft.vertex_normal.z += triangles.at(topleft.adjacent_triangles.at(i)).surface_normal.z;
				}
			}
			buffer.push_back(topleft.vertex_normal.x);
			buffer.push_back(topleft.vertex_normal.y);
			buffer.push_back(topleft.vertex_normal.z);
			/*
			test.push_back(topleft.vertex_normal.at(0) + topleft.x);
			test.push_back(topleft.vertex_normal.at(1) + topleft.y);
			test.push_back(topleft.vertex_normal.at(2) + topleft.z);
			*/
			//std::cout << "(" << topleft.vertex_normal.at(0) << ", " << topleft.vertex_normal.at(1)  << ", " << topleft.vertex_normal.at(2) << ")		";
			//which noise
			buffer.push_back(noiseNum);
			
			//botleft
			Vertex botleft = triangle2.A;
			//std::cout << "(" << botleft.x << ", " << botleft.z << ")";
			//xyz
			buffer.push_back(botleft.x);
			buffer.push_back(botleft.y);
			buffer.push_back(botleft.z);
			/*
			test.push_back(botleft.x);
			test.push_back(botleft.y);
			test.push_back(botleft.z);
			*/
			//uv
			buffer.push_back((0 + u_flip)%2);
			buffer.push_back((0 + v_flip)%2);
			//blendmap
			blendMap = gradient(botleft.y/scalingFactor);
			buffer.push_back(blendMap.x);
			buffer.push_back(blendMap.y);
			buffer.push_back(blendMap.z);
			//normal
			if(std::pow(botleft.vertex_normal.x, 2) + std::pow(botleft.vertex_normal.y, 2) + std::pow(botleft.vertex_normal.z, 2) == 0.0f) {
				for(unsigned int i=0; i<botleft.adjacent_triangles.size(); i++) {
					botleft.vertex_normal.x += triangles.at(botleft.adjacent_triangles.at(i)).surface_normal.x;
					botleft.vertex_normal.y += triangles.at(botleft.adjacent_triangles.at(i)).surface_normal.y;
					botleft.vertex_normal.z += triangles.at(botleft.adjacent_triangles.at(i)).surface_normal.z;
				}
			}
			buffer.push_back(botleft.vertex_normal.x);
			buffer.push_back(botleft.vertex_normal.y);
			buffer.push_back(botleft.vertex_normal.z);
			/*
			test.push_back(botleft.vertex_normal.at(0) + botleft.x);
			test.push_back(botleft.vertex_normal.at(1) + botleft.y);
			test.push_back(botleft.vertex_normal.at(2) + botleft.z);
			*/
			//std::cout << "(" << botleft.vertex_normal.at(0) << ", " << botleft.vertex_normal.at(1)  << ", " << botleft.vertex_normal.at(2) << ")		";
			//which noise
			buffer.push_back(noiseNum);
				
			//botright
			Vertex botright = triangle2.B;
			//std::cout << "(" << botright.x << ", " << botright.z << ")		";
			//xyz
			buffer.push_back(botright.x);
			buffer.push_back(botright.y);
			buffer.push_back(botright.z);
			/*
			test.push_back(botright.x);
			test.push_back(botright.y);
			test.push_back(botright.z);
			*/
			//uv
			buffer.push_back((1 + u_flip)%2);
			buffer.push_back((0 + v_flip)%2);
			//blendmap
			blendMap = gradient(botright.y/scalingFactor);
			buffer.push_back(blendMap.x);
			buffer.push_back(blendMap.y);
			buffer.push_back(blendMap.z);
			//normal
			if(std::pow(botright.vertex_normal.x, 2) + std::pow(botright.vertex_normal.y, 2) + std::pow(botright.vertex_normal.z, 2) == 0.0f) {
				for(unsigned int i=0; i<botright.adjacent_triangles.size(); i++) {
					botright.vertex_normal.x += triangles.at(botright.adjacent_triangles.at(i)).surface_normal.x;
					botright.vertex_normal.y += triangles.at(botright.adjacent_triangles.at(i)).surface_normal.y;
					botright.vertex_normal.z += triangles.at(botright.adjacent_triangles.at(i)).surface_normal.z;
				}
			}
			buffer.push_back(botright.vertex_normal.x);
			buffer.push_back(botright.vertex_normal.y);
			buffer.push_back(botright.vertex_normal.z);
			/*
			test.push_back(botright.vertex_normal.at(0) + botright.x);
			test.push_back(botright.vertex_normal.at(1) + botright.y);
			test.push_back(botright.vertex_normal.at(2) + botright.z);
			*/
			//std::cout << "(" << botright.vertex_normal.at(0) << ", " << botright.vertex_normal.at(1)  << ", " << botright.vertex_normal.at(2) << ")		";
			//which noise
			buffer.push_back(noiseNum);
				
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
		buffer,
		indices,
	};
	/*
	for(unsigned int c = 0; c<vertices.size(); c++) {
		std::cout << vertices.at(c).x << ", " << vertices.at(c).y << ", " << vertices.at(c).z << "		";
	}
	*/
	
	
	return heightMap;
}
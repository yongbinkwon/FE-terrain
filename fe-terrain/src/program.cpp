// Local headers
#include "program.hpp"
#include "util/window.hpp"
#include "terrain.hpp"
#include "util/shader.hpp"
#include "util/imageLoader.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>


Shading::Shader* shader;
//PER GRID DIMENSIONS
unsigned int grid_width = 128;
unsigned int grid_height = 128;
GLfloat unitsize = 2.0f;
//CAMERA
glm::vec3 cameraPos   = glm::vec3((grid_width/2)*unitsize, 30.0f,  0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
float deltaTime = 0.0f;	
float lastFrame = 0.0f;
float lastX = windowWidth/2.0f, lastY = windowHeight/2.0f;
bool firstMouse = true;
float yaw   = -90.0f;
float pitch =  0.0f;

unsigned int imageToTexture(PNGImage im) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im.width, im.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, im.pixels.data());
	/*
	int i = im.pixels.at(3);
	std::cout << i << "AAAAAAAAAAA";
	*/
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return textureID;
}

unsigned int noiseToTexture(std::vector<unsigned char> pixels, unsigned int width, unsigned int height) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return textureID;
}

HeightMap generateMap(unsigned int width, unsigned int height, unsigned int seed) {
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> terraintypes = {0, 1, 1, 0, 1, 0, 0, 1, 1};
	std::vector<GLfloat> scalingfactors = {40.0f, 10.0f};
	assert(terraintypes.size()==width*height);
	unsigned int quadCount = 0;
	
	for (unsigned int z=0; z<height; z++) {
		for (unsigned int x=0; x<width; x++) {
			unsigned int terraintype_index = x+(z*width);
			HeightMap heightmap = generatePerlinNoiseMap(grid_width, grid_height, x*(grid_width+32), z*(grid_height+32), quadCount, unitsize, seed, terraintypes.at(terraintype_index));
			quadCount = heightmap.quadNumber;
			vertices.insert(vertices.end(), heightmap.vertices.begin(), heightmap.vertices.end());
			indices.insert(indices.end(), heightmap.indices.begin(), heightmap.indices.end());
			
			
			if (x<(width-1)) {
				unsigned int left = terraintypes.at(terraintype_index);
				unsigned int right = terraintypes.at(terraintype_index+1);
				HeightMap smoothener = horizontalTerrainSmoothener(32, grid_height, x*(grid_width+32) + grid_width, z*(grid_height+32), quadCount, unitsize, seed, scalingfactors.at(left), scalingfactors.at(right));
				quadCount = smoothener.quadNumber;
				vertices.insert(vertices.end(), smoothener.vertices.begin(), smoothener.vertices.end());
				indices.insert(indices.end(), smoothener.indices.begin(), smoothener.indices.end());
			}
			
			
			if (z<(height-1)) {
				unsigned int down = terraintypes.at(terraintype_index);
				unsigned int up = terraintypes.at(terraintype_index + width);
				HeightMap smoothener = verticalTerrainSmoothener(grid_width, 32, x*(grid_width+32), z*(grid_height+32) + grid_height, quadCount, unitsize, seed, scalingfactors.at(down), scalingfactors.at(up));
				quadCount = smoothener.quadNumber;
				vertices.insert(vertices.end(), smoothener.vertices.begin(), smoothener.vertices.end());
				indices.insert(indices.end(), smoothener.indices.begin(), smoothener.indices.end());
			}
			
		}
	}
	HeightMap returnMap = {
		vertices,
		indices,
		quadCount,
	};
	return returnMap;
}

void runProgram(GLFWwindow* window)
{
	
    // Enable depth (Z) buffer (accept "closest" fragment)
	
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
	
	//glDisable(GL_DEPTH_TEST);

    // Configure miscellaneous OpenGL settings
    glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);

    // Set default colour after clearing the colour buffer
    glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
	
	//seeding based on time
	std::srand(std::time(nullptr));

    // Set up your scene here (create Vertex Array Objects, etc.)
	// Activate shader
	shader = new Shading::Shader();
    shader->makeBasicShader("../fe-terrain/shaders/simple.vert", "../fe-terrain/shaders/simple.frag");
    shader->activate();
	
	GLuint vaoID;
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
	
	HeightMap heightMap = generateMap(3, 3, std::rand());

	
	/*
	std::vector<GLfloat> vertices = {-0.6f, -0.6f, 0.0f,
									0.6f, -0.6f, 0.0f,
									0.0f, 0.6f, 0.0f};
	std::vector<GLuint> indices = {0, 1, 2};
	*/
	//xyz uv rgb x'y'z' i
	unsigned int vboID;
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*heightMap.vertices.size(), heightMap.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GL_FLOAT)+sizeof(GL_UNSIGNED_INT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 11*sizeof(GL_FLOAT)+sizeof(GL_UNSIGNED_INT), (GLvoid*)(3*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11*sizeof(GL_FLOAT)+sizeof(GL_UNSIGNED_INT), (GLvoid*)(5*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, 11*sizeof(GL_FLOAT)+sizeof(GL_UNSIGNED_INT), (GLvoid*)(8*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 1, GL_UNSIGNED_INT, GL_FALSE, 11*sizeof(GL_FLOAT)+sizeof(GL_UNSIGNED_INT), (GLvoid*)(11*sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(4);
	
	unsigned int indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, heightMap.indices.size()*sizeof(GLuint), heightMap.indices.data(), GL_STATIC_DRAW);
	
	
	//textures
	/*
	PNGImage plaintex = loadPNGFile("../fe-terrain/textures/plaintex.png");
	unsigned int plaintexID = imageToTexture(plaintex);
	
	
	PNGImage mountaintex = loadPNGFile("../fe-terrain/textures/mountaintex3.png");
	unsigned int mountaintexID = imageToTexture(mountaintex);
	
	
	PNGImage snowtex = loadPNGFile("../fe-terrain/textures/snow01.png");
	unsigned int snowtexID = imageToTexture(snowtex);
	*/
	
	//noisetex
	unsigned int noisetexID1 = noiseToTexture(noiseMap(std::rand()), 128, 128);
	unsigned int noisetexID2 = noiseToTexture(noiseMap(std::rand()), 128, 128);
	unsigned int noisetexID3 = noiseToTexture(noiseMap(std::rand()), 128, 128);
	
	//lighting
	/*
	shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f); 	
	shader.setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
	shader.setVec3("light.diffuse",  0.5f, 0.5f, 0.5f);
	shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); 
	*/
	
	GLint light_dir_loc = shader->getUniformFromName("light.direction");
	glUniform3fv(light_dir_loc, 1, glm::value_ptr(glm::vec3(-0.5f, -1.0f, 0.3f)));
	
	GLint light_ambient_loc = shader->getUniformFromName("light.ambient");
	glUniform3fv(light_ambient_loc, 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
	
	GLint light_diffuse_loc = shader->getUniformFromName("light.diffuse");
	glUniform3fv(light_diffuse_loc, 1, glm::value_ptr(glm::vec3(0.6f, 0.6f, 0.6f)));
	
	GLint light_spec_loc = shader->getUniformFromName("light.specular");
	glUniform3fv(light_spec_loc, 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));

	
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
	glm::mat4 view;
	glm::mat4 projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 350.f);
    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
		//std::cout << std::rand() << "AAAAAAAAAAAAAAAAAAA, ";
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw your scene here
		//CAMERA
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;  
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projection*view*model));
		glUniform3fv(1, 1, glm::value_ptr(cameraPos));
		glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(model));
		
		/*
		glBindTextureUnit(0, plaintexID);
		glBindTextureUnit(1, mountaintexID);
		glBindTextureUnit(2, snowtexID);
		*/
		glBindTextureUnit(0, noisetexID1);
		glBindTextureUnit(1, noisetexID2);
		glBindTextureUnit(2, noisetexID3);
		
		glDrawElements(GL_TRIANGLES, heightMap.indices.size(), GL_UNSIGNED_INT, nullptr);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

        // Handle other events
        glfwPollEvents();
        handleKeyboardInput(window);

        // Flip buffers
        glfwSwapBuffers(window);
    }
}

void handleKeyboardInput(GLFWwindow* window)
{
	float cameraSpeed = 100.0f * deltaTime;
    // Use escape key for terminating the GLFW window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
        cameraPos += cameraSpeed * cameraFront;
	}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
        cameraPos -= cameraSpeed * cameraFront;
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}  

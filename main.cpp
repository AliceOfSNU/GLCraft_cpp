#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

// linear algebra
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "GLObjects.h"
#include "camera.h"
#include "world.h"
#include "debug.h"

using namespace std;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void testRaycast(FacesSelection& selectedFaces);

constexpr int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 800;

bool isWindowed = true;
bool isKeyboardProcessed[1024] = { 0 };
Camera Camera::MainCamera = Camera(glm::vec3(4.0f, 1.0f, -4.0f));

//mouse
double mouseX, mouseY;
double lastX = SCREEN_WIDTH / 2.0f, lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// position
Chunk* currChunk = nullptr;
Chunk* chunks[3][3];

void generate_chunk(Chunk* chunk) {
	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			for (int j = 0; j < Chunk::HEIGHT / 2 + (i+k-8); ++j) {
				Block* block = chunk->grid[i][j][k] = new Block(BlockDB::BlockType::BLOCK_GRASS);
				block->pos.x = chunk->basepos.x + i;
				block->pos.z = chunk->basepos.z + k;
				block->pos.y = chunk->basepos.y + j;
				chunk->blockCnt++;
			}
		}
	}
}

int main() {
	glfwInit();

	//gl VERSION 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GLCraft", NULL, NULL);
	if (window == NULL) {
		cout << "window creation failed" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback); 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //capture mouse? set CURSOR_DISABLED/NORMAL

	gladLoadGL();
	glViewport(0, 0, 800, 800);
	
	FacesSelection selectedFaces;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			chunks[i][j] = new Chunk(glm::ivec3(Chunk::SZ*(i-1), -Chunk::HEIGHT / 2, Chunk::SZ * (j - 1)));
			generate_chunk(chunks[i][j]);
			chunks[i][j]->Build();
		}
	}

	//create gl texture
	TextureArray2D arr_tex = TextureArray2D("atlas.png", 64, 64, 3, GL_RGB);
	//Texture2D dirt_top_tex = Texture2D("dirt_top_x64.png", GL_TEXTURE0, GL_RGB);
	//Texture2D dirt_side_tex = Texture2D("dirt_side_x64.png", GL_TEXTURE0, GL_RGB);
	//Texture2D dirt_bottom_tex = Texture2D("dirt_bottom_x64.png", GL_TEXTURE0, GL_RGB);

	Shader shader = Shader("basic.vs", "basic.fs");
	Shader solidColorShader = Shader("solidcolor.vs", "solidcolor.fs");

	//get the uniform id for texture sampler
	shader.use();
	shader.setInt("tex0", 0);

	glEnable(GL_DEPTH_TEST);

	double applicationStartTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {


		//--------- GAME STATE
		// time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// position
		int found = false;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				if (chunks[i][j]->TestAABB(Camera::MainCamera.position)) {
					if (chunks[i][j] != currChunk) {
						currChunk = chunks[i][j];
						printf("currently at chunk [%d][%d]\n", i, j);
					}
					found = true; break;
				}
			}
		}
		if (!found) { 
			if(currChunk) printf("chunk outside range\n");
			currChunk = nullptr; 
		}


		//--------- INPUT

		processInput(window);

		//test area
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			isKeyboardProcessed[GLFW_KEY_SPACE] = true;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
			if (isKeyboardProcessed[GLFW_KEY_SPACE]) {
				testRaycast(selectedFaces);
				isKeyboardProcessed[GLFW_KEY_SPACE] = false;
			}
		}

		//--------- RENDER

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		//texture
		arr_tex.Bind();

		//model view projection
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		view = Camera::MainCamera.GetViewMatrix();
		proj = Camera::MainCamera.GetPerspectiveMatrix();
		shader.setMat4f("model", glm::value_ptr(model));
		shader.setMat4f("view", glm::value_ptr(view));
		shader.setMat4f("proj", glm::value_ptr(proj));

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				chunks[i][j]->Render();
			}
		}

		//debug render
		solidColorShader.use();
		solidColorShader.setMat4f("model", glm::value_ptr(model));
		solidColorShader.setMat4f("view", glm::value_ptr(view));
		solidColorShader.setMat4f("proj", glm::value_ptr(proj));
		glm::vec3 col(0.0f, 1.0f, 0.0f);
		solidColorShader.setVec3f("col", glm::value_ptr(col));
		Debug::Render();
		selectedFaces.Render();

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	
	//vao.Delete();
	//vbo_uv.Delete();
	//vbo_pos.Delete();
	//ebo.Delete();

	arr_tex.UnBind();
	//dirt_bottom_tex.Delete();
	//dirt_side_tex.Delete();
	//dirt_top_tex.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void testRaycast(FacesSelection& selectedFaces) {
	glm::vec3 dir = Camera::MainCamera.ScreenPointToRay(mouseX, mouseY);
	printf("%f \n", glm::length(dir));
	Ray ray(Camera::MainCamera.position, dir);
	Debug::DrawRay(ray);

	if (!currChunk) return;

	glm::ivec3 idx = currChunk->FindBlockIndex(Camera::MainCamera.position);
	//integer voxel indices
	int ix = idx.x, iy = idx.y, iz = idx.z;

	//'step'
	int xDir = (ray.dir.x > 0) ? 1 : (ray.dir.x == 0) ? 0 : -1;
	int yDir = (ray.dir.y > 0) ? 1 : (ray.dir.y == 0) ? 0 : -1;
	int zDir = (ray.dir.z > 0) ? 1 : (ray.dir.z == 0) ? 0 : -1;

	//'bound'
	float xBound = ((ray.dir.x >= 0) ? ix + 1 : ix);
	float yBound = ((ray.dir.y >= 0) ? iy + 1 : iy);
	float zBound = ((ray.dir.z >= 0) ? iz + 1 : iz);

	//'current t'
	glm::vec3 initPos = Camera::MainCamera.position; 
	initPos -= (currChunk->basepos); 
	initPos += 0.5f;

	float xt = (xBound - initPos.x) * ray.invDir.x;
	float yt = (yBound - initPos.y) * ray.invDir.y;
	float zt = (zBound - initPos.z) * ray.invDir.z;

	//'delta t'
	float xDelta = xDir * ray.invDir.x;
	float yDelta = yDir * ray.invDir.y;
	float zDelta = zDir * ray.invDir.z;

	int xface = ((ray.dir.x >= 0) ? Block::Face::LEFT : Block::Face::RIGHT);
	int yface = ((ray.dir.y >= 0) ? Block::Face::BOTTOM : Block::Face::TOP);
	int zface = ((ray.dir.z >= 0) ? Block::Face::BACK : Block::Face::FRONT);
	int face = 0, ii = 0;
	for (int cnt = 0; cnt < 10; ++cnt) {
		if (xt < yt && xt < zt) {
			ii = ix + xDir;
			if (ii < 0 || ii >= Chunk::SZ) break;
			face = xface, ix = ii, xt += xDelta;
		}
		else if (yt < zt) {
			ii = iy + yDir;
			if (ii < 0 || ii >= Chunk::HEIGHT) break;
			face = yface, iy = ii, yt += yDelta;
		}
		else {
			ii = iz + zDir;
			if (ii < 0 || ii >= Chunk::SZ) break;
			face = zface, iz = ii, zt += zDelta;
		}

		if (currChunk->grid[ix][iy][iz]) {
			selectedFaces.AddFace(currChunk->grid[ix][iy][iz], face);
		}
	}
	
	selectedFaces.Build();
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// TODO : make camera movable (WASD) & increase or decrease dayFactor(press O: increase, press P: decrease)
	const float cameraSpeed = 0.05f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Camera::MainCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Camera::MainCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Camera::MainCamera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Camera::MainCamera.ProcessKeyboard(RIGHT, deltaTime);


}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouseX = xpos;
	mouseY = ypos;

	// calculate how much cursor have moved, rotate camera proportional to the value, using ProcessMouseMovement.
	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	Camera::MainCamera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera::MainCamera.ProcessMouseScroll(yoffset);
}
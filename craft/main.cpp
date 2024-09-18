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
#include "gui.h"
using namespace std;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void testRaycast(FacesSelection& selectedFaces);

constexpr int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 800;

bool isWindowed = true;
bool isKeyboardProcessed[1024] = { 0 };
Camera Camera::MainCamera = Camera(glm::vec3(4.0f, 1.0f, -4.0f));

//mouse
bool mouseHeld = false;
double mouseX, mouseY;
double lastX = SCREEN_WIDTH / 2.0f, lastY = SCREEN_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// active selection
glm::ivec3 selectedBlockIdx;
int selectedFace = -1;
bool selectedBlockExists;

struct Timer {
public:
	double startTime;
	double setTime;
	bool running;
	double Start() {
		startTime = glfwGetTime();
		running = true;
		return startTime;
	}
	void Stop() {
		running = false;
	}
	double GetTime() {
		return running? (glfwGetTime() - startTime) : -1.0;
	}

};

struct BlockDestructionTimer:public Timer  {
public:
	static constexpr double DURATION = 2;
	glm::ivec3 blockIdx;

} blockDestructionTimer;


World world(Camera::MainCamera.position);


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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //capture mouse? set CURSOR_DISABLED/NORMAL

	gladLoadGL();
	glViewport(0, 0, 800, 800);
	

	FacesSelection selectedFaces;
	world.CreateInitialChunks(Camera::MainCamera.position);
	CircleFill circleUI(70.f);
	//create gl texture
	TextureArray2D arr_tex = TextureArray2D("atlas.png", 64, 64, 7, GL_RGB);
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

		//--------- INPUT
		processInput(window);
		testRaycast(selectedFaces);

		//block destruction
		if (mouseHeld) {
			if (!selectedBlockExists){ 
				if(blockDestructionTimer.running) blockDestructionTimer.Stop();
			}
			else if (selectedBlockIdx == blockDestructionTimer.blockIdx && blockDestructionTimer.GetTime() > BlockDestructionTimer::DURATION) {
				//DestroyBlock(blockDestructionTimer.blockIdx);
				cout << "timer goes off" << endl;
				Chunk* ch = world.GetChunkContainingBlock(selectedBlockIdx);
				if (ch != nullptr) {
					glm::ivec3 bidx = ch->BlockWorldToGridIdx(selectedBlockIdx);
					ch->DestroyBlockAt(bidx);
				}
				blockDestructionTimer.Start();
			}
			else if (!blockDestructionTimer.running || selectedBlockIdx != blockDestructionTimer.blockIdx) {
				//if selected block exists but timer isn't runinng, it should start running.
				//also, if the selected block changes while mouse is still pressed, timer should restart.
				Chunk* ch = world.GetChunkContainingBlock(selectedBlockIdx);

				if (ch) {
					glm::ivec3 bidx = ch->BlockWorldToGridIdx(selectedBlockIdx);
					if (ch->grid[bidx.x][bidx.y][bidx.z]) {
						blockDestructionTimer.blockIdx = selectedBlockIdx;
						blockDestructionTimer.Start();
					}
				}

			}
		}

		//--------- RENDER

		glClearColor(0.37f, 0.23f, 0.67f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
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


		// world update
		world.UpdateChunks(Camera::MainCamera.position);
		world.Build();
		world.Render();

		//-------- UI
		if (mouseHeld && blockDestructionTimer.running) {
			circleUI.Render(blockDestructionTimer.GetTime() / BlockDestructionTimer::DURATION, mouseX, mouseY);
		}
		//-------- DEBUG
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
	Ray ray(Camera::MainCamera.position, dir);
	//Debug::DrawRay(ray);
	
	glm::ivec3 currChunkIdx = Chunk::WorldToChunkIndex(Camera::MainCamera.position);
	Chunk* currChunk = world.GetChunkByIndex(currChunkIdx);
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
	int face = -1, ii = 0, found = 0;

	selectedBlockExists = false;

	for (int cnt = 0; cnt < 20; ++cnt) {
		if (xt < yt && xt < zt) {
			ii = ix + xDir;
			if (ii < 0 || ii >= Chunk::SZ) {
				currChunkIdx.x += xDir;
				currChunk = world.GetChunkByIndex(currChunkIdx);
				if (!currChunk) break;
				ii = ix = (ii < 0 ? Chunk::SZ - 1 : 0);
			}
			face = xface, ix = ii, xt += xDelta;
		}
		else if (yt < zt) {
			ii = iy + yDir;
			if (ii < 0 || ii >= Chunk::HEIGHT) {
				currChunkIdx.y += yDir;
				currChunk = world.GetChunkByIndex(currChunkIdx);
				if (!currChunk) break;
				ii = iy = (ii < 0 ? Chunk::HEIGHT - 1 : 0);
			}
			face = yface, iy = ii, yt += yDelta;
		}
		else {
			ii = iz + zDir;
			if (ii < 0 || ii >= Chunk::SZ) {
				currChunkIdx.z += zDir;
				currChunk = world.GetChunkByIndex(currChunkIdx);
				if (!currChunk) break;
				ii = iz = (ii < 0 ? Chunk::SZ-1 : 0);
			}
			face = zface, iz = ii, zt += zDelta;
		}

		if (currChunk->grid[ix][iy][iz] != nullptr) {
			glm::ivec3 idx = currChunk->BlockGridToWorldIdx(glm::ivec3{ix, iy, iz});
			
			//if (selectedBlockIdx != idx) {
			//	glm::ivec3 bidx = currChunk->grid[ix][iy][iz]->pos;
			//}

			selectedBlockIdx = idx;
			selectedFace = face;
			selectedBlockExists = true;
			//selectedFaces.AddFace(currChunk->grid[ix][iy][iz], face);

			break;
		}
	}

	if (face == -1) {
		selectedFace = -1;
		selectedBlockExists = false;
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

	if (mouseHeld) {
		//camera movement
		float dx = xpos - lastX;
		float dy = ypos - lastY;
		Camera::MainCamera.ProcessMouseMovement(dx, dy);
	}
	lastX = xpos;
	lastY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	static double mouseLastX, mouseLastY;
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		cout << "clicked\n";
		if (!mouseHeld) { //pressed
			mouseHeld = true;
			glfwGetCursorPos(window, &mouseLastX, &mouseLastY);

			if (selectedBlockExists) {
				blockDestructionTimer.blockIdx = selectedBlockIdx;
				blockDestructionTimer.Start();
			}
		}
		else { //released
			mouseHeld = false;
			blockDestructionTimer.Stop();
		}
	}
	
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera::MainCamera.ProcessMouseScroll(yoffset);
}
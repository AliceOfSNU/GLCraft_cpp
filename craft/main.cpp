#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

// linear algebra
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLObjects.h"
#include "camera.h"
#include "world.h"
#include "debug.h"
#include "gui.h"
#include "world.h"
#include "collision.h"
#include "rendering.hpp"
#include "weather.h"
using namespace std;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void testRaycast(FacesSelection& selectedFaces);

constexpr int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 800;

bool isWindowed = true;
bool isKeyboardProcessed[1024] = { 0 };
Camera Camera::MainCamera = Camera(glm::vec3(0.0f, 4.0f, 0.0f));

//mouse
bool mouseHeld = false;
double mouseX, mouseY;
double lastX = SCREEN_WIDTH / 2.0f, lastY = SCREEN_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// position
glm::vec3 last_pos;

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
Timer dragTimer;

std::shared_ptr<Shader> solidUIShader;

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
	

	// initialize objects
	FacesSelection selectedFaces;
	World::GetInstance().CreateInitialChunks(Camera::MainCamera.position);
	CircleFill circleUI(70.f);
	WeatherParticleRenderObj rainRenderObj(60.0f, 100.0f, 3000);
	//create gl texture
	TextureArray2D arr_tex = TextureArray2D("resources/atlas.png", 64, 64, 16, GL_RGBA);
	//Texture2D dirt_top_tex = Texture2D("dirt_top_x64.png", GL_TEXTURE0, GL_RGB);
	//Texture2D dirt_side_tex = Texture2D("dirt_side_x64.png", GL_TEXTURE0, GL_RGB);
	//Texture2D dirt_bottom_tex = Texture2D("dirt_bottom_x64.png", GL_TEXTURE0, GL_RGB);

	Shader shader = Shader("resources/basic.vs", "resources/basic.fs");
	Shader cutoutShader = Shader("resources/basic.vs", "resources/cutout.fs");
	Shader weatherShader = Shader("resources/billboard.vs", "resources/cutout_basic.fs");
	Shader solidColorShader = Shader("resources/solidcolor.vs", "resources/solidcolor.fs");
	Shader waterShader = Shader("resources/wave.vs", "resources/wave.fs");
	solidUIShader = std::make_shared<Shader>("resources/solidGUI.vs", "resources/solidGUI.fs");
	
	//get the uniform id for texture sampler
	shader.use();
	shader.setInt("tex0", 0);

	glEnable(GL_DEPTH_TEST);

	double applicationStartTime = glfwGetTime();
	size_t frameCnt = 0;
	last_pos = Camera::MainCamera.position;
	while (!glfwWindowShouldClose(window)) {


		//--------- GAME STATE
		// time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		frameCnt = (frameCnt + 1) % 1000000;

		//--------- INPUT
		processInput(window);
		testRaycast(selectedFaces);

		//--------- PHYSICS
		// physics must come after inputs, because inputs directly modify camera's position.
		glm::vec3 curr_pos = Camera::MainCamera.position;
		glm::vec3 begin_pos = last_pos - glm::vec3{ 0.5f, 1.5f, 0.5f };
		glm::vec3 end_pos = curr_pos - glm::vec3{ 0.5f, 1.5f, 0.5f };

		glm::vec3 updated_pos = updatePositionWithCollisionCheck(begin_pos, end_pos, { 1.0f, 2.0f, 1.0f });
		Camera::MainCamera.position = updated_pos + glm::vec3{0.5f, 1.5f, 0.5f};
		last_pos = Camera::MainCamera.position;

		//block destruction
		if (mouseHeld) {
			
			if (!selectedBlockExists) {
				if (blockDestructionTimer.running) blockDestructionTimer.Stop();
			}
			else if (selectedBlockIdx == blockDestructionTimer.blockIdx && blockDestructionTimer.GetTime() > BlockDestructionTimer::DURATION) {
				//DestroyBlock(blockDestructionTimer.blockIdx);
				Chunk* ch = World::GetInstance().GetChunkContainingBlock(selectedBlockIdx);
				if (ch != nullptr) {
					glm::ivec3 bidx = ch->BlockWorldToGridIdx(selectedBlockIdx);
					ch->DestroyBlockAt(bidx);
				}
				blockDestructionTimer.Start();
			}
			else if ((!blockDestructionTimer.running && dragTimer.GetTime() > 0.5f) ||
			(blockDestructionTimer.running && selectedBlockIdx != blockDestructionTimer.blockIdx)){
				Chunk* ch = World::GetInstance().GetChunkContainingBlock(selectedBlockIdx);
				if (ch) {
					glm::ivec3 bidx = ch->BlockWorldToGridIdx(selectedBlockIdx);
					if (ch->grid[bidx.x][bidx.y][bidx.z]) {
						blockDestructionTimer.blockIdx = selectedBlockIdx;
						blockDestructionTimer.Start();
					}
				}
			}
		}
		if (GUIManager::GetInstance().mouseEvent == 1) {
			std::cout << selectedBlockIdx.x << "," << selectedBlockIdx.y << "," << selectedBlockIdx.z << '\n';
			std::cout << selectedFace << std::endl;
			dragTimer.Start();
		} else if(GUIManager::GetInstance().mouseEvent == 2){
			dragTimer.Stop();
			glm::vec2 drag = GUIManager::GetInstance().mouseDrag;
			if(!blockDestructionTimer.running && selectedBlockExists && (drag.x*drag.x + drag.y*drag.y) < 16.0f){
				Chunk* ch = World::GetInstance().GetChunkContainingBlock(selectedBlockIdx);
				if (ch != nullptr) {
					glm::ivec3 bidx = ch->BlockWorldToGridIdx(selectedBlockIdx);
					int di[] {0, 1, 0, -1, 0, 0}, dj[] {0, 0, 0, 0, 1, -1}, dk[]{1, 0, -1, 0, 0, 0};
					if(selectedFace != -1){
						bidx += glm::ivec3{di[selectedFace], dj[selectedFace], dk[selectedFace]};
						ch->PlaceBlockAtCompileTime(bidx, BlockDB::BlockType::BLOCK_WOODEN_STAIR_P0);
					}
				}
			}
			blockDestructionTimer.Stop();
		}

		//--------- RENDER

		glClearColor(0.50f, 0.53f, 0.97f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		arr_tex.Bind();

		//model view projection
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		//model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		view = Camera::MainCamera.GetViewMatrix();
		proj = Camera::MainCamera.GetPerspectiveMatrix();

		// world update
		World::GetInstance().UpdateChunks(Camera::MainCamera.position);
		World::GetInstance().Build();
		//world.Render();

		//-------- Render
		shader.use();
		shader.setMat4f("model", glm::value_ptr(model));
		shader.setMat4f("view", glm::value_ptr(view));
		shader.setMat4f("proj", glm::value_ptr(proj));

		// 1. Opaque pass
		for (auto& [cidx, chunk] : World::GetInstance().visChunks) {
			if (!chunk->solidRenderObj.isBuilt || !chunk->solidRenderObj.isRender) continue;
			chunk->solidRenderObj.vao.Bind();
			glDrawElements(GL_TRIANGLES, chunk->solidRenderObj.idxcnt, GL_UNSIGNED_INT, 0);
		}

		// 2. Water pass
		waterShader.use();
		waterShader.setMat4f("model", glm::value_ptr(model));
		waterShader.setMat4f("view", glm::value_ptr(view));
		waterShader.setMat4f("proj", glm::value_ptr(proj));
		Chunk::ivec3 curridx = Chunk::WorldToChunkIndex(Camera::MainCamera.position);
		for (auto& [cidx, chunk] : World::GetInstance().visChunks) {
			if (!chunk->waterRenderObj.isBuilt || !chunk->waterRenderObj.isRender) continue;
			chunk->waterRenderObj.vao.Bind();
			// water blocks far away from player need not be so detailed
			// we don't draw them with water shader.
			auto [ci, cj, ck] = cidx;
			if (ci - curridx.x > 1 || ci - curridx.x < -1 || cj - curridx.y > 1 || cj - curridx.y < -1 || ck - curridx.z > 1 || ck - curridx.z < -1) {
				shader.use();
			}
			else {
				waterShader.use();
				waterShader.setFloat("_Time", currentFrame);
			}
			glDrawElements(GL_TRIANGLES, chunk->waterRenderObj.idxcnt, GL_UNSIGNED_INT, 0);
		}

		// 3. Cutout pass
		cutoutShader.use();
		cutoutShader.setMat4f("model", glm::value_ptr(model));
		cutoutShader.setMat4f("view", glm::value_ptr(view));
		cutoutShader.setMat4f("proj", glm::value_ptr(proj));
		for (auto& [cidx, chunk] : World::GetInstance().visChunks) {
			if (!chunk->cutoutRenderObj.isBuilt || !chunk->cutoutRenderObj.isRender) continue;
			chunk->cutoutRenderObj.vao.Bind();
			glDrawElements(GL_TRIANGLES, chunk->cutoutRenderObj.idxcnt, GL_UNSIGNED_INT, 0);
		}
		
		//-------- Weather particles
		weatherShader.use();
		weatherShader.setMat4f("modelview", glm::value_ptr(view));
		weatherShader.setFloat("_Time", currentFrame);
		weatherShader.setMat4f("proj", glm::value_ptr(proj));
		rainRenderObj.Render();

		//-------- UI
		if (mouseHeld && blockDestructionTimer.running) {
			circleUI.Render(blockDestructionTimer.GetTime() / BlockDestructionTimer::DURATION, mouseX, mouseY);
		}
		
		for (auto& [idx, window] : GUIManager::GetInstance().windows) {
			window->Update();
			window->Render();
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
		GUIManager::GetInstance().mouseEvent = 0;
		glfwPollEvents();

	}
	

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
	Chunk* currChunk = World::GetInstance().GetChunkByIndex(currChunkIdx);
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
				currChunk = World::GetInstance().GetChunkByIndex(currChunkIdx);
				if (!currChunk) break;
				ii = ix = (ii < 0 ? Chunk::SZ - 1 : 0);
			}
			face = xface, ix = ii, xt += xDelta;
		}
		else if (yt < zt) {
			ii = iy + yDir;
			if (ii < 0 || ii >= Chunk::HEIGHT) {
				currChunkIdx.y += yDir;
				currChunk = World::GetInstance().GetChunkByIndex(currChunkIdx);
				if (!currChunk) break;
				ii = iy = (ii < 0 ? Chunk::HEIGHT - 1 : 0);
			}
			face = yface, iy = ii, yt += yDelta;
		}
		else {
			ii = iz + zDir;
			if (ii < 0 || ii >= Chunk::SZ) {
				currChunkIdx.z += zDir;
				currChunk = World::GetInstance().GetChunkByIndex(currChunkIdx);
				if (!currChunk) break;
				ii = iz = (ii < 0 ? Chunk::SZ-1 : 0);
			}
			face = zface, iz = ii, zt += zDelta;
		}

		if (currChunk->grid[ix][iy][iz] != BlockDB::BlockType::BLOCK_AIR) {
			glm::ivec3 idx = currChunk->BlockGridToWorldIdx(glm::ivec3{ix, iy, iz});
			
			//if (selectedBlockIdx != idx) {
			//	glm::ivec3 bidx = currChunk->grid[ix][iy][iz]->pos;
			//}

			selectedBlockIdx = idx;
			selectedFace = face;
			selectedBlockExists = true;
			// selectedFaces.AddFace(currChunk->grid[ix][iy][iz], face);

			break;
		}
	}

	if (face == -1) {
		selectedFace = -1;
		selectedBlockExists = false;
	}
	
	selectedFaces.Build();
}

glm::vec3 updatePositionWithCollisionCheck(glm::vec3 begin_pos, glm::vec3 end_pos, glm::vec3 box_dims) {
	//using namespace Collision;
	Collision::CollisionCheck checker(begin_pos, end_pos, box_dims);
	Collision::AABB swAABB = checker.ComputeBroadphaseAABB(); //get swept AABB

	std::vector<Collision::AABB> colliders;
	int endx = (int)(swAABB.start.x + swAABB.scale.x + 0.5f);
	int endy = (int)(swAABB.start.y + swAABB.scale.y + 0.5f);
	int endz = (int)(swAABB.start.z + swAABB.scale.z + 0.5f);
	for (int x = (int)(swAABB.start.x-0.5f); x <= endx; ++x) {
		for (int y = (int)(swAABB.start.y-0.5f); y <= endy; ++y) {
			for (int z = (int)(swAABB.start.z-0.5f); z <= endz; ++z) {
				Chunk* chunk = World::GetInstance().CurrentChunk({ x, y, z });
				Chunk::ivec3 blockidx = chunk->FindBlockIndex({ x, y, z });
				if (chunk->grid[blockidx.x][blockidx.y][blockidx.z] != BlockDB::BlockType::BLOCK_AIR) {
					colliders.push_back({ {x-0.5f, y-0.5f, z-0.5f}, {1.0f, 1.0f, 1.0f}, chunk->grid[blockidx.x][blockidx.y][blockidx.z] });
				}
			}
		}
	}
	Collision::Collision col = checker.GetFirstHit(colliders);
	checker = Collision::CollisionCheck(col.stop_pos, col.stop_pos + col.remain_vel, box_dims);
	Collision::Collision col2 = checker.GetFirstHit(colliders);
	checker = Collision::CollisionCheck(col2.stop_pos, col2.stop_pos + col2.remain_vel, box_dims);
	Collision::Collision col3 = checker.GetFirstHit(colliders);

	return col3.stop_pos + col3.remain_vel;
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

	//DEBUG MODE INPUT
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		cout << Camera::MainCamera.position.r << ", " << Camera::MainCamera.position.g << ", " << Camera::MainCamera.position.b << "\n";
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) isKeyboardProcessed[GLFW_KEY_X] = true;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE && isKeyboardProcessed[GLFW_KEY_X]) {
		isKeyboardProcessed[GLFW_KEY_X] = false;
		if (GUIManager::GetInstance().windows.count("inventory")) {
			auto inven = GUIManager::GetInstance().windows["inventory"];
			inven->Destroy();
			GUIManager::GetInstance().windows.erase("inventory");
		}
		else {
			std::shared_ptr<Inventory> main_panel = make_shared<Inventory>();
			main_panel->renderObj = make_unique<SolidGUIRenderObject>();
			main_panel->renderObj->shader = solidUIShader;
			main_panel->centerX = 400.0f;
			main_panel->centerY = 400.0f;
			main_panel->width = 400.f;
			main_panel->height = 200.f;
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 5; ++j) {
					std::shared_ptr<Button> item = make_shared<Button>();
					item->id = "inventory_" + std::to_string(5 * i + j);
					item->renderObj = make_unique<SolidGUIRenderObject>(glm::vec3(0.1f, 0.1f, 0.1f));
					item->renderObj->shader = solidUIShader;
					item->centerX = 400.f - (60.f * 2) + j * 60.f;
					item->width = 50.f;
					item->centerY = 400.f - (60.f * 1) + i * 60.f;
					item->height = 50.f;
					auto onClickListener = [](Button& btn) {
						auto solidRend = dynamic_cast<SolidGUIRenderObject*>(btn.renderObj.get());
						auto start = btn.id.find("inventory_", 0) + 10;
						auto idnum = stoi(btn.id.substr(start, btn.id.size()-start));
						auto inven = dynamic_cast<Inventory*>(GUIManager::GetInstance().windows["inventory"].get());
						inven->Select(idnum);
					};
					auto onEnterListener = [](Button& btn) {
						auto solidRend = dynamic_cast<SolidGUIRenderObject*>(btn.renderObj.get());
						solidRend->fillcolor = { 0.2f, 0.2f, 0.2f };
					};
					auto onExitListener = [](Button& btn) {
						auto solidRend = dynamic_cast<SolidGUIRenderObject*>(btn.renderObj.get());
						auto start = btn.id.find("inventory_", 0) + 10;
						auto idnum = stoi(btn.id.substr(start, btn.id.size() - start));
						auto inven = dynamic_cast<Inventory*>(GUIManager::GetInstance().windows["inventory"].get());
						if(idnum != inven->selected) solidRend->fillcolor = { 0.1f, 0.1f, 0.1f };
					};
					item->OnClick = onClickListener;
					item->OnMouseEnter = onEnterListener;
					item->OnMouseExit = onExitListener;
					main_panel->children.push_back(std::move(item));
				}
			}
			main_panel->Build();
			GUIManager::GetInstance().windows["inventory"] = main_panel;
		}
	}
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
	GUIManager::GetInstance().mouseXY = { mouseX, GUI::SCREEN_HEIGHT-mouseY };
	GUIManager::GetInstance().mouseDelta = {mouseX - lastX, mouseY - lastY};
	lastX = xpos;
	lastY = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	static double mouseLastX, mouseLastY;
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (!mouseHeld) { //pressed
			mouseHeld = true;
			glfwGetCursorPos(window, &mouseLastX, &mouseLastY);

			if (selectedBlockExists) {
				blockDestructionTimer.blockIdx = selectedBlockIdx;
				// blockDestructionTimer.Start();
			}
			GUIManager::GetInstance().mouseEvent = 1;
		}
		else { //released
			GUIManager::GetInstance().mouseEvent = 2;
			double mouseRelX, mouseRelY;
			glfwGetCursorPos(window, &mouseRelX, &mouseRelY);
			GUIManager::GetInstance().mouseDrag = {mouseRelX - mouseLastX, mouseRelY - mouseLastY};
			mouseHeld = false;
		}
	}
	else { //held
		GUIManager::GetInstance().mouseEvent = 0;
	}
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera::MainCamera.ProcessMouseScroll(yoffset);
}
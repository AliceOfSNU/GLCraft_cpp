#pragma once
#ifndef GUI_H
#define GUI_H
#include "rendering.hpp"
#include "GLObjects.h"
#include <iostream>
#include <map>
#include <string>
#include <functional>

class CircleFill {
public:
	glm::vec3 fillcolor{ 0.6f, 1.0f, 0.6f };
	glm::vec3 backcolor{ 0.5f, 0.5f, 0.5f};
	glm::vec2 center{ -0.5f, 0.0f };
	glm::vec2 resolution{ 800, 800 };
	glm::vec2 radius;
	float size;

	CircleFill(float r); 
	void Render(float percentage, float sx, float sy);

private:
	Shader shader = Shader("resources/circleui.vert", "resources/circleui.frag");
	VAO vao;
	VBO vbo;
	std::vector<float> vertices = std::vector<float>{
		//0: xyz		
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};
};

class GUI {
	using GUIRef = std::shared_ptr<GUI>;
public:
	const static int SCREEN_WIDTH = 800;
	const static int SCREEN_HEIGHT = 800;
	virtual void Render();
	virtual void Destroy();
	virtual void Build();
	virtual void Update();
	std::vector<std::shared_ptr<GUI>> children;

};

class GUIManager {
public:
	static GUIManager& GetInstance() {
		static GUIManager instance;
		return instance;
	}
	std::map<std::string, std::shared_ptr<GUI>> windows;
	std::shared_ptr<Shader> solidUIShader;
	std::shared_ptr<Shader> atlasUIShader;

	glm::vec2 mouseXY;
	glm::vec2 mouseDelta;
	glm::vec2 mouseDrag;
	int mouseEvent;
private:
	GUIManager();
	GUIManager(GUIManager const& other) = delete;
	GUIManager& operator=(GUIManager const& other) = delete;
};

class GUIRenderObject{
public:
	VAO vao;
	VBO vbo;
	virtual void Build();
	virtual void Render();
	virtual void Destroy();

	std::vector<float> vertices;
	glm::vec2 position;
	std::shared_ptr<Shader> shader;
	int idxcnt;
};

class SolidGUIRenderObject: public GUIRenderObject {
public:
	SolidGUIRenderObject();
	SolidGUIRenderObject(glm::vec3 fillcolor);
	virtual void Render() override;
	glm::vec3 fillcolor{ 0.3f, 0.3f, 0.3f };
};

class AtlasGUIRenderObject: public GUIRenderObject {
public:
	AtlasGUIRenderObject(std::shared_ptr<TextureArray2D> tex);
	virtual void Render() override;
	virtual void Build() override;
	virtual void Destroy() override;
	std::shared_ptr<TextureArray2D> arr_tex;
	VBO vbo_uv;
	std::vector<float> uvs;
};

class Panel: public GUI{
public:

	float centerX;
	float centerY;
	float height;
	float width;

	virtual void Render() override;
	virtual void Build() override;
	virtual void Destroy() override;

	std::shared_ptr<GUIRenderObject> renderObj;

protected:
	bool isBuilt = false;
};

class Button : public Panel{
public:
	void Update() override;
	virtual void OnClick() {};
	virtual void OnMouseEnter() {};
	virtual void OnMouseExit() {};
	std::string id;
protected:
	bool isHovering = false;
};


//

class Inventory : public Panel {
public:
	using BlockType = BlockDB::BlockType;
	static BlockDB::BlockType selectedBlkTy; //persists which block type is selected, when ui is closed
	static int selected;
	static const std::vector<BlockType> btnToBlkTy;
	Inventory();
	void Select(int num);
};

class InventoryButton: public Button{
public:
	int itemidx;
	InventoryButton(int idx, float cx, float cy, float w, float h);
	virtual void OnClick() override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseExit() override;

};

class InvenBtnImg: public Panel {
public:
	int imgidx = 0;
	InvenBtnImg(int imgidx);
	virtual void Build() override;
};
#endif
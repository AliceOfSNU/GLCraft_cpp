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
	glm::vec2 mouseXY;
	glm::vec2 mouseDelta;
	glm::vec2 mouseDrag;
	int mouseEvent;
private:
	GUIManager()=default;
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
	SolidGUIRenderObject() = default;
	SolidGUIRenderObject(glm::vec3 fillcolor);
	virtual void Render() override;
	glm::vec3 fillcolor{ 0.3f, 0.3f, 0.3f };
};

//class ImageGUIRenderObject : public GUIRenderObject {
//public:
//	ImageGUIRenderObject()
//};

class Panel: public GUI{
public:

	float centerX;
	float centerY;
	float height;
	float width;

	virtual void Render() override;
	virtual void Build() override;
	virtual void Destroy() override;

	std::unique_ptr<GUIRenderObject> renderObj;

protected:
	bool isBuilt;
};

class Button : public Panel{
public:
	void Update() override;
	std::function<void(Button&)> OnClick;
	std::function<void(Button&)> OnMouseEnter;
	std::function<void(Button&)> OnMouseExit;
	std::string id;
protected:
	bool isHovering = false;
};

//

class Inventory : public Panel {
public:
	int selected = 0;
	void Select(int num);
};

#endif
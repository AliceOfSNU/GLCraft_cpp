#pragma once
#ifndef GUI_H
#define GUI_H
#include "rendering.hpp"
#include "GLObjects.h"
#include <iostream>

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
	Shader shader = Shader("circleui.vert", "circleui.frag");
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
#endif
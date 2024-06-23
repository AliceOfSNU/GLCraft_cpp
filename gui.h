#pragma once
#ifndef GUI_H
#define GUI_H
/* Hanjun Kim 2024 */
#include "shader.h"
#include "GLObjects.h"

class CircleFill {
public:
	glm::vec3 color{ 1.0f, 0.7f, 0.7f};
	glm::vec2 center{ -0.5f, 0.0f };
	glm::vec2 resolution{ 800, 800 };
	glm::vec2 radius;

	CircleFill(float r) {
		radius = { r * 0.6, r };
		vao.Create();
		vbo.Create();

		vao.Bind();
		vbo.BufferData(vertices.data(), sizeof(vertices[0]) * 18);
		vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
		vao.Unbind();
	}

	//get the uniform id for texture sampler
	void Render(float percentage) {
		shader.use();
		shader.setVec2f("center", glm::value_ptr(center));
		shader.setVec2f("resolution", glm::value_ptr(resolution));
		shader.setVec3f("color", glm::value_ptr(color));
		shader.setVec2f("radius", glm::value_ptr(radius));

		vao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

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
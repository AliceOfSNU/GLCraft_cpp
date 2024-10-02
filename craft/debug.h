#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <vector>
#include <iostream>
#include "GLObjects.h"
#include "blocks.hpp"
#include "ray.h"
#include "camera.h"
#include "rendering.hpp"

class Gizmo {
public:
	virtual void Render() {
		std::cout << "Gizmo(base class) renderer called. this shouldn't happen" << std::endl;
	};
	virtual void Build() {};
	Gizmo() {
		vao.Create();
		vbo.Create();
	}
protected:
	VAO vao;
	VBO vbo;
};

class RayGizmo : public Gizmo {

public:
	Ray ray;
	RayGizmo(const Ray& _ray) : ray(_ray) {
		Build();
	};

	void Build() {
		const float MAX_LENGTH = 35.0;
		Ray::vec3 ep = ray.pos + ray.dir * MAX_LENGTH;
		vertices = std::vector<float>{
			//0: xyz							1: color
			ray.pos.x, ray.pos.y, ray.pos.z,	//1.0f, 0.0f, 0.0f,
			ep.x, ep.y, ep.z,					//0.0f, 1.0f, 0.0f
		};

		vao.Bind();
		vbo.BufferData(vertices.data(), sizeof(vertices[0])*6);
		vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
		//vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3*sizeof(float)));
		vao.Unbind();
	}

	void Render() {
		vao.Bind();
		glDrawArrays(GL_LINES, 0, 2);
	}

private:
	std::vector<float> vertices;

};

class FacesSelection {
public:

	FacesSelection() {
		vao.Create();
		vbo.Create();
		ebo.Create();
	}

	void AddFace(Block* block, int face) {
		GLuint v = block->PlaceFaceVertexData(vertices, face);
		idxCnt += block->PlaceFaceIndex(indices, vtxCnt, face);
		vtxCnt += v;
	}

	void Build() {
		vao.Bind();
		vbo.BufferData(vertices.data(), sizeof(vertices[0]) * vertices.size());
		vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
		//vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		ebo.BufferData(indices.data(), sizeof(indices[0]) * indices.size());
		vao.Unbind();
		ebo.Unbind();
	}

	void Render() {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		vao.Bind();
		glDrawElements(GL_TRIANGLES, idxCnt, GL_UNSIGNED_INT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

private:
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;
	GLuint vtxCnt = 0, idxCnt = 0;
	VAO vao;
	VBO vbo;
	EBO ebo;
};


class Debug {
public:
	static void DrawRay(const Ray& ray) {
		gizmos.push_back(new RayGizmo(ray));
	}

	static void Render() {
		for (auto giz : gizmos) giz->Render();
	}

	static std::vector<Gizmo*> gizmos;

private:
	Debug() = delete;
	Debug(Debug const& other) = delete;
	Debug& operator=(Debug const& other) = delete;

};

#endif // !DEBUG_H

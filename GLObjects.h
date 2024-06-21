#pragma once
#ifndef GLOBEJCTS_H
#define GLOBJECTS_H

/* Hanjun Kim 2024 */

#include<glad/glad.h>
#include <stb/stb_image.h>
#include "shader.h"
#include<cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>

template <typename T>
class DataBuffer {
public:
	T* data;
	size_t length, width;

	DataBuffer(): length(-1), width(-1){}
	DataBuffer(size_t l, size_t w) :length(l), width(w) {
		data = new T[l * w];
	}

	~DataBuffer() {
		delete[] data;
	}
	size_t size() { return length * width * sizeof(T); }
	size_t numel() { return length * width; }
	void copy_data(size_t dest_position, const void* source, size_t num) {
		memcpy(&data[dest_position*width], source, num * sizeof(T));
	}
	void view_data(size_t start_n, size_t num) {
		for (size_t i = start_n; i < start_n + num; ++i) {
			for (size_t j = 0; j < width; ++j) {
				std::cout << data[i * width + j] << ',';
			}
		}
	}

	T* GetPtr(size_t n) {
		assert(n < length);
		return data + (n * width);
	}
};


class VBO {
public:
	GLuint ID{};

	void Create();
	void Bind();
	void BufferData(GLfloat* vertices, GLsizeiptr size);
	void Unbind();
	void Delete();
};

class EBO {
public:
	GLuint ID{};

	void Create();
	void Bind();
	void BufferData(GLuint* indices, GLsizeiptr size);
	void Unbind();
	void Delete();
};

class VAO {
public:
	GLuint ID{};

	void Create();
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();

private:
	bool _is_bound = false;
};

class Texture2D {
public:
	GLuint ID{};
	Texture2D(const char* image_path, GLenum slot, GLenum format);

	void Bind();
	void UnBind();
	void Delete();
};

class TextureArray2D {
public:
	GLuint ID{};
	size_t width, height, nlayers;
	TextureArray2D(const char* image_path, size_t width, size_t height, size_t nlayers, GLenum format);
	
	void Bind();
	void UnBind();
	void Delete();
};
#endif
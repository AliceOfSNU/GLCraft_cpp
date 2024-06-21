#include "GLObjects.h"

void VBO::Create() {
	glGenBuffers(1, &ID);
	Bind();
}

void VBO::BufferData(GLfloat* vertices, GLsizeiptr size) {
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}


void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}

// EBO class
void EBO::Create() {
	glGenBuffers(1, &ID);
	Bind();
}

void EBO::BufferData(GLuint* indices, GLsizeiptr size) {
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void EBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete() {
	glDeleteBuffers(1, &ID);
}

// VAO class
void VAO::Create() {
	glGenVertexArrays(1, &ID);
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
	assert(_is_bound);
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::Bind() {
	glBindVertexArray(ID);
	_is_bound = true;
}

void VAO::Unbind() {
	glBindVertexArray(0);
	_is_bound = false;
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
	_is_bound = false;
}

Texture2D::Texture2D(const char* image_path, GLenum slot, GLenum format){
	int imgw, imgh, imgch;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image_path, &imgw, &imgh, &imgch, 0);
	//create gl texture

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//make sure to use correct format(RGB/BGR/RGBA?)
	glTexImage2D(GL_TEXTURE_2D, 0, format, imgw, imgh, 0, format, GL_UNSIGNED_BYTE, bytes);
	glGenerateMipmap(GL_TEXTURE_2D);
	//free and unload
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture2D::Bind() {
	glBindTexture(GL_TEXTURE_2D, ID);
}


void Texture2D::UnBind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Delete() {
	glDeleteTextures(1, &ID);
};

TextureArray2D::TextureArray2D(const char* image_path, size_t w, size_t h, size_t l, GLenum format): width(w), height(h), nlayers(l) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);

	int imgw, imgh, imgch;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image_path, &imgw, &imgh, &imgch, 0);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);


	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, nlayers, 0, format, GL_UNSIGNED_BYTE, bytes);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, nlayers, format, GL_UNSIGNED_BYTE, bytes);

	stbi_image_free(bytes);
	Bind();
};

void TextureArray2D::Bind() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
}


void TextureArray2D::UnBind() {
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void TextureArray2D::Delete() {
	glDeleteTextures(1, &ID);
};

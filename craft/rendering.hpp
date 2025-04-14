#pragma once
#include <vector>
#include <set>
#include <iostream>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>
#include <glad/glad.h>

#include "GLObjects.h"
#include "camera.h" 
#include "blocks.hpp"

class RenderObject {
public:

	enum RenderMode {
		OPAQUE, CUTOUT, TRANSPARENT
	};
	RenderMode mode;

	RenderObject() = default;
	RenderObject(RenderMode _mode);

	void Build();
	void PlaceBlockFaceData(BlockDB::BlockType blkTy, glm::f32vec3 offset, unsigned int face, int8_t light_level);
	void CreateBuffers();
	void DeleteBuffers();

	// built means buffer holds meaningful data
	// and is ready to be rendered.
	// however, it will only be rendered when isRender = true
	bool isBuilt = false;

	// only objects with this flag enabled will be rendered.
	// an object can be built and yet not be rendered
	// if isBuilt flag is on but this flag is off.
	// for example, far away chunks that are still in memory.
	bool isRender = true;

	// GLObjects like VertexArrays Objects and Buffer Objects
	// are bound to a renderobject for its lifetime.
	// they are created in renderobject's constructor
	// and destroyed in renderobject's destructor
	VAO vao;
	VBO vbo_pos, vbo_uv, vbo_light;
	EBO ebo;

	std::vector<GLfloat> vtxdata, uvdata, lightdata;
	std::vector<GLuint> idxdata;

    // how much data transferred to GLObjects,
    // not vtxdata.size() or idxdata.size()
    // interal storage can actually be empty
	size_t vtxcnt = 0, idxcnt = 0;

private:

	bool hasBuffers; 

};

class Shader
{
public:
    unsigned int ID;

    Shader() : ID(0) {}; // default constructor
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath);
   
    void use() const
    {
        glUseProgram(ID);
    }

    // utility uniform functions
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setMat4f(const char* name, const GLfloat* value_ptr) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, value_ptr);
    }
    void setVec3f(const char* name, const GLfloat* value_ptr) const {
        glUniform3fv(glGetUniformLocation(ID, name), 1, value_ptr);
    }
    void setVec2f(const char* name, const GLfloat* value_ptr) const {
        glUniform2fv(glGetUniformLocation(ID, name), 1, value_ptr);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type);
};
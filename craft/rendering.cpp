#include "rendering.hpp"


RenderObject::RenderObject(RenderMode _mode):mode(_mode), isBuilt(false), hasBuffers(false), isRender(true) {}


// appends block's mesh and texture data into internal storage vector
void RenderObject::PlaceBlockFaceData(BlockDB::BlockType blkTy, glm::f32vec3 pos, unsigned int face) {
	BlockDB::BlockDataRow& row = BlockDB::GetInstance().tbl[blkTy];
	BlockMeshData& mesh = BlockDB::GetInstance().GetMeshData(row.meshType);
	// place vertex data. 4 vertices of a square * 3 (xyz)
	for (int i = 0; i < 4*3;) {
		vtxdata.push_back(mesh.faceVerticesData[face][i++] + pos.x);
		vtxdata.push_back(mesh.faceVerticesData[face][i++] + pos.y);
		vtxdata.push_back(mesh.faceVerticesData[face][i++] + pos.z);
	}

	// place uv data. 4 vertices of a square
	const static float uvFace[4][2]{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f},
	};
	float texf = (float)row.faceTextures[face];
	for (int v = 0; v < 4; ++v) {
		uvdata.push_back(uvFace[v][0]);
		uvdata.push_back(uvFace[v][1]);
		uvdata.push_back(texf);
	}

	// place idx data
	idxdata.push_back(vtxcnt + 0);
	idxdata.push_back(vtxcnt + 1);
	idxdata.push_back(vtxcnt + 3);
	idxdata.push_back(vtxcnt + 3);
	idxdata.push_back(vtxcnt + 1);
	idxdata.push_back(vtxcnt + 2);

	vtxcnt += 4;
	idxcnt += 6;
}


void RenderObject::Build() {
	if (isBuilt) return;
	CreateBuffers();

	vao.Bind();
	vbo_pos.BufferData(vtxdata.data(), sizeof(vtxdata[0]) * vtxdata.size());
	vao.LinkAttrib(vbo_pos, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	vbo_uv.BufferData(uvdata.data(), sizeof(uvdata[0]) * uvdata.size());
	vao.LinkAttrib(vbo_uv, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	ebo.BufferData(idxdata.data(), sizeof(idxdata[0]) * idxdata.size());
	vao.Unbind();
	ebo.Unbind();

	// once built, we can dispose of internal storage.
	vtxdata.clear();
	uvdata.clear();
	idxdata.clear();
	
	isBuilt = true;
}

void RenderObject::CreateBuffers() {
	if (hasBuffers) return;

	vao.Create();
	vbo_pos.Create();
	vbo_uv.Create();
	ebo.Create();

	hasBuffers = true;
}

void RenderObject::DeleteBuffers() {
	if (hasBuffers) {
		vao.Delete();
		vbo_pos.Delete();
		vbo_uv.Delete();
		ebo.Delete();
	}

	vtxcnt = 0;
	idxcnt = 0;

	isBuilt = false;
	hasBuffers = false;
}

// Shader

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}


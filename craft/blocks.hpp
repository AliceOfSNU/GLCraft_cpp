#pragma once
#include <vector>
#include "GLObjects.h"

#define INOUT
#define OUT

// forward declarations
class Block;
class BlockDB;

struct BlockMeshData {
	std::vector<std::vector<float>> faceVerticesData;

	static BlockMeshData CubeMesh;
	static BlockMeshData FlowerMesh;
};


class BlockDB {
public:
	enum BlockType {
		BLOCK_AIR, BLOCK_GRASS, BLOCK_DIRT, BLOCK_GRANITE, BLOCK_SNOW_SOIL, BLOCK_SAND, BLOCK_WATER, BLOCK_BIRCH_LOG, BLOCK_ELM_LOG, BLOCK_FOILAGE, BLOCK_POPPY, BLOCK_DANDELION, BLOCK_CYAN_FLOWER, BLOCK_COUNT
	};

	//this should be in opposite order
	enum BlockTextures {
		DANDELION, POPPY, CYAN_FLOWER, FOILAGE, ELM_SIDE, ELM_TOP, BIRCH_SIDE, BIRCH_TOP, WATER, GRANITE, SNOW, SNOW_SIDE, SAND, GRASS_TOP, GRASS_SIDE, DIRT, NONE
	};

	enum RenderType {
		SOLID, TRANSPARENT, CUTOUT, INVISIBLE
	};

	enum MeshType {
		CUBE, FLOWER
	};

	struct BlockDataRow {
		BlockType type;
		std::vector<BlockTextures> faceTextures;		//which Texture to put on each face
		RenderType renderType;
		MeshType meshType;

		int numFaces() {
			return faceTextures.size();
		}
	};

	static BlockDB& GetInstance() {
		static BlockDB instance;
		return instance;
	}

	std::vector<BlockDataRow> tbl;
	bool isSolidCube(BlockType ty);
	BlockMeshData& GetMeshData(MeshType ty);

private:
	BlockDB();
	BlockDB(BlockDB const& other) = delete;
	BlockDB& operator=(BlockDB const& other) = delete;
};


class Block {
public:
	glm::f32vec3 pos;
	BlockDB::BlockDataRow* blockData;
	Block();
	Block(BlockDB::BlockType type);

	static float vertexPositions[8][3];
	static int faces[6][4];
	static int faceElements[6][6];
	static float facePositions[6][12];
	enum Face
	{
		FRONT, RIGHT, BACK, LEFT, TOP, BOTTOM
	};

	using vf = std::vector<GLfloat>;
	using vi = std::vector<GLuint>;

	//iterators are automatically advanced.
	//returns the number of added vertices
	GLuint PlaceFaceTexturesData(float*& dest, int face);
	GLuint PlaceFaceTexturesData(vf& dest, int face);

	GLuint PlaceFaceVertexData(float*& dest, int face);
	GLuint PlaceFaceVertexData(vf& dest, int face);

	GLuint PlaceFaceIndex(vi& dest, GLuint vtxn, int face);

	//merged version of Place___Data.
	GLuint PlaceFaceData(
		vf& vtxit, vf& uvit, vi& idxit, INOUT GLuint& vtxn, int face
	);

};

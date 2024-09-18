#pragma once
#ifndef WORLD_H
#define WORLD_H

/* Hanjun Kim 2024 */

#define INOUT
#define OUT

#include<glad/glad.h>
#include <stb/stb_image.h>
#include "shader.h"
#include<cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include "GLObjects.h"
#include "map.cpp"
#include "layers.cpp"

using pii = std::pair<int, int>;
using namespace MapGen;

class BlockDB {
public:
	enum BlockType {
		BLOCK_GRASS, BLOCK_DIRT, BLOCK_GRANITE, BLOCK_SNOW_SOIL, BLOCK_SAND, BLOCK_WATER, BLOCK_BIRCH_LOG, BLOCK_ELM_LOG, BLOCK_FOILAGE, BLOCK_COUNT
	};

	//this should be in opposite order
	enum BlockTextures {
		FOILAGE, ELM_SIDE, ELM_TOP, BIRCH_SIDE, BIRCH_TOP, WATER, GRANITE, SNOW, SNOW_SIDE, SAND, GRASS_TOP, GRASS_SIDE, DIRT
	};

	struct BlockDataRow {
		BlockType type;
		BlockTextures faceTextures[6];		//which Texture to put on each face
	};

	static BlockDB& GetInstance() {
		static BlockDB instance;
		return instance;
	}

	std::vector<BlockDataRow> tbl;
private:
	BlockDB();
	BlockDB(BlockDB const& other) = delete;
	BlockDB& operator=(BlockDB const& other) = delete;
};

class BiomeDB {
public:
	struct BiomeDataRow {
		std::vector<int> surfaceBlockCnts;
		std::vector<BlockDB::BlockType> surfaceBlockTypes;
		//plantation type, avg tmp, avg prcp, 
	};

	static BiomeDB& GetInstance() {
		static BiomeDB instance;
		return instance;
	}

	std::vector<BiomeDataRow> biomes;

private:
	BiomeDB();
	BiomeDB(BiomeDB const& other) = delete;
	BiomeDB& operator=(BiomeDB const& other) = delete;
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

/*
chunking manages rendering of multiple blocks.
a chunk records positions of blocks and does not render adjacent, overlapping faces.
this will also provide a logical grouping of nearby blocks,
*/
class Chunk {
public:
	using ivec3 = glm::ivec3;
	using vec3 = glm::vec3;
	static constexpr int SZ = 32, HEIGHT = 32; //a chunk is SZ*HEIGHT*SZ large. the y coordinate is up.
	Block* grid[SZ][HEIGHT][SZ]; //the blocks are conveniently stored in a 3d array.
	
	int blockHeight[SZ][SZ]; //the number of blocks in each column
	BiomeType blockBiome[SZ][SZ]; //the biome type for each column
	
	size_t blockCnt;
	GLuint vtxCnt; //number of vertices to render(VBO)
	GLuint idxCnt; //number of indices to render(EBO)
	ivec3 basepos; //the position of minimum x, y, z.

	bool isBuilt, requiresRebuild;
	/*
	* The vertices of a cube are always numbered as below:
	* 
	*      *7---*6
	*     /|   / |
	*	 *4---*5 |
	*    | *3-|-*2 
	*    |/   | /
	*    *0---*1
	*/


	//blocks should be added to a chunk after construction.
	Chunk();
	Chunk(const ivec3& pos);

	//main functions
	void Build();
	void ReBuild();
	void Render();
	void DeleteBuffers();

	//manipulation
	void DestroyBlockAt(const ivec3& bidx);
	void PlaceBlockAt(const ivec3& bidx);
	
	//utils
	//testing worldpos lies inside this chunk's boundary
	bool TestAABB(vec3 worldpos);
	ivec3 FindBlockIndex(vec3 worldpos);
	static ivec3 WorldToChunkIndex(vec3 worldpos);
	ivec3 BlockWorldToGridIdx(const ivec3& worldidx);
	ivec3 BlockGridToWorldIdx(const ivec3& grididx);
	//ivec3 ChunkToWorldCoordinate(ivec3 chunkpos);

private:
	//data
	std::vector<GLfloat> vtxdata, uvdata;
	std::vector<GLuint> idxdata;

	//renderer
	VAO vao;
	VBO vbo_pos, vbo_uv;
	EBO ebo;
};


class FractalNoise2D {
public:
	double persistance;
	std::vector<double>octaves;
	double samplePoint(double x, double y);
private:

	class PerlinNoise2D{
	public:
		double samplePoint(double x, double y);
	private:
		glm::f64vec2 simpleNoiseFn(int x, int y);
		double dotGradient(int ix, int iy, double x, double y);
		inline double lerp(double x, double y, double t);
	};

	PerlinNoise2D perlin;

};

/*
MapGeneration class stores and creates maps as they are needed by TerrainGeneration
It wraps around the ProceduralMap library and provides conversion between the map's scale and world meter units.
*/

class TerrainGeneration {
public:
	FractalNoise2D heightNoise;
	FractalNoise2D roughnessNoise;
	static const int MAP_SIZE = 512;
	using BiomeMap_t = Map<BiomeData, MAP_SIZE>;
	using LandscapeMap_t = Map<OceanMapData, MAP_SIZE>;
	std::map<pii, BiomeMap_t> biomeMap;
	std::map<pii, LandscapeMap_t> oceanMap;

	TerrainGeneration();

	//6월까지 목표 -> grasslands biome만 제대로 생성
	//fills grid with granite up to height sampled from noise
	void GenerateRocks(Chunk* chunk);

	//10월까지 목표 -> 6개 biome완성
	//entry point
	void Generate(Chunk* chunk);

	/// <summary>
	/// given a world x-z position, outputs the biome map containing that position.
	/// the base position of the map does not equal the input position.
	/// thus, to properly index the map, use the utility function provided by the returned map.
	/// </summary>
	/// <param name="basepos">the world x-z position. the coordinates must be divisible by the returned map's scale</param>
	/// <param name="biomeMp">OUT biome map containing the query position</param>
	void FindOrCreateMap(pii basepos, OUT BiomeMap_t& biomeMp);
	
	/// <summary>
	/// Uses Voronoi zoom to go from the maximum resolution 4x4 of biome map
	/// to block-wise specification of biome.
	/// This fills the blockBiome array of the chunk.
	/// </summary>
	/// <param name="chunk">the chunk to operate on</param>
	/// <param name="biomeMp">the map to zoom at</param>
	void GenerateBiomeFromMap(Chunk* chunk, const BiomeMap_t biomeMp);

	void GenerateTerrainHeightsFromMap(Chunk* chunk);

	/// <summary>
	/// Replaces top few blocks of terrain with biome-specific surface blocks.
	/// For instance, snowland gets 4 soil blocks and 1 snow-covered soil at the top
	/// </summary>
	/// <param name="chunk">the chunk to operate on</param>
	void ReplaceSurface(Chunk* chunk);
	
	void GeneratePlantation(Chunk* chunk);
	
protected:
	void GenerateMap(pii basepos, OUT BiomeMap_t& biomeMp);
};

class World {
public:
	using p3i = std::tuple<int, int, int>;
	using pii = std::pair<int, int>;
	std::map<p3i, Chunk*> allChunks;
	std::map<p3i, Chunk*> visChunks;
	TerrainGeneration worldgen;
	glm::ivec3 centerChunkIdx{ 0,0,0 };

	static constexpr int VIS_WORLD_SZ = 5, HVIS_WORLD_SZ = 2, VIS_WORLD_HEIGHT = 3, HVIS_WORLD_HEIGHT = 1;

	World(glm::vec3 spawnPoint);

	void CreateInitialChunks(glm::vec3 playerPosition); //creates chunks to start with.
	Chunk* CurrentChunk(glm::vec3& position); //Pointer to current chunk.
	Chunk* GetChunkByIndex(const glm::ivec3& idx);
	Chunk* GetChunkContainingBlock(const glm::ivec3& worldIdx);
	void UpdateChunks(glm::vec3& playerPosition);
	void Render();
	void Build();

private:
	Chunk* findOrCreateChunk(const p3i& chunkIdx);
};
#endif
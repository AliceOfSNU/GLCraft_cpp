#pragma once
#ifndef WORLD_H
#define WORLD_H

/* Hanjun Kim 2024 */

#define INOUT
#define OUT

#include<glad/glad.h>
#include <stb/stb_image.h>
#include<cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <algorithm>
#include <map>
#include <iostream>
#include <chrono>
#include "GLObjects.h"
#include "map.cpp"
#include "layers.cpp"
#include "rendering.hpp"
#include "blocks.hpp"
#include "plants.hpp"

using pii = std::pair<int, int>;
using namespace MapGen;


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


/*
chunking manages rendering of multiple blocks.
a chunk records positions of blocks and does not render adjacent, overlapping faces.
this will also provide a logical grouping of nearby blocks,
*/
class Chunk {
public:
	using ivec3 = glm::ivec3;
	using vec3 = glm::vec3;
	using BlockType = BlockDB::BlockType;
	static constexpr int SZ = 32, HEIGHT = 32; //a chunk is SZ*HEIGHT*SZ large. the y coordinate is up.
	BlockType grid[SZ][HEIGHT][SZ]; //the blocks are conveniently stored in a 3d array.
	
	int blockHeight[SZ][SZ]; //the number of blocks in each column
	BiomeType blockBiome[SZ][SZ]; //the biome type for each column
	
	size_t blockCnt;
	//GLuint vtxCnt; //number of vertices to render(VBO)
	//GLuint idxCnt; //number of indices to render(EBO)
	ivec3 basepos; //the position of minimum x, y, z.
	ivec3 chunkIdx; //unique integer index for this chunk.

	bool isBuilt, requiresRebuild, initialized;
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
	Chunk(const ivec3& pos, const ivec3& chunkIdx);

	//main functions
	void Build();
	void ReBuild();

	//manipulation
	void DestroyBlockAt(const ivec3& bidx);
	void PlaceBlockAtCompileTime(const ivec3& bidx, const BlockDB::BlockType blkTy);
	
	//utils
	//testing worldpos lies inside this chunk's boundary
	bool TestAABB(vec3 worldpos);
	ivec3 FindBlockIndex(vec3 worldpos);
	static ivec3 WorldToChunkIndex(vec3 worldpos);
	ivec3 BlockWorldToGridIdx(const ivec3& worldidx);
	ivec3 BlockGridToWorldIdx(const ivec3& grididx);
	//ivec3 ChunkToWorldCoordinate(ivec3 chunkpos);

	RenderObject solidRenderObj;
	RenderObject cutoutRenderObj;
	RenderObject waterRenderObj;

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

	FractalNoise2D slowNoise, fastNoise;
	static const int MAP_SIZE = 512;
	static const int WS_MAP_SPAN = 512*8;
	using BiomeMap_t = Map<BiomeData, MAP_SIZE>;
	using LandscapeMap_t = Map<LandscapeData, MAP_SIZE>;
	std::map<pii, BiomeMap_t> biomeMap;
	std::map<pii, LandscapeMap_t> landscapeMap;

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
	/// <param name="biomeMp">OUT landscape map containing the query position</param>
	void FindOrCreateMap(pii basepos, OUT BiomeMap_t& biomeMp, OUT LandscapeMap_t& lscapeMp);
	
	/// <summary>
	/// Uses Voronoi zoom to go from the maximum resolution 4x4 of biome map
	/// to block-wise specification of biome.
	/// This fills the blockBiome array of the chunk.
	/// </summary>
	/// <param name="chunk">the chunk to operate on</param>
	/// <param name="biomeMp">the map to zoom at</param>
	void GenerateBiomeFromMap(Chunk* chunk, const BiomeMap_t biomeMp);

	/// <summary>
	/// uses landscape paramters (absolute scale and roughness)
	/// to modulate and combine to perlin noises and interpolate to get
	/// block-wise terrain height
	/// </summary>
	/// <param name="chunk">chunk to operate on</param>
	/// <param name="lscapeMp">holds generation paramters</param>
	/// <param name="biomeMp">used to invert elevation to negative number for ocean</param>
	void GenerateTerrainHeightsFromMap(Chunk* chunk, const LandscapeMap_t lscapeMp, const BiomeMap_t biomeMp);

	/// <summary>
	/// Replaces top few blocks of terrain with biome-specific surface blocks.
	/// For instance, snowland gets 4 soil blocks and 1 snow-covered soil at the top
	/// </summary>
	/// <param name="chunk">the chunk to operate on</param>
	void ReplaceSurface(Chunk* chunk);
	
	void GenerateBiomass(Chunk& chunk);
	
protected:
	void GenerateMap(pii basepos, OUT BiomeMap_t& biomeMp, OUT LandscapeMap_t& lscapeMp);
};

class World {
public:
	using p3i = std::tuple<int, int, int>;
	using pii = std::pair<int, int>;
	std::map<p3i, Chunk*> allChunks;
	std::map<p3i, Chunk*> visChunks;
	TerrainGeneration worldgen;
	glm::ivec3 centerChunkIdx{ 0,0,0 };

	static constexpr int VIS_WORLD_SZ = 7, HVIS_WORLD_SZ = 3, VIS_WORLD_HEIGHT = 3, HVIS_WORLD_HEIGHT = 1;

	static World& GetInstance() {
		static World instance = World({0.0f, 1.0f, 0.0f});
		return instance;
	}

	void CreateInitialChunks(glm::vec3 playerPosition); //creates chunks to start with.
	Chunk* CurrentChunk(const glm::vec3& position); //Pointer to current chunk.
	Chunk* GetChunkByIndex(const glm::ivec3& idx);
	Chunk* GetChunkContainingBlock(const glm::ivec3& worldIdx);
	void UpdateChunks(glm::vec3& playerPosition);
	void Build();

private:
	World(glm::vec3 centerPoint);
	World(World const& other) = delete;
	World& operator=(World const& other) = delete;
	Chunk* findOrCreateChunk(const p3i& chunkIdx);
};
#endif
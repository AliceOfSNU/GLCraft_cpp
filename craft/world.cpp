#include "world.h"

/* Hanjun Kim 2024 */

float Block::vertexPositions[8][3] = {
	{-0.5f, -0.5f, 0.5f},//-X-Y+Z 0
	{0.5f, -0.5f, 0.5f},//+X-Y+Z 1
	{0.5f, -0.5f, -0.5f},//+X-Y-Z 2
	{-0.5f, -0.5f, -0.5f},//-X-Y-Z 3
	{-0.5f, 0.5f, 0.5f},//-X-Y+Z 4
	{0.5f, 0.5f, 0.5f},//+X-Y+Z 5
	{0.5f, 0.5f, -0.5f},//+X-Y-Z 6
	{-0.5f, 0.5f, -0.5f},//-X-Y-Z 7
};

int Block::faces[6][4] = {
	{0, 1, 5, 4},
	{1, 2, 6, 5},
	{2, 3, 7, 6},
	{3, 0, 4, 7},
	{4, 5, 6, 7},
	{3, 2, 1, 0},
};

int Block::faceElements[6][6] = {
	{0,1,4,4,1,5},
	{1,2,5,5,2,6},
	{2,3,6,6,3,7},
	{3,0,7,7,0,4},
	{4,5,7,7,5,6},
	{3,2,0,0,2,1},
};

float Block::facePositions[6][12] = {
	{-0.5,-0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,0.5, -0.5,0.5,0.5,}, //FRONT(z=+0.5)
	{0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f}, //RIGHT(x=+0.5) 
	{0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,0.5,-0.5,0.5,0.5,-0.5}, //BACK(z=-0.5)
	{-0.5,-0.5,-0.5,-0.5,-0.5,0.5,-0.5,0.5,0.5,-0.5,0.5,-0.5 }, //LEFT(x=-0.5)
	{-0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,-0.5,-0.5,0.5,-0.5}, //TOP
	{-0.5,-0.5,-0.5,0.5,-0.5,-0.5,0.5,-0.5,0.5,-0.5,-0.5,0.5}, //BOTTOM
};

Block::Block() {
	pos = glm::f32vec3(0.0f);
	blockData = &BlockDB::GetInstance().tbl[BlockDB::BlockType::BLOCK_DIRT];
}

Block::Block(BlockDB::BlockType type) {
	pos = glm::f32vec3(0.0f);
	blockData = &BlockDB::GetInstance().tbl[type];
}

//private singleton constructor
BlockDB::BlockDB() {
	tbl.resize(BlockType::BLOCK_COUNT);
	//DATA
													//ID						 /TEXTUURE FRONT				LEFT						 BACK						RIGHT						TOP							BOTTOM
	tbl[BlockType::BLOCK_DIRT]		= BlockDataRow{ BlockType::BLOCK_DIRT,		 { BlockTextures::DIRT,			BlockTextures::DIRT,		 BlockTextures::DIRT,		BlockTextures::DIRT,		BlockTextures::DIRT,		BlockTextures::DIRT} };
	tbl[BlockType::BLOCK_GRASS]		= BlockDataRow{ BlockType::BLOCK_GRASS,		 { BlockTextures::GRASS_SIDE,	BlockTextures::GRASS_SIDE,	 BlockTextures::GRASS_SIDE, BlockTextures::GRASS_SIDE,	BlockTextures::GRASS_TOP,	BlockTextures::DIRT} };
	tbl[BlockType::BLOCK_SAND]		= BlockDataRow{ BlockType::BLOCK_SAND,		 { BlockTextures::SAND,			BlockTextures::SAND,		 BlockTextures::SAND,		BlockTextures::SAND,		BlockTextures::SAND,		BlockTextures::SAND} };
	tbl[BlockType::BLOCK_WATER]		= BlockDataRow{ BlockType::BLOCK_WATER,		 {BlockTextures::GRANITE,		BlockTextures::GRANITE,		 BlockTextures::GRANITE,	BlockTextures::GRANITE,		BlockTextures::GRANITE,		BlockTextures::GRANITE}};
	tbl[BlockType::BLOCK_GRANITE]	= BlockDataRow{ BlockType::BLOCK_GRANITE,	 { BlockTextures::GRANITE,		BlockTextures::GRANITE,		 BlockTextures::GRANITE,	BlockTextures::GRANITE,		BlockTextures::GRANITE,		BlockTextures::GRANITE} };
	tbl[BlockType::BLOCK_SNOW_SOIL]	= BlockDataRow{ BlockType::BLOCK_SNOW_SOIL,	 { BlockTextures::SNOW_SIDE,	BlockTextures::SNOW_SIDE,	 BlockTextures::SNOW_SIDE,	BlockTextures::SNOW_SIDE,	BlockTextures::SNOW,		BlockTextures::DIRT} };

}

BiomeDB::BiomeDB() {
	using BiomeType = MapGen::BiomeType;
	using BlockType = BlockDB::BlockType;
	biomes.resize(BiomeType::BIOME_COUNT);

	//DATA
	biomes[BiomeType::DESERT].surfaceBlockTypes.push_back(BlockType::BLOCK_SAND);
	biomes[BiomeType::DESERT].surfaceBlockCnts.push_back(4);

	biomes[BiomeType::GRASSLAND].surfaceBlockTypes = { BlockType::BLOCK_GRASS, BlockType::BLOCK_DIRT };
	biomes[BiomeType::GRASSLAND].surfaceBlockCnts = { 1, 4 };

	biomes[BiomeType::SHRUBLAND].surfaceBlockTypes = { BlockType::BLOCK_GRASS, BlockType::BLOCK_DIRT };
	biomes[BiomeType::SHRUBLAND].surfaceBlockCnts = { 1, 4 };

	biomes[BiomeType::RAINFOREST].surfaceBlockTypes = { BlockType::BLOCK_GRASS, BlockType::BLOCK_DIRT };
	biomes[BiomeType::RAINFOREST].surfaceBlockCnts = { 1, 4 };

	biomes[BiomeType::SNOWLAND].surfaceBlockTypes = { BlockType::BLOCK_SNOW_SOIL, BlockType::BLOCK_DIRT };
	biomes[BiomeType::SNOWLAND].surfaceBlockCnts = { 1, 4 };

	//END DATA
}


/*
* following functions places each face's data onto memory location pointed by dest
* the data can be which texture to map onto the face, the vertex locations of the face etc..
* the functions return the number of elements written to dest
	 *3-----*2
	 |		|
	 |		|
	 *0-----*1
*/
GLuint Block::PlaceFaceTexturesData(float*& dest, int face) {
	//places U,V,T(texture index in array) data of four vertices belonging to 'face'.
	const float uvFace[4][2]{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f},
	};

	// 0.0 0.0 10.0 /vertex0
	// 1.0 0.0 10.0 /vertex1
	// 1.0 1.0 10.0 /vertex2
	// 0.0 1.0 10.0 /vertex3
	float texf = (float)blockData->faceTextures[face];
	for (int i = 0; i < 4; ++i) {
		*dest = uvFace[i][0];
		*(dest + 1) = uvFace[i][1];
		*(dest + 2) = texf;
		dest += 3;
	}

	//returns 4 = 3(U, V, T)* 4(#vertices)
	return 4;
}

GLuint Block::PlaceFaceTexturesData(vf& dest, int face) {
	const float uvFace[4][2]{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f},
	};
	float texf = (float)blockData->faceTextures[face];
	for (int v = 0; v < 4; ++v) {
		dest.push_back(uvFace[v][0]);
		dest.push_back(uvFace[v][1]);
		dest.push_back(texf);
	}

	//returns 4 = 3(U, V, T)* 4(#vertices)
	return 4;
}

GLuint Block::PlaceFaceVertexData(float*& dest, int face) {
	for (int i = 0; i < 12; i += 3) {
		*dest = Block::facePositions[face][i] + pos.x;
		*(dest + 1) = Block::facePositions[face][i + 1] + pos.y;
		*(dest + 2) = Block::facePositions[face][i + 2] + pos.z;
		dest += 3;
	}

	//returns 4(#vertices)
	return 4;
}

GLuint Block::PlaceFaceVertexData(vf& dest, int face) {
	for (int i = 0; i < 12;) {
		dest.push_back(Block::facePositions[face][i++] + pos.x);
		dest.push_back(Block::facePositions[face][i++] + pos.y);
		dest.push_back(Block::facePositions[face][i++] + pos.z);
	}

	//returns 4(#vertices)
	return 4;
}

GLuint Block::PlaceFaceIndex(vi& dest, GLuint vtxn, int face) {
	dest.push_back(vtxn + 0);
	dest.push_back(vtxn + 1);
	dest.push_back(vtxn + 3);
	dest.push_back(vtxn + 3);
	dest.push_back(vtxn + 1);
	dest.push_back(vtxn + 2);
	return 6;
}

//this directly increments vertex count
GLuint Block::PlaceFaceData(
	vf& vtxit, vf& uvit, vi& idxit, INOUT GLuint& vtxn, int face
) {
	GLuint vtxCnt = PlaceFaceVertexData(vtxit, face);
	PlaceFaceTexturesData(uvit, face);
	GLuint idxCnt = PlaceFaceIndex(idxit, vtxn, face);
	vtxn += vtxCnt;
	return idxCnt;
}

Chunk::Chunk() :blockCnt(0), vtxCnt(0), idxCnt(0), isBuilt(false), requiresRebuild(false) { 
	basepos = ivec3(0, 0, 0); 
};

Chunk::Chunk(const ivec3& pos) : blockCnt(0), vtxCnt(0), idxCnt(0), basepos(pos), isBuilt(false) {
	//nothing to do
};

void Chunk::Build() {
	
	//Building a chunk twice is an error, because we could be wasting computation.
	//isBuilt flag must be turned off before any rebuild.
	if (isBuilt) return;

	// At this point, we assume all blocks have been put to our grid
	// when more blocks are added, or blocks are deleted from the chunk,
	// the chunk must be rebuilt.
	
	//create data buffer(vector)
	//building takes a bit of memory but this goes to stack.
	vao.Create();
	vbo_pos.Create();
	vbo_uv.Create();
	ebo.Create();

	vtxdata.reserve(blockCnt * 6 * 4);
	uvdata.reserve(blockCnt * 6 * 3);
	idxdata.reserve(blockCnt * 6 * 6);

	vtxCnt = 0, idxCnt = 0;
	for (int i = 0; i < SZ; ++i) { //x dir
		for (int j = 0; j < HEIGHT; ++j) { //y dir
			for (int k = 0; k < SZ; ++k) { //z dir
				if (!grid[i][j][k]) continue;

				//check if other block exists in that direction
				//if not, add data for that direction's face.
				//not this implementation uses short circuiting in branches
				Block* block = grid[i][j][k];
				if (i == 0 || i > 0 && grid[i - 1][j][k] == nullptr)				idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::LEFT);
				if (i == SZ - 1 || i < SZ - 1 && grid[i + 1][j][k] == nullptr)		idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::RIGHT);
				if (j == 0 || j > 0 && grid[i][j-1][k] == nullptr)					idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::BOTTOM);
				if (j == HEIGHT - 1 || j < HEIGHT-1 && grid[i][j+1][k] == nullptr)	idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::TOP);
				if (k == 0 || k > 0 && grid[i][j][k-1] == nullptr)					idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::BACK);
				if (k == SZ - 1 || k < SZ - 1 && grid[i][j][k+1] == nullptr)		idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::FRONT);

				//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::FRONT);
				//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::TOP);
			}
		}
	}

	//remove existing buffers.

	//create data buffers.
	vao.Bind();
	vbo_pos.BufferData(vtxdata.data(), sizeof(vtxdata[0]) * vtxdata.size());
	vao.LinkAttrib(vbo_pos, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	vbo_uv.BufferData(uvdata.data(), sizeof(uvdata[0])*uvdata.size());
	vao.LinkAttrib(vbo_uv, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	ebo.BufferData(idxdata.data(), sizeof(idxdata[0])*idxdata.size());
	vao.Unbind();
	ebo.Unbind();

	isBuilt = true;

}

void Chunk::ReBuild() {
	if (!requiresRebuild) return;
	DeleteBuffers();
	Build();
	requiresRebuild = false;
}

void Chunk::DeleteBuffers() {
	
	//delete all buffers
	vao.Delete();
	vbo_pos.Delete();
	vbo_uv.Delete();
	ebo.Delete();

	//data can be freed. 
	vtxdata.clear();
	uvdata.clear();
	idxdata.clear();

	isBuilt = false;
}

void Chunk::Render() {
	//assumes shader and the texture is bound and activated.
	vao.Bind();
	glDrawElements(GL_TRIANGLES, idxCnt, GL_UNSIGNED_INT, 0);
}

void Chunk::DestroyBlockAt(const Chunk::ivec3& bidx) {
	delete grid[bidx.x][bidx.y][bidx.z];
	grid[bidx.x][bidx.y][bidx.z] = nullptr;
	requiresRebuild = true;//requires rebuild.
}

bool Chunk::TestAABB(vec3 worldpos) {
	//tests if worldpos is inside this chunk's boundary
	if (worldpos.x >= basepos.x && worldpos.x < basepos.x + SZ &&
		worldpos.y >= basepos.y && worldpos.y < basepos.y + HEIGHT &&
		worldpos.z >= basepos.z && worldpos.z < basepos.z + SZ) return true;

	return false;
}

Chunk::ivec3 Chunk::FindBlockIndex(vec3 worldpos) {
	worldpos -= basepos;
	return ivec3(worldpos.x + 0.5f, worldpos.y + 0.5f, worldpos.z + 0.5f);
}

Chunk::ivec3 Chunk::WorldToChunkIndex(vec3 worldpos) {
	worldpos += 0.5;
	Chunk::ivec3 currChunkIdx;
	//IMPORTANT! maybe you want to offset worldpos by 0.5 in every axis,
	//because visual chunk boundaries are at -0.5f off the chunk's basepos
	currChunkIdx.x = (worldpos.x >= 0.0 ? (int)(worldpos.x / Chunk::SZ) : (int)(worldpos.x / Chunk::SZ) - 1);
	currChunkIdx.y = (worldpos.y >= 0.0 ? (int)(worldpos.y / Chunk::HEIGHT) : (int)(worldpos.y / Chunk::HEIGHT) - 1);
	currChunkIdx.z = (worldpos.z >= 0.0 ? (int)(worldpos.z / Chunk::SZ) : (int)(worldpos.z / Chunk::SZ) - 1);
	return currChunkIdx;
}

Chunk::ivec3 Chunk::BlockWorldToGridIdx(const ivec3& worldIdx) {
	//converts global world-space idx of a block to the local grid idx inside this chunk.
	//in fact, the computation is chunk-agnostic, nontheless the function is not static
	//because its counterpart is not static.
	Chunk::ivec3 ret{};
	ret.x = worldIdx.x % Chunk::SZ;
	if (ret.x < 0) ret.x += Chunk::SZ;
	ret.y = worldIdx.y % Chunk::HEIGHT;
	if (ret.y < 0) ret.y += Chunk::HEIGHT;
	ret.z = worldIdx.z% Chunk::SZ;
	if (ret.z < 0) ret.z += Chunk::SZ;

	return ret;
}

Chunk::ivec3 Chunk::BlockGridToWorldIdx(const ivec3& gridIdx) {
	//converts the grid idx of a block in this chunk to the global world-space idx
	return Chunk::ivec3{ basepos.x + gridIdx.x, basepos.y + gridIdx.y, basepos.z + gridIdx.z };
}

/// Noise Generator

glm::f64vec2 FractalNoise2D::PerlinNoise2D::simpleNoiseFn(int ix, int iy) {
	const unsigned w = 8 * sizeof(unsigned);
	const unsigned s = w / 2;
	unsigned a = ix, b = iy;
	a *= 3284157443;
	b ^= a << s | a >> w - s;
	b *= 1911520717;
	a ^= b << s | b >> w - s;
	a *= 2048419325;
	float random = a * (3.14159265 / ~(~0u >> 1));

	return glm::f64vec2{ sin(random), cos(random) };
}

double FractalNoise2D::PerlinNoise2D::dotGradient(int ix, int iy, double x, double y) {
	glm::f64vec2 gradient = simpleNoiseFn(ix, iy);
	return ((x - (double)ix) * gradient.y + (y - (double)iy) * gradient.x);
}

inline double FractalNoise2D::PerlinNoise2D::lerp(double a, double b, double t) {
	return (1.0 - t) * a + t * b;
}

double FractalNoise2D::PerlinNoise2D::samplePoint(double x, double y) {
	//grid coords
	int x0 = x >= 0 ? (int)x : (int)x - 1;
	int y0 = y >= 0 ? (int)y : (int)y-1;
	int x1 = x0 + 1, y1 = y0 + 1;
	double sx = x - x0, sy = y - y0;

	double n0 = dotGradient(x0, y0, x, y);
	double n1 = dotGradient(x1, y0, x, y);
	double ix0 = lerp(n0, n1, sx);

	n0 = dotGradient(x0, y1, x, y);
	n1 = dotGradient(x1, y1, x, y);
	double ix1 = lerp(n0, n1, sx);

	return lerp(ix0, ix1, sy);
}

double FractalNoise2D::samplePoint(double x, double y) {
	double amount = 1.0;
	double result = 0.0;
	for (double f : octaves) {
		result += amount * perlin.samplePoint(f * x, f * y);
		amount *= persistance;
	}
	return result;
}


//-------- Terrain

TerrainGeneration::TerrainGeneration() {
	heightNoise.persistance = 0.5;
	roughnessNoise.persistance = 0.5;
	//base
	//heightNoise.octaves.push_back(0.004f);
	//heightNoise.octaves.push_back(0.01f);
	//heightNoise.octaves.push_back(0.05f);

	//mountains and plains are created by modulating height noise by roughness noise.
	heightNoise.octaves.push_back(0.05f);
	heightNoise.octaves.push_back(0.1f);
	roughnessNoise.octaves.push_back(0.005f);
}

void TerrainGeneration::GenerateRocks(Chunk* chunk) { //TO BE DEPRECATED
	const int base_terrain_offset = 0;
	const int base_terrain_scale = 40;
	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			double roughness = 0.5 + roughnessNoise.samplePoint(i + chunk->basepos.x + 0.5, k + chunk->basepos.z + 0.5);
			int elevation = base_terrain_offset + base_terrain_scale * roughness * heightNoise.samplePoint(i + chunk->basepos.x + 0.5, k + chunk->basepos.z + 0.5);
			int j = 0; //height in chunk
			for (; j < Chunk::HEIGHT; ++j) {
				if (chunk->basepos.y + j > elevation) break;
				BlockDB::BlockType type = BlockDB::BlockType::BLOCK_GRANITE;
				//if (chunk->basepos.y + j == elevation) type = BlockDB::BlockType::BLOCK_GRASS;
				Block* block = chunk->grid[i][j][k] = new Block(type);
				block->pos.x = chunk->basepos.x + i;
				block->pos.z = chunk->basepos.z + k;
				block->pos.y = chunk->basepos.y + j;
				chunk->blockCnt++;
			}
			chunk->blockHeight[i][k] = std::min(Chunk::HEIGHT, j);
		}
	}
}


void TerrainGeneration::GenerateMap(pii basepos, OUT BiomeMap_t& biomeMp) {
	// level 8
	Map<float, 1> baseMp({ basepos.first, basepos.second }, 512); //total map size is gonna be 512 * 8 = 4096 * 4096
	Map<float, 8> noiseMp = WhiteNoise<8>::Forward(baseMp);

	Map<OceanMapData, 8> bOceanMp8 = GenIslandLayer<8>::Forward(noiseMp);

	// level 16
	Map<OceanMapData, 16> bOceanMp16 = Zoom<OceanMapData, 8>::Forward(bOceanMp8);

	// level 32
	Map<OceanMapData, 32> bOceanMp32 = Zoom<OceanMapData, 16>::Forward(bOceanMp16);
	Map<PreClimateData, 32> climateMp32 = GenPreClimateLayer<32>::Forward(bOceanMp32);
	Map<BiomeData, 32> biomeMp32 = GenBiomeLayer<32>::Forward(climateMp32, bOceanMp32);

	// level 64
	Map<BiomeData, 64> biomeMp64 = Zoom<BiomeData, 32>::Forward(biomeMp32);
	//Map<LandscapeData, 64> landscapeMp64 = LandscapeLayer<64>::Forward(bOceanMp64);

	// level 128
	Map<BiomeData, 128> biomeMp128 = Zoom<BiomeData, 64>::Forward(biomeMp64);
	//Map<LandscapeData, 128> landscapeMp128 = Zoom<LandscapeData, 64>::Forward(landscapeMp64);

	//// level 256
	Map<BiomeData, 256> biomeMp256 = Zoom<BiomeData, 128>::Forward(biomeMp128); //return
	//Map<LandscapeData, 256> landscapeMp256 = Zoom<LandscapeData, 128>::Forward(landscapeMp128);

	//// level 512
	biomeMp = Zoom<BiomeData, 256>::Forward(biomeMp256);
	//Map<LandscapeData, 256> landscapeMp256 = Zoom<LandscapeData, 256>::Forward(landscapeMp256);

}

void TerrainGeneration::FindOrCreateMap(pii basepos, OUT BiomeMap_t& biomeMp) {
	//1. get the map base position
	pii mapbase = { (int)(basepos.first / MAP_SIZE) * MAP_SIZE,(int)(basepos.second / MAP_SIZE) * MAP_SIZE };
	mapbase.first -= MAP_SIZE / 2; //so that (0,0) is near the center of the map.
	mapbase.second -= MAP_SIZE / 2;
	
	//2. check cache
	if (biomeMap.count(mapbase)) {
		biomeMp = biomeMap[mapbase];
		return;
	}

	//3. create map if not exist
	GenerateMap(mapbase, OUT biomeMp);

	//4. cache the map 
	biomeMap[mapbase] = biomeMp;
	return;
}

void TerrainGeneration::GenerateBiomeFromMap(Chunk* chunk, const BiomeMap_t biomeMp) {
	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			MapGen::vec2i xz = biomeMp.WorldToMapPoint(chunk->basepos.x + i, chunk->basepos.z + k);
			chunk->blockBiome[i][k] = biomeMp.data[xz.x][xz.y].biomeType;
		}
	}

	//TODO: VORONOI zoom
	return;
}

void TerrainGeneration::ReplaceSurface(Chunk* chunk) {
	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			//double roughness = 0.5 + roughnessNoise.samplePoint(i + chunk->basepos.x + 0.5, k + chunk->basepos.z + 0.5);
			//1. get replacement data for biome
			BiomeDB::BiomeDataRow biome = BiomeDB::GetInstance().biomes[chunk->blockBiome[i][k]];
			int top = chunk->blockHeight[i][k] - 1;
			for (int b = 0; b < biome.surfaceBlockTypes.size(); ++b) {
				//2. replace top rock blocks with predefined surface block types
				BlockDB::BlockType surfType = biome.surfaceBlockTypes[b];
				int surfDepth = biome.surfaceBlockCnts[b];
				if (top < 0) break;
				for (int j = top; j > top-surfDepth && j >= 0 ; --j) {
					chunk->grid[i][j][k]->blockData = &BlockDB::GetInstance().tbl[(int)surfType];
				}
				top -= surfDepth;
				if (surfDepth < 0) break; //safety
			}
		}
	}
	return;
}

void TerrainGeneration::Generate(Chunk* chunk) {
	BiomeMap_t biomeMp;
	FindOrCreateMap({ chunk->basepos.x, chunk->basepos.z }, OUT biomeMp);
	GenerateBiomeFromMap(chunk, biomeMp);
	GenerateRocks(chunk);
	ReplaceSurface(chunk);
}

/// WORLD FUNCTIONS
Chunk* World::findOrCreateChunk(const p3i& chunkIdx) {
	if (allChunks.count(chunkIdx)) return allChunks[chunkIdx];

	//if the chunk doesn't exist, create it!
	Chunk* chunk = new Chunk(
		glm::ivec3(
			Chunk::SZ * std::get<0>(chunkIdx),
			Chunk::HEIGHT * std::get<1>(chunkIdx),
			Chunk::SZ * std::get<2>(chunkIdx)
		)
	);

	//populate the chunk with blocks data
	worldgen.Generate(chunk);

	allChunks[chunkIdx] = chunk;

	return chunk;
}

World::World(glm::vec3 spawnPoint) {
	//initialize worldgen
	worldgen = TerrainGeneration();
	//create initial chunks around spawn point
	centerChunkIdx = Chunk::WorldToChunkIndex(spawnPoint);
}

void World::CreateInitialChunks(glm::vec3 spawnPoint){
	centerChunkIdx = Chunk::WorldToChunkIndex(spawnPoint);
	//generate initial blocks
	for (int i = centerChunkIdx.x - HVIS_WORLD_SZ; i <= centerChunkIdx.x + HVIS_WORLD_SZ; ++i) {
		for (int k = centerChunkIdx.z - HVIS_WORLD_SZ; k <= centerChunkIdx.z + HVIS_WORLD_SZ; ++k) {
			for (int j = -HVIS_WORLD_HEIGHT; j <= HVIS_WORLD_HEIGHT; ++j) {
				Chunk* chunk = visChunks[{i, j, k}] = findOrCreateChunk({ i, j, k });
				if(!chunk->isBuilt) chunk->Build();
			}
		}
	}
}

Chunk* World::CurrentChunk(glm::vec3& position) {
	glm::ivec3 currChunkIdx = Chunk::WorldToChunkIndex(position);
	return visChunks[{currChunkIdx.x, currChunkIdx.y, currChunkIdx.z}];
}

Chunk* World::GetChunkByIndex(const glm::ivec3& idx) {
	if (visChunks.count({ idx.x, idx.y, idx.z })) {
		return visChunks[{idx.x, idx.y, idx.z}];
	}
	return nullptr;
}

Chunk* World::GetChunkContainingBlock(const glm::ivec3& worldpos) {
	int cx = (worldpos.x >= 0 ? (int)(worldpos.x / Chunk::SZ) : (int)((worldpos.x+1) / Chunk::SZ) - 1);
	int cy = (worldpos.y >= 0 ? (int)(worldpos.y / Chunk::HEIGHT) : (int)((worldpos.y+1) / Chunk::HEIGHT) - 1);
	int cz = (worldpos.z >= 0 ? (int)(worldpos.z / Chunk::SZ) : (int)((worldpos.z+1) / Chunk::SZ) - 1);
	
	if (allChunks.count({cx, cy, cz})) return allChunks[{cx, cy, cz}];
	else return nullptr;
}
//renders all visible chunks
void World::Render() {
	for (auto& [cidx, chunk] : visChunks) {
		chunk->Render();
	}
}

void World::Build() {
	//if any visible chunk has modifications,
	//rebuild it.
	for (auto& [cidx, chunk] : visChunks) {
		if(chunk->requiresRebuild) chunk->ReBuild();
	}
}

//updates the map of visible chunks, unloading invisible chunks and building newly visible chunks.
void World::UpdateChunks(glm::vec3& playerPosition) {
	
	glm::ivec3 cijk = Chunk::WorldToChunkIndex(playerPosition);
	int ci = cijk.x, ck = cijk.z;
	if (ci > centerChunkIdx.x + 1 || ci < centerChunkIdx.x - 1
		|| ck > centerChunkIdx.z + 1 || ck < centerChunkIdx.z - 1) {
		centerChunkIdx = cijk;

		//iterate over visible chunks
		std::vector<p3i> to_remove{};
		for (auto& [cidx, chunk] : visChunks) {
			auto [i, j, k] = cidx;
			//if moved out of view
			if (i < ci - HVIS_WORLD_SZ || i > ci + HVIS_WORLD_SZ ||
				k < ck - HVIS_WORLD_SZ || k > ck + HVIS_WORLD_SZ) {
				chunk->DeleteBuffers();
				to_remove.push_back(cidx);
			}
		}

		//iterate over chunks that needs to be rendered, finding them and 
		for (int i = ci - HVIS_WORLD_SZ; i <= ci + HVIS_WORLD_SZ; ++i) {
			for (int k = ck - HVIS_WORLD_SZ; k <= ck + HVIS_WORLD_SZ; ++k) {
				for (int j = -HVIS_WORLD_HEIGHT; j <= HVIS_WORLD_HEIGHT; ++j) {
					if (!visChunks.count({ i, j, k })) {
						Chunk* chunk = visChunks[{i, j, k}] = findOrCreateChunk({ i, j, k });
						//build the chunks if not already built!
						if (!chunk->isBuilt) chunk->Build();
					}
				}
			}
		}

		for (p3i& rmvidx : to_remove) {
			visChunks.erase(rmvidx);
		}
	}
}
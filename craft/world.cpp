#include "world.h"

/* AliceOfSNU 2024 */

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

	biomes[BiomeType::TUNDRA].surfaceBlockTypes = { BlockType::BLOCK_SNOW_SOIL, BlockType::BLOCK_DIRT };
	biomes[BiomeType::TUNDRA].surfaceBlockCnts = { 1, 4 };

	//END DATA
}


Chunk::Chunk() :blockCnt(0), isBuilt(false), requiresRebuild(false), initialized(false){ 
	basepos = ivec3(0, 0, 0); 
	chunkIdx = ivec3(-100'000'000, -100'000'000, -100'000'000);
	solidRenderObj = RenderObject(RenderObject::RenderMode::OPAQUE);
	cutoutRenderObj = RenderObject(RenderObject::RenderMode::CUTOUT);
	waterRenderObj = RenderObject(RenderObject::RenderMode::OPAQUE);
};

Chunk::Chunk(const ivec3& pos, const ivec3& cidx) : blockCnt(0), isBuilt(false), requiresRebuild(false), initialized(false), basepos(pos), chunkIdx(cidx) {
	solidRenderObj = RenderObject(RenderObject::RenderMode::OPAQUE);
	cutoutRenderObj = RenderObject(RenderObject::RenderMode::CUTOUT);
	waterRenderObj = RenderObject(RenderObject::RenderMode::OPAQUE);
};

void Chunk::Build() {

	//Building a chunk twice is an error, because we could be wasting computation.
	//isBuilt flag must be turned off before any rebuild.
	if (isBuilt) return;

	// At this point, we assume all blocks have been put to our grid
	// when more blocks are added, or blocks are deleted from the chunk,
	// the chunk must be rebuilt.

	// get references to adjacent chunks
	Chunk* in_chk = World::GetInstance().GetChunkByIndex(chunkIdx - Chunk::ivec3{ 1, 0, 0 });
	Chunk* ip_chk = World::GetInstance().GetChunkByIndex(chunkIdx + Chunk::ivec3{ 1, 0, 0 });
	Chunk* kn_chk = World::GetInstance().GetChunkByIndex(chunkIdx - Chunk::ivec3{ 0, 0, 1 });
	Chunk* kp_chk = World::GetInstance().GetChunkByIndex(chunkIdx + Chunk::ivec3{ 0, 0, 1 });
	//vtxCnt = 0, idxCnt = 0;
	for (int i = 0; i < SZ; ++i) { //x dir
		for (int j = 0; j < HEIGHT; ++j) { //y dir
			for (int k = 0; k < SZ; ++k) { //z dir
				if (grid[i][j][k] == BlockType::BLOCK_AIR) continue;

				//check if other block exists in that direction
				//if not, add data for that direction's face.
				//not this implementation uses short circuiting in branches
				glm::f32vec3 pos{ basepos.x + i, basepos.y + j, basepos.z + k };
				BlockType blkTy = grid[i][j][k];
				auto& blockData = BlockDB::GetInstance().tbl[blkTy];
				BlockDB& blockDB = BlockDB::GetInstance();
				switch (blockData.renderType) {
				case BlockDB::RenderType::SOLID:
					// place left and right
					if (i == 0 && (!in_chk || !blockDB.isSolidCube(in_chk->grid[SZ-1][j][k])))		solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::LEFT);
					else if(i > 0 && !blockDB.isSolidCube(grid[i - 1][j][k]))						solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::LEFT);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::LEFT);
					if (i == SZ - 1 && (!ip_chk || !blockDB.isSolidCube(ip_chk->grid[0][j][k])))	solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::RIGHT);
					else if(i < SZ - 1 && !blockDB.isSolidCube(grid[i + 1][j][k]))					solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::RIGHT);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::RIGHT);
					// place top and bottom
					if (j == 0 || j > 0 && !BlockDB::GetInstance().isSolidCube(grid[i][j - 1][k]))						solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::BOTTOM);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::BOTTOM);
					if (j == HEIGHT - 1 || j < HEIGHT - 1 && !BlockDB::GetInstance().isSolidCube(grid[i][j + 1][k]))	solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::TOP);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::TOP);
					
					// place back and front
					if (k == 0 && (!kn_chk || !blockDB.isSolidCube(kn_chk->grid[i][j][SZ - 1])))	solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::BACK);
					else if(k > 0 && !BlockDB::GetInstance().isSolidCube(grid[i][j][k - 1]))		solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::BACK);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::BACK);
					if (k == SZ - 1 && (!kp_chk || !blockDB.isSolidCube(kp_chk->grid[i][j][0])))	solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::FRONT);
					else if( k < SZ - 1 && !BlockDB::GetInstance().isSolidCube(grid[i][j][k + 1]))	solidRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::FRONT);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::FRONT);
					break;
				case BlockDB::RenderType::WATER_RENDER:
					// place left and right
					if (i == 0 && (!in_chk || !blockDB.isSolidCube(in_chk->grid[SZ - 1][j][k])))	waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::LEFT);
					else if (i > 0 && !blockDB.isSolidCube(grid[i - 1][j][k]))						waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::LEFT);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::LEFT);
					if (i == SZ - 1 && (!ip_chk || !blockDB.isSolidCube(ip_chk->grid[0][j][k])))	waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::RIGHT);
					else if (i < SZ - 1 && !blockDB.isSolidCube(grid[i + 1][j][k]))					waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::RIGHT);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::RIGHT);
					// place top and bottom
					if (j == 0 || j > 0 && !BlockDB::GetInstance().isSolidCube(grid[i][j - 1][k]))						waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::BOTTOM);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::BOTTOM);
					if (j == HEIGHT - 1 || j < HEIGHT - 1 && !BlockDB::GetInstance().isSolidCube(grid[i][j + 1][k]))	waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::TOP);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::TOP);

					// place back and front
					if (k == 0 && (!kn_chk || !blockDB.isSolidCube(kn_chk->grid[i][j][SZ - 1])))	waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::BACK);
					else if (k > 0 && !BlockDB::GetInstance().isSolidCube(grid[i][j][k - 1]))		waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::BACK);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::BACK);
					if (k == SZ - 1 && (!kp_chk || !blockDB.isSolidCube(kp_chk->grid[i][j][0])))	waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::FRONT);
					else if (k < SZ - 1 && !BlockDB::GetInstance().isSolidCube(grid[i][j][k + 1]))	waterRenderObj.PlaceBlockFaceData(blkTy, pos, Block::Face::FRONT);//idxCnt += block->PlaceFaceData(vtxdata, uvdata, idxdata, INOUT vtxCnt, Block::Face::FRONT);
					break;
				case BlockDB::RenderType::CUTOUT:
					// place all faces, without culling
					for (int f = 0; f < blockData.numFaces(); ++f) {
						cutoutRenderObj.PlaceBlockFaceData(grid[i][j][k], pos, f);
					}
					break;
				}
				
			}
		}
	}

	// transfer data to GL buffers
	solidRenderObj.Build();
	cutoutRenderObj.Build();
	waterRenderObj.Build();
	isBuilt = true;
	requiresRebuild = false;

}

void Chunk::ReBuild() {
	if (!requiresRebuild) return;

	// to rebuild, delete all existing builds first
	solidRenderObj.DeleteBuffers();
	cutoutRenderObj.DeleteBuffers();
	waterRenderObj.DeleteBuffers();
	isBuilt = false;

	Build();
	requiresRebuild = false;
}

//void Chunk::Render() {
//	//assumes shader and the texture is bound and activated.
//	vao.Bind();
//	glDrawElements(GL_TRIANGLES, idxCnt, GL_UNSIGNED_INT, 0);
//}

void Chunk::DestroyBlockAt(const Chunk::ivec3& bidx) {
	// deleting a block makes it air!
	grid[bidx.x][bidx.y][bidx.z] = BlockType::BLOCK_AIR;
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

void Chunk::PlaceBlockAtCompileTime(const ivec3& blockIdx, const BlockDB::BlockType blkTy) {
	glm::ivec3 cidx = chunkIdx;
	glm::ivec3 bidx{ blockIdx.x, blockIdx.y, blockIdx.z };
	while (bidx.x < 0) cidx.x--, bidx.x += Chunk::SZ;
	while (bidx.x >= Chunk::SZ) cidx.x++, bidx.x -= Chunk::SZ;
	while (bidx.y < 0) cidx.y--, bidx.y += Chunk::HEIGHT;
	while (bidx.y >= Chunk::HEIGHT) cidx.y++, bidx.y -= Chunk::HEIGHT;
	while (bidx.z < 0) cidx.z--, bidx.z += Chunk::SZ;
	while (bidx.z >= Chunk::SZ) cidx.z++, bidx.z -= Chunk::SZ;
	if (cidx != chunkIdx) {
		Chunk* ck = World::GetInstance().GetChunkByIndex(cidx);
		if (!ck) return;
		return ck->PlaceBlockAtCompileTime(bidx, blkTy);
	}
	grid[bidx.x][bidx.y][bidx.z] = blkTy;
	requiresRebuild = true;
	return;
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

//double FractalNoise2D::samplePoint(double x, double y) {
//	double amount = 1.0;
//	double result = 0.0;
//	for (double f : octaves) {
//		result += amount * perlin.samplePoint(f * x, f * y);
//		amount *= persistance;
//	}
//	return result;
//}

double FractalNoise2D::samplePoint(double x, double y) {
	double amount = 1.0;
	double result = 0.0;
	for (double f : octaves) {
		result += perlin.samplePoint(f * x, f * y);
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

	slowNoise.octaves.push_back(0.01f);
	slowNoise.persistance = 0.5;
	fastNoise.octaves.push_back(0.03f);
	fastNoise.persistance = 0.5;
}

void TerrainGeneration::GenerateRocks(Chunk* chunk) { //TO BE DEPRECATED
	const int base_terrain_offset = 0;
	const int base_terrain_scale = 40;
	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			//double roughness = 0.5 + roughnessNoise.samplePoint(i + chunk->basepos.x + 0.5, k + chunk->basepos.z + 0.5);
			//int elevation = base_terrain_offset + base_terrain_scale * roughness * heightNoise.samplePoint(i + chunk->basepos.x + 0.5, k + chunk->basepos.z + 0.5);
			int elevation = chunk->blockHeight[i][k];

			// invert the elevation if ocean
			bool isOcean = chunk->blockBiome[i][k] == BiomeType::DEEP_OCEAN || chunk->blockBiome[i][k] == BiomeType::SHALLOW_OCEAN;
			//if (isOcean) {
			//	elevation = std::min(elevation, -elevation);
			//	elevation = std::min(elevation, -1);
			//}
			
			int j = 0; //height in chunk
			for (; j < Chunk::HEIGHT; ++j) {
				if (chunk->basepos.y + j > elevation) break;
				BlockDB::BlockType type = BlockDB::BlockType::BLOCK_GRANITE;
				//if (chunk->basepos.y + j == elevation) type = BlockDB::BlockType::BLOCK_GRASS;
				chunk->grid[i][j][k] = type;
				//block->pos.x = chunk->basepos.x + i;
				//block->pos.z = chunk->basepos.z + k;
				//block->pos.y = chunk->basepos.y + j;
				chunk->blockCnt++;
			}

			int jsurf= j;
			if (isOcean) {
				//fill up to water level = 0
				for (; chunk->basepos.y + j <= 0 && j < Chunk::HEIGHT; ++j) {
					chunk->grid[i][j][k] = BlockDB::BlockType::BLOCK_WATER;
					//Block* block = chunk->grid[i][j][k] = new Block(BlockDB::BlockType::BLOCK_WATER);
					//block->pos.x = chunk->basepos.x + i;
					//block->pos.z = chunk->basepos.z + k;
					//block->pos.y = chunk->basepos.y + j;
					chunk->blockCnt++;
				}
			}
			//chunk->blockHeight[i][k] = std::min(Chunk::HEIGHT, jsurf); //holds ocean floor value for water
		}
	}
}

template<unsigned int SZ>
void ASSERT_VALID_MAP(Map<BiomeData, SZ> mp) {
	for (int i = 0; i <= SZ; ++i) {
		for (int j = 0; j <= SZ; ++j) {
			int val = (int)mp.data[i][j].biomeType;
			if (val < 0 || val >= BiomeType::BIOME_COUNT) {
				throw std::out_of_range("Biome Map data has corrupted value");
			}
		}
	}
}

void TerrainGeneration::GenerateMap(pii basepos, OUT BiomeMap_t& biomeMp, OUT LandscapeMap_t& lscapeMp) {
	// level 8
	Map<float, 1> baseMp({ basepos.first, basepos.second }, MAP_SIZE); //total map size is gonna be 512 * 8 = 4096 * 4096
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

	// level 128
	Map<BiomeData, 128> biomeMp128 = Zoom<BiomeData, 64>::Forward(biomeMp64);
	Map<LandscapeData, 128> landscapeMp128 = GenLandscapeLayer<128>::Forward(biomeMp128);

	//// level 256
	Map<BiomeData, 256> biomeMp256 = Zoom<BiomeData, 128>::Forward(biomeMp128); 
	Map<LandscapeData, 256> landscapeMp256 = NoisyZoom<LandscapeData, 128>::Forward(landscapeMp128);

	//// level 512
	biomeMp = Zoom<BiomeData, 256>::Forward(biomeMp256);
	lscapeMp = NoisyZoom<LandscapeData, 256>::Forward(landscapeMp256);

	ASSERT_VALID_MAP(biomeMp);
	return;
}

void TerrainGeneration::FindOrCreateMap(pii basepos, OUT BiomeMap_t& biomeMp, OUT LandscapeMap_t& lscapeMp) {
	//1. get the map base position
	//base position is in world space
	pii mapbase = { floor(static_cast<float>(basepos.first) / WS_MAP_SPAN) * WS_MAP_SPAN,floor(static_cast<float>(basepos.second) / WS_MAP_SPAN) * WS_MAP_SPAN };
	//mapbase.first -= MAP_SIZE / 2; //so that (0,0) is near the center of the map.
	//mapbase.second -= MAP_SIZE / 2;
	
	//2. check cache
	if (biomeMap.count(mapbase)) {
		biomeMp = biomeMap[mapbase];
		lscapeMp = landscapeMap[mapbase];
		return;
	}

	//3. create map if not exist
	GenerateMap(mapbase, OUT biomeMp, OUT lscapeMp);

	//4. cache the map 
	biomeMap[mapbase] = biomeMp;
	landscapeMap[mapbase] = lscapeMp;
	return;
}

void TerrainGeneration::GenerateBiomeFromMap(Chunk* chunk, const BiomeMap_t biomeMp) {
	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			MapGen::vec2i xz = biomeMp.WorldToMapPoint(chunk->basepos.x + i, chunk->basepos.z + k);
			chunk->blockBiome[i][k] = biomeMp.data[xz.x][xz.y].biomeType;

			if (biomeMp.data[xz.x][xz.y].biomeType < 0 || biomeMp.data[xz.x][xz.y].biomeType >= BiomeType::BIOME_COUNT) {
				throw std::out_of_range("chunk->blockBiome has corrupted values");
			}
		}
	}

	//TODO: VORONOI zoom
	return;
}

void TerrainGeneration::GenerateTerrainHeightsFromMap(Chunk* chunk, const LandscapeMap_t lscapeMp, const BiomeMap_t biomeMp) {

	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			int x = chunk->basepos.x + i, z = chunk->basepos.z + k;

			MapGen::vec2i xzb = biomeMp.WorldToMapPoint(x, z);
			MapGen::vec2f xzls = lscapeMp.WorldToMapPointF(x, z);

			//1. sample from perlin noise and interpolate
			LandscapeData lsdata = lscapeMp.SamplePointSubpixel(xzls.x, xzls.y);
			float alpha = lsdata.roughness;
			int scale = lsdata.maxAbsScale;
			float fn = fastNoise.samplePoint(x, z), sn = slowNoise.samplePoint(x, z);
			float h = alpha * fn + (1.0f - alpha) * sn;

			//2. invert elevation if ocean
			bool isOcean = biomeMp.data[xzb.x][xzb.y].biomeType == BiomeType::SHALLOW_OCEAN || biomeMp.data[xzb.x][xzb.y].biomeType == BiomeType::DEEP_OCEAN;
			if (isOcean)
				chunk->blockHeight[i][k] = std::min(-1, static_cast<int>(scale * (-1.0f + h)));
			else {
				chunk->blockHeight[i][k] = scale * (1.0f + h);
			}
		}
	}

	return;
}

void TerrainGeneration::ReplaceSurface(Chunk* chunk) {
	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			//double roughness = 0.5 + roughnessNoise.samplePoint(i + chunk->basepos.x + 0.5, k + chunk->basepos.z + 0.5);
			//1. get replacement data for biome
			BiomeDB::BiomeDataRow biome = BiomeDB::GetInstance().biomes[chunk->blockBiome[i][k]];
			bool isOcean = chunk->blockBiome[i][k] == BiomeType::DEEP_OCEAN || chunk->blockBiome[i][k] == BiomeType::SHALLOW_OCEAN;
			if (isOcean) continue; //do not replace surface for ocean floors

			int top = chunk->blockHeight[i][k] - chunk->basepos.y;
			for (int b = 0, accDepth = 0; b < biome.surfaceBlockTypes.size(); ++b) {
				//2. replace top rock blocks with predefined surface block types
				BlockDB::BlockType surfType = biome.surfaceBlockTypes[b];
				int surfDepth = biome.surfaceBlockCnts[b];
				if (top < 0) break;
				int j = top - accDepth;
				while (j >= Chunk::HEIGHT) j--;
				accDepth += surfDepth;
				for (; j > top-accDepth && j >= 0 ; --j) {
					chunk->grid[i][j][k] = surfType;
				}
			}

		}
	}
	return;
}


void TerrainGeneration::GenerateBiomass(Chunk& chunk) {
	for (int i = 0; i < Chunk::SZ; ++i) {
		for (int k = 0; k < Chunk::SZ; ++k) {
			//1. get which biome
			BiomeType biome = chunk.blockBiome[i][k];
			int top = chunk.blockHeight[i][k] - chunk.basepos.y;
			
			if (top < 0 || top + 1 >= Chunk::HEIGHT) continue; // skip if out of range
			if (chunk.grid[i][top + 1][k]) continue; // skip if something's already there.

			int bi = chunk.basepos.x + i, bk = chunk.basepos.z + k;
			glm::ivec3 basepos{ i, top + 1, k };
			float r = simpleNoiseFn(bi, bk); // create a flower with probability ~0.05

			switch (biome) {
			case BiomeType::GRASSLAND: //GRASSLAND -> FLOWERS
			case BiomeType::RAINFOREST:
				if (r > 0.97) {
					// generate flowers
					float s = simpleNoiseFn((bi+bk)/20, (bi-bk)/20);
					auto flower = SmallPlants::Make(SmallPlants::RandomPlant(s));
					for (auto& [rpos, blkType] : flower) {
						chunk.PlaceBlockAtCompileTime(basepos + rpos, blkType);
					}
				}
				else if (r > 0.94) {
					// generate trees
					auto tree = Trees::Make(Trees::ELM);
					for (auto& [rpos, blkType] : tree) {
						chunk.PlaceBlockAtCompileTime(basepos + rpos, blkType);
					}
				}
				break;
				
			case BiomeType::SNOWLAND: //SNOWLAND -> SPRUCE
			case BiomeType::TUNDRA: //SNOWLAND -> SPRUCE
				if (r > 0.97) {
					// generate trees
					auto tree = Trees::Make(Trees::BIRCH);
					for (auto& [rpos, blkType] : tree) {
						chunk.PlaceBlockAtCompileTime(basepos + rpos, blkType);
					}
				}
				break;

			}

		}
	}
	return;
}

void TerrainGeneration::Generate(Chunk* chunk) {
	BiomeMap_t biomeMp;
	LandscapeMap_t lscapeMp;
	auto begin = std::chrono::steady_clock::now();
	FindOrCreateMap({ chunk->basepos.x, chunk->basepos.z }, OUT biomeMp, OUT lscapeMp);
	GenerateBiomeFromMap(chunk, biomeMp);
	GenerateTerrainHeightsFromMap(chunk, lscapeMp, biomeMp);
	GenerateRocks(chunk);
	ReplaceSurface(chunk);
	auto end = std::chrono::steady_clock::now();
	auto timeus = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	std::cout << "gen chunk @ " << chunk->basepos.x << "," << chunk->basepos.y << "," << chunk->basepos.z << " time:" << timeus << std::endl;

	//GenerateBiomass(*chunk);
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
		),
		{ std::get<0>(chunkIdx), std::get<1>(chunkIdx), std::get<2>(chunkIdx) }
	);

	//populate the chunk with blocks data
	worldgen.Generate(chunk);

	allChunks[chunkIdx] = chunk;

	return chunk;
}

World::World(glm::vec3 spawnPoint) {
	// initialize worldgen
	worldgen = TerrainGeneration();
	// create initial chunks around spawn point
	centerChunkIdx = Chunk::WorldToChunkIndex(spawnPoint);
}

void World::CreateInitialChunks(glm::vec3 spawnPoint){
	centerChunkIdx = Chunk::WorldToChunkIndex(spawnPoint);
	// generate initial blocks
	for (int i = centerChunkIdx.x - HVIS_WORLD_SZ; i <= centerChunkIdx.x + HVIS_WORLD_SZ; ++i) {
		for (int k = centerChunkIdx.z - HVIS_WORLD_SZ; k <= centerChunkIdx.z + HVIS_WORLD_SZ; ++k) {
			for (int j = -HVIS_WORLD_HEIGHT; j <= HVIS_WORLD_HEIGHT; ++j) {
				Chunk* chunk = visChunks[{i, j, k}] = findOrCreateChunk({ i, j, k });
			}
		}
	}

	// late initialization
	for (int i = centerChunkIdx.x - HVIS_WORLD_SZ; i <= centerChunkIdx.x + HVIS_WORLD_SZ; ++i) {
		for (int k = centerChunkIdx.z - HVIS_WORLD_SZ; k <= centerChunkIdx.z + HVIS_WORLD_SZ; ++k) {
			for (int j = -HVIS_WORLD_HEIGHT; j <= HVIS_WORLD_HEIGHT; ++j) {
				Chunk* chunk = visChunks[{i, j, k}];
				if (!chunk) continue;
				if (!chunk->initialized) {
					worldgen.GenerateBiomass(*chunk);
					std::cout << "biomass " << i << "," << j << "," << k << std::endl;
					chunk->initialized = true;
				}
			}
		}
	}

	for (auto& [cidx, chunk] : visChunks) {
		if (!chunk->isBuilt) chunk->Build();
	}

}

Chunk* World::CurrentChunk(const glm::vec3& position) {
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


void World::Build() {
	//if any visible chunk has modifications,
	//rebuild it.
	for (auto& [cidx, chunk] : visChunks) {
		if (chunk->requiresRebuild) {
			std::cout << "rebuilding " << chunk->basepos.x << "," << chunk->basepos.y << "," << chunk->basepos.z << std::endl;
			chunk->ReBuild();
		}
	}
}

void World::UpdateChunks(glm::vec3& playerPosition) {

	glm::ivec3 cijk = Chunk::WorldToChunkIndex(playerPosition);
	int ci = cijk.x, ck = cijk.z;
	if (ci > centerChunkIdx.x + 1 || ci < centerChunkIdx.x - 1
		|| ck > centerChunkIdx.z + 1 || ck < centerChunkIdx.z - 1) {
		centerChunkIdx = cijk;

		// iterate over visible chunks
		std::vector<p3i> to_remove{};
		for (auto& [cidx, chunk] : visChunks) {
			auto [i, j, k] = cidx;
			// if moved out of view
			if (i < ci - HVIS_WORLD_SZ || i > ci + HVIS_WORLD_SZ ||
				k < ck - HVIS_WORLD_SZ || k > ck + HVIS_WORLD_SZ) {
				// VAO's and VBO's memory can be freed
				chunk->isBuilt = false;
				chunk->solidRenderObj.DeleteBuffers();
				chunk->cutoutRenderObj.DeleteBuffers();

				to_remove.push_back(cidx);
			}
		}

		// iterate over chunks that needs to be rendered, finding them and 
		for (int i = ci - HVIS_WORLD_SZ; i <= ci + HVIS_WORLD_SZ; ++i) {
			for (int k = ck - HVIS_WORLD_SZ; k <= ck + HVIS_WORLD_SZ; ++k) {
				for (int j = -HVIS_WORLD_HEIGHT; j <= HVIS_WORLD_HEIGHT; ++j) {
					if (!visChunks.count({ i, j, k })) {
						Chunk* chunk = visChunks[{i, j, k}] = findOrCreateChunk({ i, j, k });
					}
				}
			}
		}

		for (p3i& rmvidx : to_remove) {
			visChunks.erase(rmvidx);
		}

		// 'late initialization' and build.
		// 'late initialization' involves generation that requires adjacent chunks to be already built.
		for (auto& [cidx, chunk] : visChunks) {
			if (!chunk->initialized) {
				worldgen.GenerateBiomass(*chunk);
				chunk->initialized = true;
			}
		}
		for (auto& [cidx, chunk] : visChunks) {
			if (!chunk->isBuilt)chunk->Build();
		}
	}
}
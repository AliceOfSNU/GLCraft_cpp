#include "world.h"

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
	tbl[BlockType::BLOCK_DIRT] = BlockDataRow{ BlockType::BLOCK_DIRT,		 { BlockTextures::DIRT,			 BlockTextures::DIRT,		 BlockTextures::DIRT,		BlockTextures::DIRT,		BlockTextures::DIRT,		BlockTextures::DIRT} };
	tbl[BlockType::BLOCK_GRASS] = BlockDataRow{ BlockType::BLOCK_GRASS,	 { BlockTextures::GRASS_SIDE,	 BlockTextures::GRASS_SIDE,	 BlockTextures::GRASS_SIDE, BlockTextures::GRASS_SIDE,	BlockTextures::GRASS_TOP,	BlockTextures::DIRT} };

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

Chunk::Chunk() :blockCnt(0), vtxCnt(0), idxCnt(0) { 
	basepos = ivec3(0, 0, 0); 
};
Chunk::Chunk(const ivec3& pos) : blockCnt(0), vtxCnt(0), idxCnt(0), basepos(pos) {};

void Chunk::Build() {
	// At this point, we assume all blocks have been put to our grid
	// when more blocks are added, or blocks are deleted from the chunk,
	// the chunk must be rebuilt.
	
	//create data buffer(vector)
	//building takes a bit of memory but this goes to stack.
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

}

void Chunk::Render() {
	//assumes shader and the texture is bound and activated.
	vao.Bind();
	glDrawElements(GL_TRIANGLES, idxCnt, GL_UNSIGNED_INT, 0);
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

double FractalNoise2D::PerlinNoise2D::samplePoint(double x, double y) {
	//grid coords
	int x0 = (int)x;
	int y0 = (int)y;
	int x1 = x0 + 1, y1 = y0 + 1;
	double sx = x - x0, sy = y - y0;

}
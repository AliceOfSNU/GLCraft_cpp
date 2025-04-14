#include "blocks.hpp"


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

BlockMeshData BlockMeshData::CubeMesh{
	{
		{-0.5,-0.5,0.5, 0.5,-0.5,0.5, 0.5,0.5,0.5, -0.5,0.5,0.5,}, //FRONT(z=+0.5)
		{0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f}, //RIGHT(x=+0.5) 
		{0.5,-0.5,-0.5,-0.5,-0.5,-0.5,-0.5,0.5,-0.5,0.5,0.5,-0.5}, //BACK(z=-0.5)
		{-0.5,-0.5,-0.5,-0.5,-0.5,0.5,-0.5,0.5,0.5,-0.5,0.5,-0.5 }, //LEFT(x=-0.5)
		{-0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,-0.5,-0.5,0.5,-0.5}, //TOP
		{-0.5,-0.5,-0.5,0.5,-0.5,-0.5,0.5,-0.5,0.5,-0.5,-0.5,0.5}, //BOTTOM
	}
};

BlockMeshData BlockMeshData::FlowerMesh{
	{
		{-0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5}, //DIAGONAL 1
		{-0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5}, //DIAGONAL 1
	}
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
													//ID						 /TEXTUURE FRONT				LEFT						 BACK						RIGHT						TOP							BOTTOM						//renderType	  //MeshType		//blocksLight
	tbl[BlockType::BLOCK_DIRT] =      BlockDataRow{ BlockType::BLOCK_DIRT,		 { BlockTextures::DIRT,			BlockTextures::DIRT,		 BlockTextures::DIRT,		BlockTextures::DIRT,		BlockTextures::DIRT,		BlockTextures::DIRT} ,	   RenderType::SOLID, MeshType::CUBE, 	true};
	tbl[BlockType::BLOCK_GRASS] =	  BlockDataRow{ BlockType::BLOCK_GRASS,		 { BlockTextures::GRASS_SIDE,	BlockTextures::GRASS_SIDE,	 BlockTextures::GRASS_SIDE, BlockTextures::GRASS_SIDE,	BlockTextures::GRASS_TOP,	BlockTextures::DIRT} ,     RenderType::SOLID, MeshType::CUBE, 	true};
	tbl[BlockType::BLOCK_SAND] = 	  BlockDataRow{ BlockType::BLOCK_SAND,		 { BlockTextures::SAND,			BlockTextures::SAND,		 BlockTextures::SAND,		BlockTextures::SAND,		BlockTextures::SAND,		BlockTextures::SAND} ,	   RenderType::SOLID, MeshType::CUBE, 	true};
	tbl[BlockType::BLOCK_GRANITE] =	  BlockDataRow{ BlockType::BLOCK_GRANITE,	 { BlockTextures::GRANITE,		BlockTextures::GRANITE,		 BlockTextures::GRANITE,	BlockTextures::GRANITE,		BlockTextures::GRANITE,		BlockTextures::GRANITE},   RenderType::SOLID, MeshType::CUBE, 	true };
	tbl[BlockType::BLOCK_SNOW_SOIL] = BlockDataRow{ BlockType::BLOCK_SNOW_SOIL,	 { BlockTextures::SNOW_SIDE,	BlockTextures::SNOW_SIDE,	 BlockTextures::SNOW_SIDE,	BlockTextures::SNOW_SIDE,	BlockTextures::SNOW,		BlockTextures::DIRT},	   RenderType::SOLID, MeshType::CUBE, 	true };
	tbl[BlockType::BLOCK_WATER] =	  BlockDataRow{ BlockType::BLOCK_WATER,		 { BlockTextures::WATER,		BlockTextures::WATER,		 BlockTextures::WATER,		BlockTextures::WATER,		BlockTextures::WATER,		BlockTextures::WATER},	   RenderType::WATER_RENDER, MeshType::CUBE, true};
	
	// TREES
	tbl[BlockType::BLOCK_BIRCH_LOG] = BlockDataRow{ BlockType::BLOCK_BIRCH_LOG,	 { BlockTextures::BIRCH_SIDE,	BlockTextures::BIRCH_SIDE,	 BlockTextures::BIRCH_SIDE,	BlockTextures::BIRCH_SIDE,	BlockTextures::BIRCH_TOP,	BlockTextures::BIRCH_TOP}, RenderType::SOLID,  MeshType::CUBE, true };
	tbl[BlockType::BLOCK_ELM_LOG] =   BlockDataRow{ BlockType::BLOCK_ELM_LOG,	 { BlockTextures::ELM_SIDE,		BlockTextures::ELM_SIDE,	 BlockTextures::ELM_SIDE,	BlockTextures::ELM_SIDE,	BlockTextures::ELM_TOP,		BlockTextures::ELM_TOP},   RenderType::SOLID,  MeshType::CUBE, true };
	tbl[BlockType::BLOCK_FOILAGE] =	  BlockDataRow{ BlockType::BLOCK_FOILAGE,	 { BlockTextures::FOILAGE,		BlockTextures::FOILAGE,		 BlockTextures::FOILAGE,	BlockTextures::FOILAGE,		BlockTextures::FOILAGE,		BlockTextures::FOILAGE},   RenderType::CUTOUT, MeshType::CUBE, true };
	
	// FLOWERS
	tbl[BlockType::BLOCK_POPPY] =	  BlockDataRow{ BlockType::BLOCK_POPPY, 	 { BlockTextures::POPPY,		BlockTextures::POPPY,		},	RenderType::CUTOUT, MeshType::FLOWER, false };
	tbl[BlockType::BLOCK_DANDELION] = BlockDataRow{ BlockType::BLOCK_DANDELION,	 { BlockTextures::DANDELION,	BlockTextures::DANDELION,   },	RenderType::CUTOUT, MeshType::FLOWER, false };
	tbl[BlockType::BLOCK_CYAN_FLOWER]=BlockDataRow{ BlockType::BLOCK_CYAN_FLOWER,{ BlockTextures::CYAN_FLOWER,	BlockTextures::CYAN_FLOWER, },	RenderType::CUTOUT, MeshType::FLOWER, false };
	tbl[BlockType::BLOCK_AIR] =		  BlockDataRow{ BlockType::BLOCK_AIR,		 { BlockTextures::NONE,			BlockTextures::NONE,		 BlockTextures::NONE,		BlockTextures::NONE,		BlockTextures::NONE,		BlockTextures::NONE},	   RenderType::INVISIBLE, MeshType::CUBE, false };

	// STAIRS
	tbl[BlockType::BLOCK_WOODEN_STAIR_P0] = BlockDataRow{ BlockType::BLOCK_WOODEN_STAIR_P0, { BlockTextures::WOOD,	BlockTextures::NONE, BlockTextures::NONE, BlockTextures::NONE,	BlockTextures::NONE, BlockTextures::NONE},	   RenderType::SHAPE_SOLID, MeshType::SHAPED, true };
	tbl[BlockType::BLOCK_WOODEN_STAIR_P90] = BlockDataRow{ BlockType::BLOCK_WOODEN_STAIR_P90, { BlockTextures::WOOD,	BlockTextures::NONE, BlockTextures::NONE, BlockTextures::NONE,	BlockTextures::NONE, BlockTextures::NONE},	   RenderType::SHAPE_SOLID, MeshType::SHAPED, true };
	tbl[BlockType::BLOCK_WOODEN_STAIR_P180] = BlockDataRow{ BlockType::BLOCK_WOODEN_STAIR_P180, { BlockTextures::WOOD,	BlockTextures::NONE, BlockTextures::NONE, BlockTextures::NONE,	BlockTextures::NONE, BlockTextures::NONE},	   RenderType::SHAPE_SOLID, MeshType::SHAPED, true };
	tbl[BlockType::BLOCK_WOODEN_STAIR_P270] = BlockDataRow{ BlockType::BLOCK_WOODEN_STAIR_P270, { BlockTextures::WOOD,	BlockTextures::NONE, BlockTextures::NONE, BlockTextures::NONE,	BlockTextures::NONE, BlockTextures::NONE},	   RenderType::SHAPE_SOLID, MeshType::SHAPED, true };
	
	tbl[BlockType::BLOCK_COBBLESTONE_STAIR_P0] = 	BlockDataRow{ BlockType::BLOCK_COBBLESTONE_STAIR_P0, { BlockTextures::COBBLESTONE,	BlockTextures::NONE, BlockTextures::NONE, BlockTextures::NONE,	BlockTextures::NONE, BlockTextures::NONE},	   RenderType::SHAPE_SOLID, MeshType::SHAPED, true };
	tbl[BlockType::BLOCK_COBBLESTONE_STAIR_P90] = 	BlockDataRow{ BlockType::BLOCK_COBBLESTONE_STAIR_P90, { BlockTextures::COBBLESTONE,	BlockTextures::NONE, BlockTextures::NONE, BlockTextures::NONE,	BlockTextures::NONE, BlockTextures::NONE},	   RenderType::SHAPE_SOLID, MeshType::SHAPED, true };
	tbl[BlockType::BLOCK_COBBLESTONE_STAIR_P180] = 	BlockDataRow{ BlockType::BLOCK_COBBLESTONE_STAIR_P180, { BlockTextures::COBBLESTONE,	BlockTextures::NONE, BlockTextures::NONE, BlockTextures::NONE,	BlockTextures::NONE, BlockTextures::NONE},	   RenderType::SHAPE_SOLID, MeshType::SHAPED, true };
	tbl[BlockType::BLOCK_COBBLESTONE_STAIR_P270] = 	BlockDataRow{ BlockType::BLOCK_COBBLESTONE_STAIR_P270, { BlockTextures::COBBLESTONE,	BlockTextures::NONE, BlockTextures::NONE, BlockTextures::NONE,	BlockTextures::NONE, BlockTextures::NONE},	   RenderType::SHAPE_SOLID, MeshType::SHAPED, true };
	

}

bool BlockDB::isSolidCube(BlockType blkTy) {
	auto& blockData = tbl[blkTy];
	if (blockData.meshType == MeshType::CUBE && (blockData.renderType == RenderType::SOLID || blockData.renderType == RenderType::WATER_RENDER)) return true;

	return false;
}

bool BlockDB::isTransparentBlock(BlockType blkTy) {
	auto& blockData = tbl[blkTy];
	return !blockData.blocksLight;
}

BlockMeshData& BlockDB::GetMeshData(MeshType ty) {
	switch (ty) {
	case MeshType::CUBE:
		return BlockMeshData::CubeMesh;

	case MeshType::FLOWER:
		return BlockMeshData::FlowerMesh;
	}

	return BlockMeshData::CubeMesh;
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

GLuint Block::PlaceModelData(
	BlockDB::BlockType blkTy, glm::f32vec3 offset,
	vf& vtxit, vf& uvit, vi& idxit, INOUT size_t& vtxcnt, INOUT size_t& idxcnt){
	switch (blkTy){
		case BlockDB::BlockType::BLOCK_WOODEN_STAIR_P0:
		case BlockDB::BlockType::BLOCK_WOODEN_STAIR_P90:
		case BlockDB::BlockType::BLOCK_WOODEN_STAIR_P180:
		case BlockDB::BlockType::BLOCK_WOODEN_STAIR_P270:
			return PlaceStairModelData(blkTy, offset, vtxit, uvit, idxit, vtxcnt, idxcnt);
		default:
			return 0;
	}
}

GLuint Block::PlaceStairModelData(
	BlockDB::BlockType blkTy, glm::f32vec3 offset,
	vf& vtxit, vf& uvit, vi& idxit, INOUT size_t& vtxcnt, INOUT size_t& idxcnt)
{
	//1. place vertex data
	// static variables in the same translation unit is initialized in declaration order.
	static std::vector<glm::vec3> StairMesh_P0 {
		{-0.5f, -0.5f, 0.5f},
		{0.5f, -0.5f, 0.5f},
		{0.5f, 0.0f, 0.5f},
		{0.0f, 0.0f, 0.5f},
		{0.0f, 0.5f, 0.5f},
		{-0.5f, 0.5f, 0.5f},
		{-0.5f, -0.5f, -0.5f},
		{0.5f, -0.5f, -0.5f},
		{0.5f, 0.0f, -0.5f},
		{0.0f, 0.0f, -0.5f},
		{0.0f, 0.5f, -0.5f},
		{-0.5f, 0.5f, -0.5f},

		{-0.5f, 0.5f, 0.5f},//12
		{0.0f, 0.5f, 0.5f},//13
		{0.0f, 0.0f, 0.5f},//14
		{-0.5f, 0.5f, -0.5f},//15
		{0.0f, 0.5f, -0.5f},//16
		{0.0f, 0.0f, -0.5f},//17
		{0.0f, 0.0f, 0.5f},//18
		{0.5f, 0.0f, 0.5f},//19
		{0.5f, -0.5f, 0.5f},//20
		{0.0f, 0.0f, -0.5f},//21
		{0.5f, 0.0f, -0.5f},//22
		{0.5f, -0.5f, -0.5f},//23

		{-0.5f, 0.5f, 0.5f},//24
		{-0.5f, -0.5f, 0.5f},//25
		{-0.5f, 0.5f, -0.5f},//26
		{-0.5f, -0.5f, -0.5f},//27
		{-0.5f, -0.5f, 0.5f,},//28
		{0.5f, -0.5f, 0.5f,},//29
		{-0.5f, -0.5f, -0.5f,},//30
		{0.5f, -0.5f, -0.5f,},//31

	};

	auto rotate = [](const std::vector<glm::vec3>& mesh, const glm::mat4x4& rotator) -> std::vector<glm::vec3>{
		std::vector<glm::vec3> mesh_rotated;
		std::transform(mesh.begin(), mesh.end(), std::back_inserter(mesh_rotated), 
			[&rotator](glm::vec3 vec){
				return glm::vec3(rotator * glm::vec4(vec, 1.0f));
			});
		return mesh_rotated;
	};

	static std::vector<glm::vec3> StairMesh_P90 = rotate(
		StairMesh_P0, 
		glm::rotate(
			glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)
		)
	);
	static std::vector<glm::vec3> StairMesh_P180 = rotate(
		StairMesh_P0, 
		glm::rotate(
			glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)
		)
	);
	static std::vector<glm::vec3> StairMesh_P270 = rotate(
		StairMesh_P0, 
		glm::rotate(
			glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)
		)
	);

	switch(blkTy){
		case BlockDB::BlockType::BLOCK_WOODEN_STAIR_P0:
			for(int i = 0; i < StairMesh_P0.size(); ++i){
				vtxit.push_back(StairMesh_P0[i].x+offset.x);
				vtxit.push_back(StairMesh_P0[i].y+offset.y);
				vtxit.push_back(StairMesh_P0[i].z+offset.z);
			}
			break;
		
		case BlockDB::BlockType::BLOCK_WOODEN_STAIR_P90:
			for(int i = 0; i < StairMesh_P90.size(); ++i){
				vtxit.push_back(StairMesh_P90[i].x+offset.x);
				vtxit.push_back(StairMesh_P90[i].y+offset.y);
				vtxit.push_back(StairMesh_P90[i].z+offset.z);
			}
			break;

		case BlockDB::BlockType::BLOCK_WOODEN_STAIR_P180:
			for(int i = 0; i < StairMesh_P180.size(); ++i){
				vtxit.push_back(StairMesh_P180[i].x+offset.x);
				vtxit.push_back(StairMesh_P180[i].y+offset.y);
				vtxit.push_back(StairMesh_P180[i].z+offset.z);
			}
			break;

		case BlockDB::BlockType::BLOCK_WOODEN_STAIR_P270:
			for(int i = 0; i < StairMesh_P270.size(); ++i){
				vtxit.push_back(StairMesh_P270[i].x+offset.x);
				vtxit.push_back(StairMesh_P270[i].y+offset.y);
				vtxit.push_back(StairMesh_P270[i].z+offset.z);
			}
			break;
	}


	//2. place uv data
	const static float uvs[32][2]{
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 0.5f},
		{0.5f, 0.5f},
		{0.5f, 1.0f},
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 0.5f},
		{0.5f, 0.5f},
		{0.5f, 1.0f},
		{0.0f, 1.0f},
		{0.0f, 1.0f},
		{0.0f, 0.5f},
		{0.0f, 0.0f},
		{1.0f, 1.0f},
		{1.0f, 0.5f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{0.0f, 0.5f},
		{0.0f, 0.0f},
		{1.0f, 1.0f},
		{1.0f, 0.5f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 1.0f},
		{1.0f, 0.0f},
		{0.0f, 1.0f},
		{0.0f, 0.0f},
	};

	BlockDB::BlockDataRow& row = BlockDB::GetInstance().tbl[blkTy];
	for(int v = 0; v < 32; ++v){
		uvit.push_back(uvs[v][0]);
		uvit.push_back(uvs[v][1]);
		uvit.push_back((float)row.faceTextures[0]);
	}

	//3. place index data
	const static std::vector<GLuint> order{
		0, 3, 1, 1, 2, 3, 0, 3, 5, 3, 4, 5,
		6, 9, 7, 7, 8, 9, 6, 9, 11, 9, 10, 11,
		12, 13, 15, 13, 16, 15, 13, 14, 16, 14, 17, 16,
		18, 19, 21, 19, 22, 21, 19, 20, 22, 20, 23, 22,
		24, 25, 27, 24, 27, 26, 28, 29, 30, 29, 31, 30,
	};

	for(GLuint x: order){
		idxit.push_back(vtxcnt + x);
	}

	vtxcnt += 32;
	idxcnt += order.size();
	return 32;
}
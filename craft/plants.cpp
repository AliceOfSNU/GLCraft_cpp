#include "plants.hpp"


// FLOWERS
std::vector< std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>> SmallPlants::tbl = {

	// DATA: number of rows in this table must match the number of smallplant types

	// POPPY
	{
		{{0, 0, 0}, BlockDB::BLOCK_POPPY},
	},
	// DANDELION
	{
		{{0, 0, 0}, BlockDB::BLOCK_DANDELION},
	},
	// CYAN FLOWER
	{
		{{0, 0, 0}, BlockDB::BLOCK_CYAN_FLOWER},
	},
};

std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>& SmallPlants::Make(PlantType ty) {
	if (ty >= tbl.size()) throw std::out_of_range("oops! check the number of flowers in database and the requested flower type!");
	return tbl[(size_t)ty];
}


// TREES
std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>& Trees::Make(TreeType ty) {
	static int counter = 0;
	counter = (counter + 1) % 2;
	if (ty >= protoTypes.size()) throw std::out_of_range("oops! check the number of trees in database and the requested tree type!");
	return protoTypes[ty][counter];
}

std::vector<std::vector<std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>>>
Trees::initializePrototypes() {
	
	std::vector<std::vector<std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>>> protoTypes(TreeType::NUM_TREES);
	std::vector<std::vector<std::vector<char>>> canopyPrototypes;

	protoTypes[ELM].resize(2, std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>());
	protoTypes[BIRCH].resize(2, std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>());

	// canopy prototype is a human-readable blueprint of how leaf blocks are arranged
	// 'O' means there's a leaf block there, 'X' means nothing there.
	// 0th entry corresponds to the topmost layer.
	canopyPrototypes = {
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'O', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'O', 'O', 'O', 'X'},
		{'X', 'O', 'X', 'O', 'X'},
		{'X', 'O', 'O', 'O', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		{{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'}},

		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}}
	};


	// prototype 1
	for (int y = 6, i = 0; i < 4; ++i, --y) {
		for (int x = -2; x <= 2; ++x) {
			for (int z = -2; z <= 2; ++z) {
				if (canopyPrototypes[i][x + 2][z + 2] == 'O') {
					protoTypes[ELM][0].push_back({ {x, y, z}, BlockDB::BlockType::BLOCK_FOILAGE });
				}
			}
		}
	}
	for (int y = 0; y <= 5; ++y) {
		protoTypes[ELM][0].push_back({ {0, y, 0}, BlockDB::BlockType::BLOCK_ELM_LOG });
	}

	// prototype 2
	canopyPrototypes = {

		// 7
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 6
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'X', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 5
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}},

		// 4
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}},

		// 3
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'X', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

	};

	for (int y = 7, i = 0; i < 5; ++i, --y) {
		for (int x = -2; x <= 2; ++x) {
			for (int z = -2; z <= 2; ++z) {
				if (canopyPrototypes[i][x + 2][z + 2] == 'O') {
					protoTypes[ELM][1].push_back({ {x, y, z}, BlockDB::BlockType::BLOCK_FOILAGE });
				}
			}
		}
	}
	for (int y = 0; y <= 6; ++y) {
		protoTypes[ELM][1].push_back({ {0, y, 0}, BlockDB::BlockType::BLOCK_ELM_LOG });
	}



	// SPRUCE
	canopyPrototypes = {
		// 11
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'O', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 10
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 9
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'O', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 8
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}},

		// 7
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'X', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 6
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}},

		// 5
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'X', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 4
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}},

		// 3
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'X', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 2
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}}
	};

	for (int y = 11, i = 0; i < 10; ++i, --y) {
		for (int x = -2; x <= 2; ++x) {
			for (int z = -2; z <= 2; ++z) {
				if (canopyPrototypes[i][x + 2][z + 2] == 'O') {
					protoTypes[BIRCH][0].push_back({ {x, y, z}, BlockDB::BlockType::BLOCK_FOILAGE });
				}
			}
		}
	}
	for (int y = 0; y <= 8; ++y) {
		protoTypes[BIRCH][0].push_back({ {0, y, 0}, BlockDB::BlockType::BLOCK_BIRCH_LOG });
	}

	// smaller prototype
	canopyPrototypes = {
		// 10
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 9
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'O', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 8
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 7
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'O', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 6
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}},

		// 5
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'X', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 4
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}},

		// 3
		{{'X', 'X', 'X', 'X', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'O', 'X', 'O', 'X'},
		{'X', 'X', 'O', 'X', 'X'},
		{'X', 'X', 'X', 'X', 'X'}},

		// 2
		{{'X', 'O', 'O', 'O', 'X'},
		{'O', 'O', 'O', 'O', 'O'},
		{'O', 'O', 'X', 'O', 'O'},
		{'O', 'O', 'O', 'O', 'O'},
		{'X', 'O', 'O', 'O', 'X'}}
	};

	for (int y = 10, i = 0; i < 9; ++i, --y) {
		for (int x = -2; x <= 2; ++x) {
			for (int z = -2; z <= 2; ++z) {
				if (canopyPrototypes[i][x + 2][z + 2] == 'O') {
					protoTypes[BIRCH][1].push_back({ {x, y, z}, BlockDB::BlockType::BLOCK_FOILAGE });
				}
			}
		}
	}
	for (int y = 0; y <= 6; ++y) {
		protoTypes[BIRCH][1].push_back({ {0, y, 0}, BlockDB::BlockType::BLOCK_BIRCH_LOG });
	}

	return protoTypes; // NRVO
}

std::vector<std::vector<std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>>> Trees::protoTypes = Trees::initializePrototypes();

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <algorithm>

#include "blocks.hpp"

class SmallPlants {
public:
	enum PlantType {
		POPPY, DANDELION, CYAN_FLOWER, NUM_SMALLPLANTS
	};

	static std::vector< std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>> tbl;

	static std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>& Make(PlantType ty);
	static PlantType RandomPlant(float r) {
		return (PlantType)(r * PlantType::NUM_SMALLPLANTS);
	}

};

class Trees {
public:
	enum TreeType {
		ELM, BIRCH, NUM_TREES
	};

	struct TreeInfo {
		int treeHeight;
		int trunkHeight;
	};

	static std::vector<TreeInfo> tbl;
	static std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>& Make(TreeType ty);

	static std::vector<std::vector<std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>>> protoTypes;
	static std::vector<std::vector<std::vector<std::pair<glm::ivec3, BlockDB::BlockType>>>> initializePrototypes();
};
#pragma once
#ifndef MAP_H
#define MAP_H
	
//each chunk should have a map
//map specifies the biome type
//this class only contains data. 
// filling in the data is the job of 'MapGen' class

#include <vector>
#include <iostream>

namespace MapGen {

	struct vec2i { int x, y;   };
	struct vec2f { float x, y; };

	template<class MapDataTy, unsigned int SZ>
	class Map {
	public:

		//the constructor allocates memory
		Map(const vec2i& basepos, const int scale);
		Map();
		~Map();
		

		//actually, a map of size SZ with hold (1+SZ) rows and (1+SZ) cols of data
		//the last row/col is a padding for computing values along the edges.
		MapDataTy** data; 
		vec2i basepos;
		int scale;

		int size() { return SZ; }

		vec2i MapToWorldPoint(int i, int j);
	};

	//the simplest data would just tell apart oceans from land.
	struct OceanMapData {
		bool isLand = false;

		static OceanMapData mix(const OceanMapData& a, const OceanMapData& b, float r) {
			if (r < 0.5) return OceanMapData{ a.isLand };
			return OceanMapData{ b.isLand };
		}

		static OceanMapData mix(const OceanMapData& a, const OceanMapData& b, const OceanMapData& c, const OceanMapData& d, float r) {
			if (r < 0.25) return OceanMapData{ a.isLand };
			else if (r < 0.5) return OceanMapData{ b.isLand };
			else if (r < 0.75) return OceanMapData{ c.isLand };
			else return OceanMapData{ d.isLand };
		}
	};

	struct PreClimateData {
		int tempLevel;
		int prcpLevel;
	
	};

	
	
	enum BiomeType {
		DESERT, RAINFOREST, SHRUBLAND, GRASSLAND, TUNDRA, SNOWLAND, SHALLOW_OCEAN, DEEP_OCEAN, NONE
	};

	struct BiomeData {
		BiomeType biomeType = NONE;
		//int avgTemp;
		//int avgPrcp;

		static BiomeData mix(const BiomeData& a, const BiomeData& b, float r) {
			BiomeData data;
			if (r < 0.5) data.biomeType = a.biomeType;
			else data.biomeType = b.biomeType;
			return data;
		}

		static BiomeData mix(const BiomeData& a, const BiomeData& b, const BiomeData& c, const BiomeData& d, float r) {
			BiomeData data;
			if (r < 0.25) data.biomeType = a.biomeType;
			else if (r < 0.5) data.biomeType = b.biomeType;
			else if (r < 0.75) data.biomeType = c.biomeType;
			else data.biomeType = d.biomeType;

			return data;//NRVO
		}

	};

	struct LandscapeData {
		int avgAlt;
		int varAlt;

	};


}

#endif
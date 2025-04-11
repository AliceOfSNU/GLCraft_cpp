#pragma once
#ifndef MAP_H
#define MAP_H
	
//each chunk should have a map
//map specifies the biome type
//this class only contains data. 
// filling in the data is the job of 'MapGen' class

#include <vector>
#include <iostream>
#include <map>

namespace MapGen {

	struct vec2i { int x, y;   };
	struct vec2f { float x, y; };

	template<class MapDataTy, unsigned int SZ>
	class Map {
	public:

		Map(const vec2i& basepos, const int scale);
		Map();
		//~Map();

		// actually, a map of size SZ with hold (2+SZ) rows and (2+SZ) cols of data
		// the first/last row/col is a padding for computing values along the edges.
		int pad = 2;
		MapDataTy** data;
		vec2i basepos;

		// scale is the number of blocks that corresponds to one pixel of the map
		int scale;

		int size() { return SZ+2*pad; }

		vec2i MapToWorldPoint(int i, int j)const;
		vec2i WorldToMapPoint(int i, int j)const;
		vec2f WorldToMapPointF(int i, int j)const;
		MapDataTy SamplePointSubpixel(double i, double j)const;
	};

	// the simplest data would just tell apart oceans from land.
	struct OceanMapData {
		bool isLand = false;

		static OceanMapData mix(const OceanMapData& a, const OceanMapData& b, float r) {
			if (r < 0.5) return OceanMapData{ a.isLand };
			return OceanMapData{ b.isLand };
		}

		static OceanMapData mix(const OceanMapData& a, const OceanMapData& b, const OceanMapData& c, float r) {
			if (r < 0.33) return OceanMapData{ a.isLand };
			else if (r < 0.67) return OceanMapData{ b.isLand };
			return OceanMapData{ c.isLand };
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
		DESERT, RAINFOREST, SHRUBLAND, GRASSLAND, TUNDRA, SNOWLAND, SHALLOW_OCEAN, DEEP_OCEAN, NONE, BIOME_COUNT
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

		static BiomeData mix(const BiomeData& a, const BiomeData& b, const BiomeData& c, float r) {
			BiomeData data;
			if (r < 0.33) data.biomeType = a.biomeType;
			else if (r < 0.67) data.biomeType = b.biomeType;
			else data.biomeType = c.biomeType;
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
		int maxAbsScale = 0;
		float roughness = 0.0f;

		static LandscapeData mix(const LandscapeData& a, const LandscapeData& b, float r) {
			LandscapeData data;
			data.maxAbsScale = (a.maxAbsScale + b.maxAbsScale) / 2.0f + (4*r-2);
			data.roughness = (a.roughness + b.roughness) / 2.0f;

			return data;
		}

		static LandscapeData mix(const LandscapeData& a, const LandscapeData& b, const LandscapeData& c, float r) {
			LandscapeData data;
			data.maxAbsScale = (a.maxAbsScale + b.maxAbsScale + c.maxAbsScale) / 3.0f + (4 * r - 2);
			data.roughness = (a.roughness + b.roughness + c.roughness) / 3.0f;

			return data;
		}

		static LandscapeData mix(const LandscapeData& a, const LandscapeData& b, const LandscapeData& c, const LandscapeData& d, float r) {
			LandscapeData data;
			data.maxAbsScale = (a.maxAbsScale + b.maxAbsScale + c.maxAbsScale + d.maxAbsScale) / 4.0f;
			data.roughness = (a.roughness + b.roughness + c.roughness + d.roughness) / 4.0f;
			return data;
		}
	};

	//specialized member functions
	template<>
	inline LandscapeData Map<LandscapeData, 512>::SamplePointSubpixel(double x, double z) const;

	//template definitions
	//map ctors and dtors
	template<typename MapDataTy, unsigned int SZ>
	Map<MapDataTy, SZ>::Map() : basepos({ 0, 0 }), scale(1), pad(2) {
		data = new MapDataTy * [SZ + 2 * pad];
		for (int i = 0; i < size(); ++i) {
			data[i] = new MapDataTy[SZ + 2 * pad];
		}
	}

	template<typename MapDataTy, unsigned int SZ>
	Map<MapDataTy, SZ>::Map(const vec2i& pos, const int sc) : basepos(pos), scale(sc), pad(2) {
		data = new MapDataTy * [SZ + 2 * pad];
		for (int i = 0; i <= size(); ++i) {
			data[i] = new MapDataTy[SZ + 2 * pad];
		}
	}
	
	// utility function to convert the map data array index to actual world coordinate
	// all computation is integer.
	template<typename MapDataTy, unsigned int SZ>
	vec2i Map<MapDataTy, SZ>::MapToWorldPoint(int i, int j) const {
		return { basepos.x + (i-pad) * scale, basepos.y + (j-pad) * scale };
	}

	template<typename MapDataTy, unsigned int SZ>
	vec2i Map<MapDataTy, SZ>::WorldToMapPoint(int i, int j) const {
		return { (i - basepos.x) / scale + pad, (j - basepos.y) / scale + pad };
	}

	template<typename MapDataTy, unsigned int SZ>
	vec2f Map<MapDataTy, SZ>::WorldToMapPointF(int i, int j) const {
		return { (i - basepos.x) / static_cast<float>(scale) + pad, (j - basepos.y) / static_cast<float>(scale) + pad};
	}

	template<typename MapDataTy, unsigned int SZ>
	MapDataTy Map<MapDataTy, SZ>::SamplePointSubpixel(double x, double z) const {
		int x0 = static_cast<int>(x), z0 = static_cast<int>(z);
		double r = ((x - x0) + (z - z0)) / 2;
		MapDataTy::mix(data[x0][z0], data[1 + x0][z0], data[x0][1 + z0], data[1 + x0][1 + z0], r);
	}

	// specialization for landscape map at max resolution.
	// at other resolutions, subsampling should equal in behaviour to zoom layer's impl(mixing)
	// at maximum resolution, we use bilinear sampling
	template<>
	LandscapeData Map<LandscapeData, 512>::SamplePointSubpixel(double x, double z) const {
		int x0 = static_cast<int>(x), z0 = static_cast<int>(z), x1 = x0+1, z1 = z0 + 1;
		LandscapeData d;
		const LandscapeData& d00 = data[x0][z0], d10 = data[x1][z0], d01 = data[x0][z1], d11 = data[x1][z1];
		// lerp abs scale
		double mxs0 = (z1 - z) * d00.maxAbsScale + (z - z0) * d01.maxAbsScale;
		double mxs1 = (z1 - z) * d10.maxAbsScale + (z - z0) * d11.maxAbsScale;
		double mxs = (x1 - x) * mxs0 + (x - x0) * mxs1;
		d.maxAbsScale = mxs;
		
		// lerp roughness
		double r0 = (z1 - z) * d00.roughness + (z - z0) * d01.roughness;
		double r1 = (z1 - z) * d10.roughness + (z - z0) * d11.roughness;
		d.roughness = (x1 - x) * r0 + (x - x0) * r1;

 		return d;
	}
}

#endif
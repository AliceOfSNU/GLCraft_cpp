// generation.cpp : Defines the functions for the static library.
//

//#include "pch.h"
//#include "framework.h"
#include "pch.h"
#include "layers.h"

namespace MapGen {

	static float simpleNoiseFn(int ix, int iy) {
		const unsigned w = 8 * sizeof(unsigned);
		const unsigned s = w / 2;
		unsigned a = ix, b = iy;
		a *= 3284157443;
		b ^= a << s | a >> w - s;
		b *= 1911520717;
		a ^= b << s | b >> w - s;
		a *= 2048419325;
		float random = a * (3.14159265 / ~(~0u >> 1));

		return 0.5f + 0.5f * sin(random);
	}

	template<unsigned int SZ>
	Map<float, SZ> WhiteNoise<SZ>::Forward(const Map<float, 1>& input) {
		Map<float, SZ> mp(input.basepos, input.scale);
		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) {
				mp.data[i][j] = simpleNoiseFn(input.basepos.x + input.scale * i, input.basepos.y + input.scale * j);
			}
		}

		//move constructor
		return mp;
	}


	template<unsigned int SZ>
	Map<OceanMapData, SZ> GenIslandLayer<SZ>::Forward(Map<float, SZ>& input) {
		Map<OceanMapData, SZ> mp(input.basepos, input.scale);
		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) {
				mp.data[i][j].isLand = (input.data[i][j] > 0.4f);
			}
		}
		//move constructor
		return mp;
	}


	template<class Ty, unsigned int SZ>
	Map<Ty, SZ * 2> Zoom<Ty, SZ>::Forward(Map<Ty, SZ>& input) {

		//scale halves!
		Map<Ty, SZ * 2> mp(input.basepos, input.scale / 2);

		//boundary mixing
		for (int j = 0; j < SZ; ++j) {
			for (auto i : { 0U, SZ }) {
				mp.data[i * 2][2 * j] = input.data[i][j];
				//mixing is deterministic.
				float r = simpleNoiseFn(mp.basepos.x + mp.scale * i * 2, mp.basepos.y + mp.scale * (2 * j + 1));
				mp.data[i * 2][2 * j + 1] = Ty::mix(input.data[i][j], input.data[i][j + 1], r);

				if (i == 0U) {
					mp.data[2 * i + 1][2 * j] = Ty::mix(input.data[i][j], input.data[i + 1][j], r);
					mp.data[2 * i + 1][2 * j + 1] = Ty::mix(input.data[i][j], input.data[i + 1][j], input.data[i][j + 1], input.data[i + 1][j + 1], r);
				}
			}
		}
		for (int i = 0; i < SZ; ++i) {
			for (auto j : { 0U, SZ }) {
				mp.data[2 * i][2 * j] = input.data[i][j];
				float r = simpleNoiseFn(mp.basepos.x + mp.scale * (2 * i + 1), mp.basepos.y + mp.scale * j * 2);
				mp.data[2 * i + 1][2 * j] = Ty::mix(input.data[i][j], input.data[i + 1][j], r);

				if (j == 0U) {
					mp.data[2 * i][2 * j + 1] = Ty::mix(input.data[i][j], input.data[i][j + 1], r);
					mp.data[2 * i + 1][2 * j + 1] = Ty::mix(input.data[i][j], input.data[i + 1][j], input.data[i][j + 1], input.data[i + 1][j + 1], r);
				}
			}
		}
		mp.data[SZ * 2][SZ * 2] = input.data[SZ][SZ];

		//interior mixing
		//the mixing is absolutely deterministic and is a function of final datapoint
		for (int i = 1; i < SZ; ++i) {
			for (int j = 1; j < SZ; ++j) {
				mp.data[2 * i][2 * j] = input.data[i][j];
				float r = simpleNoiseFn(i, j);
				mp.data[2 * i][2 * j + 1] = Ty::mix(input.data[i][j], input.data[i][j + 1], r);
				mp.data[2 * i + 1][2 * j] = Ty::mix(input.data[i][j], input.data[i + 1][j], r);
				mp.data[2 * i + 1][2 * j + 1] = Ty::mix(input.data[i][j], input.data[i + 1][j], input.data[i][j + 1], input.data[i + 1][j + 1], r);

			}
		}

		return mp;
	}

	template<unsigned int SZ>
	Map<PreClimateData, SZ> GenPreClimateLayer<SZ>::Forward(Map<OceanMapData, SZ>& input) {

		Map<PreClimateData, SZ> mp(input.basepos, input.scale);

		using pii = std::pair<int, int>;
		// 0(dryest) <----> 3(most humid)
		//0. fill the map with initial value
		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) mp.data[i][j].prcpLevel = -1;
		}
		//1. do a distance-to-ocean-bfs
		std::queue<pii> q;
		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) {
				if (!input.data[i][j].isLand) {
					q.emplace(i, j);
					mp.data[i][j].prcpLevel = 0;
				}
			}
		}
		int di[]{ -1, 0, 1, 0 }, dj[]{ 0, 1, 0, -1 };
		while (q.size()) {
			auto [i, j] = q.front(); q.pop();
			for (int dir = 0; dir < 4; ++dir) {
				int ni = i + di[dir], nj = j + dj[dir];
				if (ni < 0 || ni > SZ || nj < 0 || nj > SZ) continue;
				if (mp.data[ni][nj].prcpLevel == -1) {
					mp.data[ni][nj].prcpLevel = std::min(4, mp.data[i][j].prcpLevel + 1);
					q.push({ ni, nj });
				}
			}
		}

		//2. give humidity levels according to the distance
		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) {
				//temporary logic: should be more random than this..
				mp.data[i][j].prcpLevel = 4 - mp.data[i][j].prcpLevel;
			}
		}

		//3. give random temperatures 
		Map<float, SZ> noise = WhiteNoise<SZ>::Forward(Map<float, 1>(input.basepos, input.scale));
		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) {
				mp.data[i][j].tempLevel = std::min(4, std::max(0, (int)(noise.data[i][j] * 4)));
			}
		}

		return mp;
	}

	template<unsigned int SZ>
	std::map<std::pair<int, int>, Map<BiomeData, SZ>> GenBiomeLayer<SZ>::biomeMapCache = {};

	template<unsigned int SZ>
	void GenBiomeLayer<SZ>::Stitch(Map<BiomeData, SZ>& input, Map<BiomeData, SZ>& other, int ioffset, int joffset) {
		if (ioffset < 0) { //top
			for (int j = 0; j <= SZ; ++j) {
				input.data[0][j] = other.data[SZ][j];
			}
		}
		else if (ioffset > 0) { //bottom
			for (int j = 0; j <= SZ; ++j) {
				input.data[SZ][j] = other.data[0][j];
			}
		}
		else if (joffset < 0) { //left
			for (int i = 0; i <= SZ; ++i) {
				input.data[i][0] = other.data[i][SZ];
			}
		}
		else {
			for (int i = 0; i <= SZ; ++i) { //right
				input.data[i][SZ] = other.data[i][0];
			}
		}
		return;
	}

	template<unsigned int SZ>
	Map<BiomeData, SZ> GenBiomeLayer<SZ>::Forward(Map<PreClimateData, SZ>& climateInput, Map<OceanMapData, SZ>& islandInput) {
		Map<BiomeData, SZ> mp(climateInput.basepos, climateInput.scale);
		Map<float, SZ> noise = WhiteNoise<SZ>::Forward(Map<float, 1>(climateInput.basepos, climateInput.scale)); //to turn some ocean cells into deep ocean

		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) {
				if (islandInput.data[i][j].isLand) {
					//land biomes
					int tmpLevel = climateInput.data[i][j].tempLevel;
					if (climateInput.data[i][j].prcpLevel < 2) {
						// dry biomes
						if (tmpLevel == 0) {
							mp.data[i][j].biomeType = BiomeType::TUNDRA;
						}
						else if (tmpLevel == 1) {
							mp.data[i][j].biomeType = BiomeType::SHRUBLAND;
						}
						else {
							mp.data[i][j].biomeType = BiomeType::DESERT;
						}
					}
					else { //wet biomes
						if (tmpLevel == 0) {
							mp.data[i][j].biomeType = BiomeType::SNOWLAND;
						}
						else if (tmpLevel == 1) {
							mp.data[i][j].biomeType = BiomeType::GRASSLAND;
						}
						else {
							mp.data[i][j].biomeType = BiomeType::RAINFOREST;
						}
					}
				}
				else {
					//ocean biomes
					bool isSurroundedByMoreOcean = true;
					int di[]{ -1, -1, -1, 0, 0, 1, 1, 1 }, dj[]{ -1, 0, 1, -1, 1, -1, 0, 1 };
					for (int dir = 0; dir < 8; ++dir) {
						int ni = i + di[dir], nj = j + dj[dir];
						if (ni < 0 || ni > SZ || nj < 0 || nj > SZ) continue;
						if (islandInput.data[ni][nj].isLand) { isSurroundedByMoreOcean = false; break; }
					}
					if (isSurroundedByMoreOcean && noise.data[i][j] < 0.3f) mp.data[i][j].biomeType = BiomeType::DEEP_OCEAN;
					else mp.data[i][j].biomeType = BiomeType::SHALLOW_OCEAN;
				}
			}
		}

		//stitch together if left or right exists
		int di[]{ -(int)SZ, 0, (int)SZ, 0 }, dj[]{ 0, (int)SZ, 0, -(int)SZ };
		for (int dir = 0; dir < 4; ++dir) {
			std::pair<int, int> idx = { mp.basepos.x + mp.scale * di[dir], mp.basepos.y + mp.scale * dj[dir] };
			if (biomeMapCache.count(idx)) {
				Stitch(mp, biomeMapCache[idx], di[dir], dj[dir]);
			}
		}

		//save
		biomeMapCache[{mp.basepos.x, mp.basepos.y}] = mp;

		return mp;
	}

	template<unsigned int SZ>
	Map<LandscapeData, SZ> GenLandscapeLayer<SZ>::Forward(Map<OceanMapData, SZ>& input) {
		//init maxAbsScale and roughness from two independently-scaled white noise
		Map<LandscapeData, SZ> mp(input.basepos, input.scale);
		const int roughness_scale = 64;//larger this value, the slower the variation in roughness map
		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) {
				LandscapeData & celldata = mp.data[i][j];
				celldata.maxAbsScale = 64 * simpleNoiseFn(mp.basepos.x + mp.scale * i, mp.basepos.y + mp.scale * j);
				celldata.roughness = simpleNoiseFn((mp.basepos.x + mp.scale * i)/roughness_scale, (mp.basepos.y + mp.scale*j)/roughness_scale);
			}
		}

		return mp;
	}

	template<unsigned int SZ>
	Map<LandscapeData, SZ> GenShorelineLayer<SZ>::Forward(Map<LandscapeData, SZ>& lscapeInput, Map<BiomeData, SZ>& biomeInput) {
		for (int i = 0; i <= SZ; ++i) {
			for (int j = 0; j <= SZ; ++j) {
				int di[]{ -1, -1, -1, 0, 0, 1, 1, 1 }, dj[]{ -1, 0, 1, -1, 1, -1, 0, 1 };
				bool isLand = biomeInput.data[i][j].biomeType != BiomeType::SHALLOW_OCEAN && biomeInput.data[i][j].biomeType != BiomeType::DEEP_OCEAN;
				bool isShore = false;
				for (int dir = 0; dir < 8; ++dir) {
					int ni = i + di[dir], nj = j + dj[dir];
					if (ni < 0 || ni > SZ || nj < 0 || nj > SZ) continue;
					bool isMoreLand = biomeInput.data[ni][nj].biomeType != BiomeType::SHALLOW_OCEAN && biomeInput.data[ni][nj].biomeType != BiomeType::DEEP_OCEAN;
					if (isLand ^ isMoreLand) {
						isShore = true; break;
					}
				}
				if (isShore) {
					//limit absolute scale at shore to 4
					lscapeInput.data[i][j].maxAbsScale = 4;
				}
			}
		}

		return lscapeInput;
	}

}
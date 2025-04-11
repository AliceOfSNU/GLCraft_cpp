#pragma once
#ifndef LAYERS_H
#define LAYERS_H

#include <vector>
#include <queue>
#include "map.h"

namespace MapGen {

	template<class InputTy, unsigned int InputSZ, class OutputTy, unsigned int OutputSZ>
	class Layer {
	public:

		/*
		All Layer subclass must implement a Forward pass.
		this reads and modifies the data of the map provided as argument,
		and the specific logic depends on the type of layer.
		*/
		//virtual static Map<OutputTy, OutputSZ> Forward() = 0;
		//virtual static Map<OutputTy, OutputSZ> Forward(Map<InputTy, InputSZ>& mp) = 0;
		//virtual static Map<OutputTy, OutputSZ> Forward(Map<InputTy, InputSZ>& mp1, Map<InputTy, InputSZ>& mp2) = 0;
	};

	template<typename T, typename... Types>
	class Inputs;

	class Empty;

	template<class Ty, unsigned int SZ>
	class Zoom : public Layer<Ty, SZ, Ty, SZ*2> {
	public:
		static Map<Ty, SZ*2> Forward(Map<Ty, SZ>&);
	};

	template<class Ty, unsigned int SZ>
	class NoisyZoom : public Layer<Ty, SZ, Ty, SZ * 2> {
	public:
		/// <summary>
		/// unlike the default zoom, which just smudges the boundary by allowing information to flow in one direction only,
		/// noisy zoom allows information to flow in both direction, hence possibly creating swaps or exchanges
		/// swaps are undesirable if you want patchy results, but are inevitable if you want a pixel to look at all its neighbors
		/// </summary>
		/// <param name=""></param>
		/// <returns></returns>
		static Map<Ty, SZ * 2> Forward(Map<Ty, SZ>&);
	};

	
	template<unsigned int SZ>
	class WhiteNoise: public Layer<Inputs<Empty>, 1, float, SZ> {
	public:
		static Map<float, SZ> Forward(const Map<float, 1>& input);
	};

	template<unsigned int SZ>
	class GenIslandLayer : public Layer<float, SZ, OceanMapData, SZ> {
	public:
		
		/// <summary>
		/// probabilistically decide whether each pixel is ocean or land.
		/// p is proportional to the amount of land present
		/// </summary>
		/// <param name="input"> map of floats between 0.0 and 1.0 </param>
		/// <returns> a map of OceanMapData. data indicates whether each pixel is ocean or land</returns>
		static Map<OceanMapData, SZ> Forward(Map<float, SZ>& input);
	};

	template<unsigned int SZ>
	class GenPreClimateLayer : public Layer<OceanMapData, SZ, PreClimateData, SZ> {
	public:
		/// <summary>
		/// generates temperature level and precipitation level for each region
		/// based on random noise and proximity to the ocean.
		/// generally, the close to the ocean, the higher the precipitation level.
		/// temperature level is more or less random.
		/// </summary>
		/// <param name="input"> map of ocean/land </param>
		/// <returns> map of temoerature and precipitation level </returns>
		static Map<PreClimateData, SZ> Forward(Map<OceanMapData, SZ>& input);
	};

	template<unsigned int SZ>
	class GenBiomeLayer : public Layer<Inputs<PreClimateData, OceanMapData>, SZ, BiomeData, SZ> {
	public:

		static std::map<std::pair<int, int>, Map<BiomeData, SZ>> biomeMapCache;
		static void Stitch(Map<BiomeData, SZ>& input, Map<BiomeData, SZ>& other, int ioffset, int joffset);

		/// <summary>
		/// assigns biomes to each cell, based on the temperature and precipitation level
		/// the basic logic is:
		/// 
		/// humidity/temp  0			1			  2
		///			0~1    tundra		shrubland	  desert
		///			2~3    snowland		grassland	  rainforest
		///         
		/// </summary>
		/// <param name="input"> map of temp and precipitation level </param>
		/// <returns> map of biome types </returns>
		static Map<BiomeData, SZ> Forward(Map<PreClimateData, SZ>& climateInput, Map<OceanMapData, SZ>& islandInput);
	};

	template<unsigned int SZ>
	class GenLandscapeLayer : public Layer<OceanMapData, SZ, LandscapeData, SZ> {
	public:

		/// <summary>
		/// assigns landscape generation parameters for each cell, 
		/// the parameters will decide how much and at which scale block heights may vary.
		/// special treatment for ocean biomes to clamp maximum height below sea level=0
		/// </summary>
		static Map<LandscapeData, SZ> Forward(Map<OceanMapData, SZ>& input);
		static Map<LandscapeData, SZ> Forward(Map<BiomeData, SZ>& biomeInput);

	};

	template<unsigned int SZ>
	class GenShorelineLayer : public Layer<Inputs<LandscapeData, BiomeData>, SZ, LandscapeData, SZ> {
	public:

		/// <summary>
		/// find the boundary between land and ocean,
		/// and limit the abs scale there to a small value(~4) so that a smooth shoreline exists
		/// </summary>
		static Map<LandscapeData, SZ> Forward(Map<LandscapeData, SZ>& lscapeInput, Map<BiomeData, SZ>& biomeInput);

	};

	// function definitions
	
	// this noise function is used only inside this file
	// hence scope is static
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

		//return 0.5f + 0.5f * sin(random);
		return fmodf(random + 0.2f, 1.0f);
	}

	static float simpleNoiseFn(vec2i v) {
		return simpleNoiseFn(v.x, v.y);
	}

	static vec2f simpleNoiseFn2D(int ix, int iy) {
		const unsigned w = 8 * sizeof(unsigned);
		const unsigned s = w / 2;
		unsigned a = ix, b = iy;
		a *= 3284157443;
		b ^= a << s | a >> w - s;
		b *= 1911520717;
		a ^= b << s | b >> w - s;
		a *= 2048419325;
		float random = a * (3.14159265 / ~(~0u >> 1));

		return vec2f{ sin(random + 0.05f), cos(random + 0.05f) };
	}

	static float dotGradient(int ix, int iy, float x, float y) {
		vec2f gradient = simpleNoiseFn2D(ix, iy);
		return ((x - (float)ix) * gradient.y + (y - (float)iy) * gradient.x);
	}

	static float lerp(float a, float b, float t) {
		return (1.0 - t) * a + t * b;
	}

	static float perlinNoiseFn(float x, float y) {
		//grid coords
		int x0 = x >= 0 ? (int)x : (int)x - 1;
		int y0 = y >= 0 ? (int)y : (int)y - 1;
		int x1 = x0 + 1, y1 = y0 + 1;
		float sx = x - x0, sy = y - y0;

		float n0 = dotGradient(x0, y0, x, y);
		float n1 = dotGradient(x1, y0, x, y);
		float ix0 = lerp(n0, n1, sx);

		n0 = dotGradient(x0, y1, x, y);
		n1 = dotGradient(x1, y1, x, y);
		float ix1 = lerp(n0, n1, sx);

		return lerp(ix0, ix1, sy);
	}
	
	template<unsigned int SZ>
	Map<float, SZ> WhiteNoise<SZ>::Forward(const Map<float, 1>& input) {
		Map<float, SZ> mp(input.basepos, input.scale);
		mp.pad = input.pad;
		for (int i = 0; i < mp.size(); ++i) {
			for (int j = 0; j < mp.size(); ++j) {
				mp.data[i][j] = simpleNoiseFn(input.MapToWorldPoint(i, j));
			}
		}

		//move constructor
		return mp;
	}

	template<unsigned int SZ>
	Map<OceanMapData, SZ> GenIslandLayer<SZ>::Forward(Map<float, SZ>& input) {
		Map<OceanMapData, SZ> mp(input.basepos, input.scale);
		mp.pad = input.pad;
		for (int i = 0; i < mp.size(); ++i) {
			for (int j = 0; j < mp.size(); ++j) {
				mp.data[i][j].isLand = (input.data[i][j] > 0.3f);
			}
		}
		//move constructor
		return mp;
	}

	/*
	this zoom function works when map zoom is 1.
	map inputs to zoom layer are assumed to have padding 2.
	this implementation is currently not used.
	*/

	template<class Ty, unsigned int SZ>
	Map<Ty, SZ * 2> NoisyZoom<Ty, SZ>::Forward(Map<Ty, SZ>& input) {

		// scale halves!
		Map<Ty, SZ * 2> mp(input.basepos, input.scale / 2);
		mp.pad = input.pad;

		// boundary mixing
		for (int j = 1; j < input.size()-1; ++j) {
			// i == 0
			float r = simpleNoiseFn(mp.MapToWorldPoint(0, 2*j-1));
			mp.data[0][2 * j - 1] = Ty::mix(input.data[0][j], input.data[1][j], r);
			r = simpleNoiseFn(mp.MapToWorldPoint(0, 2 * j));
			mp.data[0][2 * j] = Ty::mix(input.data[0][j], input.data[1][j], input.data[0][j + 1], input.data[1][j + 1], r);

			// i == mp.size()-1
			int i = mp.size() - 1;
			mp.data[i][2 * j - 1] = input.data[input.size() - 1][j];
			r = simpleNoiseFn(mp.MapToWorldPoint(i, 2 * j));
			mp.data[i][2 * j] = Ty::mix(input.data[input.size() - 1][j], input.data[input.size() - 1][j +1], r);

		}

		for (int i = 1; i < input.size()-1; ++i) {
			// j == 0
			float r = simpleNoiseFn(mp.MapToWorldPoint(2*i-1, 0));
			mp.data[2 * i - 1][0] = Ty::mix(input.data[i][0], input.data[i][1], r);
			r = simpleNoiseFn(mp.MapToWorldPoint(2*i, 0));
			mp.data[2 * i][0] = Ty::mix(input.data[i][0], input.data[i][1], input.data[i+1][0], input.data[i+1][1], r);

			// j == mp.size()-1
			int j = mp.size() - 1;
			mp.data[2 * i - 1][j] = input.data[i][input.size() - 1];
			r = simpleNoiseFn(mp.MapToWorldPoint(2 * i, j));
			mp.data[2 * i][j] = Ty::mix(input.data[i][input.size()-1], input.data[i + 1][input.size() - 1], r);

		}
		
		// remainders
		float r = simpleNoiseFn(mp.MapToWorldPoint(0, 0));
		mp.data[0][0] = Ty::mix(input.data[0][0], input.data[0][1], input.data[1][0], input.data[1][1], r);
		int end = mp.size() - 1, iend = input.size() - 1;
		r = simpleNoiseFn(mp.MapToWorldPoint(0, end));
		mp.data[0][end] = Ty::mix(input.data[0][iend], input.data[1][iend], r);
		r = simpleNoiseFn(mp.MapToWorldPoint(end, 0));
		mp.data[end][0] = Ty::mix(input.data[iend][0], input.data[iend][1], r);
		mp.data[end][end] = input.data[iend][iend];

		// interior mixing
		// the mixing is deterministic
		for (int i = 1; i < input.size()-1; ++i) {
			for (int j = 1; j < input.size()-1; ++j) {
				mp.data[2 * i - 1][2 * j - 1] = input.data[i][j];
				float r = simpleNoiseFn(mp.MapToWorldPoint(2 * i - 1, 2 * j));
				mp.data[2 * i - 1][2 * j] = Ty::mix(input.data[i][j], input.data[i][j + 1], r);
				r = simpleNoiseFn(mp.MapToWorldPoint(2 * i, 2 * j - 1));
				mp.data[2 * i][2 * j - 1] = Ty::mix(input.data[i][j], input.data[i + 1][j], r);
				r = simpleNoiseFn(mp.MapToWorldPoint(2 * i, 2 * j));
				mp.data[2 * i][2 * j] = Ty::mix(input.data[i][j], input.data[i + 1][j], input.data[i][j + 1], input.data[i + 1][j + 1], r);

			}
		}

		return mp;
	}


	template<class Ty, unsigned int SZ>
	Map<Ty, SZ * 2> Zoom<Ty, SZ>::Forward(Map<Ty, SZ>& input) {

		// scale halves!
		Map<Ty, SZ * 2> mp(input.basepos, input.scale / 2);
		if (input.pad != 2) {
			throw std::invalid_argument("input to zoom layer must have pad 2");
		}
		mp.pad = input.pad;

		// interior mixing
		// the mixing is deterministic
		for (int i = 0; i < SZ + 2; ++i) {
			for (int j = 0; j < SZ + 2; ++j) {
				mp.data[2 * i][2 * j] = input.data[i + 1][j + 1];
				float r = simpleNoiseFn(mp.MapToWorldPoint(2 * i, 2 * j + 1));
				mp.data[2 * i][2 * j + 1] = Ty::mix(input.data[i + 1][j + 1], input.data[i + 1][j + 2], r);
				r = simpleNoiseFn(mp.MapToWorldPoint(2 * i + 1, 2 * j));
				mp.data[2 * i + 1][2 * j] = Ty::mix(input.data[i + 1][j + 1], input.data[i + 2][j + 1], r);
				r = simpleNoiseFn(mp.MapToWorldPoint(2 * i + 1, 2 * j + 1));
				mp.data[2 * i + 1][2 * j + 1] = Ty::mix(input.data[i + 1][j + 1], input.data[i + 2][j + 1], input.data[i + 1][j + 2], input.data[i + 2][j + 2], r);
			}
		}

		return mp;
	}

	//template<class Ty, unsigned int SZ>
	//Map<Ty, SZ * 2> NoisyZoom<Ty, SZ>::Forward(Map<Ty, SZ>& input) {

	//	// scale halves!
	//	Map<Ty, SZ * 2> mp(input.basepos, input.scale / 2);
	//	mp.pad = input.pad;

	//	// boundary mixing
	//	for (int j = 1; j < input.size() - 1; ++j) {
	//		// i == 0
	//		float r = simpleNoiseFn(mp.MapToWorldPoint(0, 2 * j - 1));
	//		mp.data[0][2 * j - 1] = Ty::mix(input.data[0][j], input.data[1][j], input.data[0][j - 1], input.data[1][j - 1], r);
	//		r = simpleNoiseFn(mp.MapToWorldPoint(0, 2 * j));
	//		mp.data[0][2 * j] = Ty::mix(input.data[0][j], input.data[1][j], input.data[0][j + 1], input.data[1][j + 1], r);

	//		// i == mp.size()-1
	//		int end = mp.size() - 1, iend = input.size() - 1;
	//		r = simpleNoiseFn(mp.MapToWorldPoint(end, 2 * j - 1));
	//		mp.data[end][2 * j - 1] = Ty::mix(input.data[iend][j], input.data[iend - 1][j], input.data[iend][j - 1], input.data[iend - 1][j - 1], r);
	//		r = simpleNoiseFn(mp.MapToWorldPoint(end, 2 * j));
	//		mp.data[end][2 * j] = Ty::mix(input.data[iend][j], input.data[iend - 1][j], input.data[iend][j + 1], input.data[iend - 1][j + 1], r);
	//	}

	//	for (int i = 1; i < input.size() - 1; ++i) {
	//		// j == 0
	//		float r = simpleNoiseFn(mp.MapToWorldPoint(2 * i - 1, 0));
	//		mp.data[2 * i - 1][0] = Ty::mix(input.data[i][0], input.data[i][1], input.data[i - 1][0], input.data[i - 1][1], r);
	//		r = simpleNoiseFn(mp.MapToWorldPoint(2 * i, 0));
	//		mp.data[2 * i][0] = Ty::mix(input.data[i][0], input.data[i][1], input.data[i + 1][0], input.data[i + 1][1], r);

	//		// j == mp.size()-1
	//		int end = mp.size() - 1, iend = input.size() - 1;
	//		r = simpleNoiseFn(mp.MapToWorldPoint(2 * i - 1, end));
	//		mp.data[2 * i - 1][end] = Ty::mix(input.data[i][iend], input.data[i][iend - 1], input.data[i - 1][iend], input.data[i - 1][iend - 1], r);
	//		r = simpleNoiseFn(mp.MapToWorldPoint(2 * i, end));
	//		mp.data[2 * i][end] = Ty::mix(input.data[i][iend], input.data[i][iend - 1], input.data[i + 1][iend], input.data[i + 1][iend - 1], r);
	//	}


	//	// remainders
	//	int end = mp.size() - 1, iend = input.size() - 1;
	//	float r = simpleNoiseFn(mp.MapToWorldPoint(0, 0));
	//	mp.data[0][0] = Ty::mix(input.data[0][0], input.data[0][1], input.data[1][0], input.data[1][1], r);
	//	r = simpleNoiseFn(mp.MapToWorldPoint(0, end));
	//	mp.data[0][end] = Ty::mix(input.data[0][iend], input.data[1][iend], input.data[0][iend - 1], input.data[1][iend - 1], r);
	//	r = simpleNoiseFn(mp.MapToWorldPoint(end, 0));
	//	mp.data[end][0] = Ty::mix(input.data[iend][0], input.data[iend][1], input.data[iend - 1][0], input.data[iend - 1][1], r);
	//	r = simpleNoiseFn(mp.MapToWorldPoint(end, end));
	//	mp.data[end][end] = Ty::mix(input.data[iend][iend], input.data[iend][iend - 1], input.data[iend - 1][iend], input.data[iend - 1][iend - 1], r);

	//	// interior mixing
	//	// the mixing is deterministic
	//	for (int i = 1; i < input.size() - 1; ++i) {
	//		for (int j = 1; j < input.size() - 1; ++j) {
	//			float r = simpleNoiseFn(mp.MapToWorldPoint(2 * i - 1, 2 * j - 1));
	//			mp.data[2 * i - 1][2 * j - 1] = Ty::mix(input.data[i][j], input.data[i][j - 1], input.data[i - 1][j], input.data[i - 1][j - 1], r);
	//			r = simpleNoiseFn(mp.MapToWorldPoint(2 * i - 1, 2 * j));
	//			mp.data[2 * i - 1][2 * j] = Ty::mix(input.data[i][j], input.data[i][j + 1], input.data[i - 1][j], input.data[i - 1][j + 1], r);
	//			r = simpleNoiseFn(mp.MapToWorldPoint(2 * i, 2 * j - 1));
	//			mp.data[2 * i][2 * j - 1] = Ty::mix(input.data[i][j], input.data[i][j - 1], input.data[i + 1][j], input.data[i + 1][j - 1], r);
	//			r = simpleNoiseFn(mp.MapToWorldPoint(2 * i, 2 * j));
	//			mp.data[2 * i][2 * j] = Ty::mix(input.data[i][j], input.data[i + 1][j], input.data[i][j + 1], input.data[i + 1][j + 1], r);
	//		}
	//	}

	//	return mp;
	//}


	template<unsigned int SZ>
	Map<PreClimateData, SZ> GenPreClimateLayer<SZ>::Forward(Map<OceanMapData, SZ>& input) {

		Map<PreClimateData, SZ> mp(input.basepos, input.scale);
		mp.pad = input.pad;

		using pii = std::pair<int, int>;
		// 0(dryest) <----> 3(most humid)
		////0. fill the map with initial value
		//for (int i = 0; i < mp.size(); ++i) {
		//	for (int j = 0; j < mp.size(); ++j) mp.data[i][j].prcpLevel = -1;
		//}
		////1. do a distance-to-ocean-bfs
		//std::queue<pii> q;
		//for (int i = 0; i < mp.size(); ++i) {
		//	for (int j = 0; j < mp.size(); ++j) {
		//		if (!input.data[i][j].isLand) {
		//			q.emplace(i, j);
		//			mp.data[i][j].prcpLevel = 0;
		//		}
		//	}
		//}
		//int di[]{ -1, 0, 1, 0 }, dj[]{ 0, 1, 0, -1 };
		//while (q.size()) {
		//	auto [i, j] = q.front(); q.pop();
		//	for (int dir = 0; dir < 4; ++dir) {
		//		int ni = i + di[dir], nj = j + dj[dir];
		//		if (ni < 0 || ni >= mp.size() || nj < 0 || nj >= mp.size()) continue;
		//		if (mp.data[ni][nj].prcpLevel == -1) {
		//			mp.data[ni][nj].prcpLevel = std::min(4, mp.data[i][j].prcpLevel + 1);
		//			q.push({ ni, nj });
		//		}
		//	}
		//}

		//2. give humidity levels according to the distance
		for (int i = 0; i < mp.size(); ++i) {
			for (int j = 0; j < mp.size(); ++j) {
				//temporary logic: should be more random than this..
				//mp.data[i][j].prcpLevel = 4 - mp.data[i][j].prcpLevel;
				mp.data[i][j].prcpLevel = static_cast<int>(4 * simpleNoiseFn(mp.MapToWorldPoint(i, j)));
			}
		}

		//3. give random temperatures 
		Map<float, SZ> noise = WhiteNoise<SZ>::Forward(Map<float, 1>(input.basepos, input.scale));
		for (int i = 0; i < mp.size(); ++i) {
			for (int j = 0; j < mp.size(); ++j) {
				mp.data[i][j].tempLevel = std::min(4, std::max(0, static_cast<int>(noise.data[i][j] * 4)));
			}
		}

		return mp;
	}

	template<unsigned int SZ>
	std::map<std::pair<int, int>, Map<BiomeData, SZ>> GenBiomeLayer<SZ>::biomeMapCache = {};

	template<unsigned int SZ>
	void GenBiomeLayer<SZ>::Stitch(Map<BiomeData, SZ>& input, Map<BiomeData, SZ>& other, int ioffset, int joffset) {
		if (ioffset < 0) { // top
			for (int j = 0; j < input.size(); ++j) {
				input.data[0][j] = other.data[other.size()-1][j];
			}
		}
		else if (ioffset > 0) { // bottom
			for (int j = 0; j < input.size(); ++j) {
				input.data[input.size()-1][j] = other.data[0][j];
			}
		}
		else if (joffset < 0) { // left
			for (int i = 0; i < input.size(); ++i) {
				input.data[i][0] = other.data[i][other.size()-1];
			}
		}
		else {
			for (int i = 0; i < input.size(); ++i) { // right
				input.data[i][input.size()-1] = other.data[i][0];
			}
		}
		return;
	}

	template<unsigned int SZ>
	Map<BiomeData, SZ> GenBiomeLayer<SZ>::Forward(Map<PreClimateData, SZ>& climateInput, Map<OceanMapData, SZ>& islandInput) {
		Map<BiomeData, SZ> mp(climateInput.basepos, climateInput.scale);
		mp.pad = climateInput.pad;
		Map<float, SZ> noise = WhiteNoise<SZ>::Forward(Map<float, 1>(climateInput.basepos, climateInput.scale)); //to turn some ocean cells into deep ocean

		for (int i = 0; i < mp.size(); ++i) {
			for (int j = 0; j < mp.size(); ++j) {
				if (islandInput.data[i][j].isLand) {
					// land biomes
					int tmpLevel = climateInput.data[i][j].tempLevel;
					if (climateInput.data[i][j].prcpLevel <= 2) {
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
					else { // wet biomes
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
					// ocean biomes
					bool isSurroundedByMoreOcean = true;
					int di[]{ -1, -1, -1, 0, 0, 1, 1, 1 }, dj[]{ -1, 0, 1, -1, 1, -1, 0, 1 };
					for (int dir = 0; dir < 8; ++dir) {
						int ni = i + di[dir], nj = j + dj[dir];
						if (ni < 0 || ni >= mp.size() || nj < 0 || nj >= mp.size()) continue;
						if (islandInput.data[ni][nj].isLand) { isSurroundedByMoreOcean = false; break; }
					}
					if (isSurroundedByMoreOcean && noise.data[i][j] < 0.3f) mp.data[i][j].biomeType = BiomeType::DEEP_OCEAN;
					else mp.data[i][j].biomeType = BiomeType::SHALLOW_OCEAN;
				}
			}
		}

		// stitch together if left or right exists
		//int di[]{ -(int)SZ, 0, (int)SZ, 0 }, dj[]{ 0, (int)SZ, 0, -(int)SZ };
		//for (int dir = 0; dir < 4; ++dir) {
		//	std::pair<int, int> idx = { mp.basepos.x + mp.scale * di[dir], mp.basepos.y + mp.scale * dj[dir] };
		//	if (biomeMapCache.count(idx)) {
		//		Stitch(mp, biomeMapCache[idx], di[dir], dj[dir]);
		//	}
		//}

		//// save
		//biomeMapCache[{mp.basepos.x, mp.basepos.y}] = mp;

		return mp;
	}

	
	template<unsigned int SZ>
	Map<LandscapeData, SZ> GenLandscapeLayer<SZ>::Forward(Map<OceanMapData, SZ>& input) {
		// init maxAbsScale and roughness from two independently-scaled white noise
		Map<LandscapeData, SZ> mp(input.basepos, input.scale);
		const int roughness_scale = 64;//larger this value, the slower the variation in roughness map
		for (int i = 0; i < mp.size(); ++i) {
			for (int j = 0; j < mp.size(); ++j) {
				LandscapeData & celldata = mp.data[i][j];
				celldata.maxAbsScale = 64 * simpleNoiseFn(mp.MapToWorldPoint(i, j));
				celldata.roughness = simpleNoiseFn((mp.basepos.x + mp.scale * (i-mp.pad))/roughness_scale, (mp.basepos.y + mp.scale*(j-mp.pad))/roughness_scale);
			}
		}

		return mp;
	}

	template<unsigned int SZ>
	Map<LandscapeData, SZ> GenShorelineLayer<SZ>::Forward(Map<LandscapeData, SZ>& lscapeInput, Map<BiomeData, SZ>& biomeInput) {
		for (int i = 0; i < lscapeInput.size(); ++i) {
			for (int j = 0; j < lscapeInput.size(); ++j) {
				int di[]{ -1, -1, -1, 0, 0, 1, 1, 1 }, dj[]{ -1, 0, 1, -1, 1, -1, 0, 1 };
				bool isLand = biomeInput.data[i][j].biomeType != BiomeType::SHALLOW_OCEAN && biomeInput.data[i][j].biomeType != BiomeType::DEEP_OCEAN;
				bool isShore = false;
				for (int dir = 0; dir < 8; ++dir) {
					int ni = i + di[dir], nj = j + dj[dir];
					if (ni < 0 || ni >= biomeInput.size() || nj < 0 || nj >= biomeInput.size()) continue;
					bool isMoreLand = biomeInput.data[ni][nj].biomeType != BiomeType::SHALLOW_OCEAN && biomeInput.data[ni][nj].biomeType != BiomeType::DEEP_OCEAN;
					if (isLand ^ isMoreLand) {
						isShore = true; break;
					}
				}
				if (isShore) {
					// limit absolute scale at shore to 0
					lscapeInput.data[i][j].maxAbsScale = 0;
				}
			}
		}

		return lscapeInput;
	}

	template<unsigned int SZ>
	Map<LandscapeData, SZ> GenLandscapeLayer<SZ>::Forward(Map<BiomeData, SZ>& biomeInput) {
		Map<LandscapeData, SZ> mp(biomeInput.basepos, biomeInput.scale);
		// biomeInput has pad = 2,
		// result has pad = 1. removes 1 padding.
		mp.pad = 1;
		const int roughness_scale = 64;//larger this value, the slower the variation in roughness map

		for (int i = 0; i < mp.size(); ++i) {
			for (int j = 0; j < mp.size(); ++j) {
				LandscapeData& celldata = mp.data[i][j];
				vec2i wp = mp.MapToWorldPoint(i, j);
				float f = 0.002f;
				celldata.maxAbsScale = 32 * (1 + perlinNoiseFn(f * wp.x, f * wp.y));
				celldata.roughness = simpleNoiseFn((mp.basepos.x + mp.scale * (i - mp.pad)) / roughness_scale, (mp.basepos.y + mp.scale * (j - mp.pad)) / roughness_scale);
				
				// these offsets take into account the different paddings 1!=2.
				// do not change!
				int di[]{ 0, 0, 0, 1, 1, 2, 2, 2 }, dj[]{ 0, 1, 2, 0, 2, 0, 1, 2 };

				bool isLand = biomeInput.data[i+1][j+1].biomeType != BiomeType::SHALLOW_OCEAN && biomeInput.data[i][j].biomeType != BiomeType::DEEP_OCEAN;
				bool isShore = false;
				for (int dir = 0; dir < 8; ++dir) {
					int ni = i + di[dir], nj = j + dj[dir];
					if (ni < 0 || ni >= biomeInput.size() || nj < 0 || nj >= biomeInput.size()) continue;
					bool isMoreLand = biomeInput.data[ni][nj].biomeType != BiomeType::SHALLOW_OCEAN && biomeInput.data[ni][nj].biomeType != BiomeType::DEEP_OCEAN;
					if (isLand ^ isMoreLand) {
						isShore = true; break;
					}
				}
				if (isShore) {
					// limit absolute scale at shore to 0
					celldata.maxAbsScale = 0;
				}
			}
		}

		return mp;
	}
}
#endif
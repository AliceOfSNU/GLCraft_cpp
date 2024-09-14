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

	template<class T, class... Types>
	class Inputs;

	class Empty;

	template<class Ty, unsigned int SZ>
	class Zoom : public Layer<Ty, SZ, Ty, SZ*2> {
	public:
		static Map<Ty, SZ*2> Forward(Map<Ty, SZ>&);
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


}
#endif
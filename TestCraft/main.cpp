// TestCraft.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>

#include "map.cpp"
#include "layers.cpp"

using namespace MapGen;

template<class OceanMapData, unsigned int SZ>
void printMap(Map<OceanMapData, SZ>& mp) {
	for (int i = 0; i < SZ; ++i) {
		for (int j = 0; j < SZ; ++j) {
			std::cout << (mp.data[i][j].isLand ? 'O' : '.');
		}
		std::cout << std::endl;
	}
	return;
}

template<unsigned int SZ>
void saveMap(Map<BiomeData, SZ>& mp) {
	std::ofstream imgfile("biomes.txt");
	for (int i = 0; i <= SZ; ++i) {
		for (int j = 0; j <= SZ; ++j) {
			imgfile << (int)(mp.data[i][j].biomeType)<< ' ';
		}
	}

	imgfile.close();
}

int main()
{
    std::cout << "Hello World!\n";


	//all intermediate maps must be deleted 

	// level 8
	Map<float, 8> noiseMp = WhiteNoise<8>::Forward();
	noiseMp.basepos = { 0, 0 };
	noiseMp.scale = 2048;
	Map<OceanMapData, 8> bOceanMp8 = GenIslandLayer<8>::Forward(noiseMp);

	// level 16
	Map<OceanMapData, 16> bOceanMp16 = Zoom<OceanMapData, 8>::Forward(bOceanMp8);

	// level 32
	Map<OceanMapData, 32> bOceanMp32 = Zoom<OceanMapData, 16>::Forward(bOceanMp16);
	Map<PreClimateData, 32> climateMp32 = GenPreClimateLayer<32>::Forward(bOceanMp32);
	Map<BiomeData, 32> biomeMp32 = GenBiomeLayer<32>::Forward(climateMp32, bOceanMp32);

	// level 64
	Map<BiomeData, 64> biomeMp64 = Zoom<BiomeData, 32>::Forward(biomeMp32);
	//Map<LandscapeData, 64> landscapeMp64 = LandscapeLayer<64>::Forward(bOceanMp64);

	// level 128
	Map<BiomeData, 128> biomeMp128 = Zoom<BiomeData, 64>::Forward(biomeMp64);
	//Map<LandscapeData, 128> landscapeMp128 = Zoom<LandscapeData, 64>::Forward(landscapeMp64);

	//// level 256
	Map<BiomeData, 256> biomeMp256 = Zoom<BiomeData, 128>::Forward(biomeMp128);
	//Map<LandscapeData, 256> landscapeMp256 = Zoom<LandscapeData, 128>::Forward(landscapeMp128);

	//// level 512
	//Map<BiomeData, 512> biomeMp512 = Zoom<BiomeData, 256>::Forward(biomeMp256);
	//Map<LandscapeData, 256> landscapeMp256 = Zoom<LandscapeData, 256>::Forward(landscapeMp256);



	saveMap(biomeMp256);
	std::cout << std::endl;


	
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

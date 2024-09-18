#include "pch.h"
#include "map.h"

namespace MapGen {

	//map ctors and dtors
	template<typename MapDataTy, unsigned int SZ>
	Map<MapDataTy, SZ>::Map() : basepos({0, 0}), scale(1) {
		data = new MapDataTy * [SZ + 1];
		for (int i = 0; i <= SZ; ++i) {
			data[i] = new MapDataTy[SZ + 1];
		}
	}

	template<typename MapDataTy, unsigned int SZ>
	Map<MapDataTy, SZ>::Map(const vec2i& pos, const int sc): basepos(pos), scale(sc){
		data = new MapDataTy * [SZ+1];
		for (int i = 0; i <= SZ; ++i) {
			data[i] = new MapDataTy[SZ+1];
		}
	}

	//template<typename MapDataTy, unsigned int SZ>
	//Map<MapDataTy, SZ>::Map(Map&& other) : data(other.data), basepos(other.basepos), scale(other.scale) {
	//	std::cout << "move ctor" << std::endl;
	//	other.data = nullptr;
	//}

	//template<typename MapDataTy, unsigned int SZ>
	//Map<MapDataTy, SZ>& Map<MapDataTy, SZ>::operator=(const Map<MapDataTy, SZ>&& other){
	//	data = other.data;
	//	other.data = nullptr;
	//	basepos(other.basepos);
	//	scale = other.scale;
	//	std::cout << "move assign" << std::endl;
	//}

	//template<typename MapDataTy, unsigned int SZ>
	//Map<MapDataTy, SZ>::~Map() {
	//	for (int i = 0; i <= SZ; ++i) delete[] data[i];
	//	delete[] data;
	//}

	//utility function to convert the map data array index to actual world coordinate
	//all computation is integer.
	template<typename MapDataTy, unsigned int SZ>
	vec2i Map<MapDataTy, SZ>::MapToWorldPoint(int i, int j) const {
		return { basepos.x + i * scale, basepos.y + j * scale };
	}

	template<typename MapDataTy, unsigned int SZ>
	vec2i Map<MapDataTy, SZ>::WorldToMapPoint(int i, int j) const {
		return { (i - basepos.x) / scale, (j - basepos.y) / scale };
	}

}
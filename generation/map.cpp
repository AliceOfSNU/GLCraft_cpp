#include "pch.h"
#include "map.h"

namespace MapGen {

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


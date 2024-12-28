#pragma once
#include "GLObjects.h"
#include <vector>

class WeatherParticleRenderObj {
public:

	WeatherParticleRenderObj(float radius, float height, int num_particles);
	void Render();

	Texture2D texture;
	std::vector<float> offsets;


protected:
	void Build(); //copies all vertex data
	int particle_cnt;

	VAO vao;
	VBO vtx_buffer;
	VBO uv_buffer;
	VBO offset_buffer;
};
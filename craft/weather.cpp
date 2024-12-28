#include "weather.h"

WeatherParticleRenderObj::WeatherParticleRenderObj(float radius, float height, int num_particles): particle_cnt(num_particles), texture("raindrop.png", GL_TEXTURE0, GL_RGBA) {
	offsets.resize(3*num_particles);
	for (int i = 0; i < num_particles; ++i) {
		float x = -radius + radius * 2 * (rand() % 1000 / 1000.f);
		float z = -radius + radius * 2 * (rand() % 1000 / 1000.f);
		float y = height * (rand() % 1000 / 1000.f);
		offsets[3 * i] = x;
		offsets[3 * i + 1] = y;
		offsets[3 * i + 2] = z;
	}
	Build();
}

void WeatherParticleRenderObj::Build() {
	vao.Create();
	vtx_buffer.Create();
	uv_buffer.Create();
	offset_buffer.Create();

	std::vector<float> vts = std::vector<float>{
		//0: xyz		
		-0.5f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};
	std::vector<float> uvs = std::vector<float>{
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};

	vao.Bind();
	vtx_buffer.BufferData(vts.data(), sizeof(vts[0]) * vts.size());
	vao.LinkAttrib(vtx_buffer, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	uv_buffer.BufferData(uvs.data(), sizeof(uvs[0]) * uvs.size());
	vao.LinkAttrib(uv_buffer, 1, 2, GL_FLOAT, 2 * sizeof(float), (void*)0);
	offset_buffer.BufferData(offsets.data(), sizeof(offsets[0]) * offsets.size());
	vao.LinkAttrib(offset_buffer, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	vao.SetAttribDivisor(2, 1);

}

void WeatherParticleRenderObj::Render() {
	texture.Bind();
	vao.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particle_cnt);

}
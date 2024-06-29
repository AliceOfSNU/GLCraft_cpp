#include "gui.h"

CircleFill::CircleFill(float r) {
	radius = { r * 0.6, r };
	size = 0.6f;
	vao.Create();
	vbo.Create();

	vao.Bind();
	vbo.BufferData(vertices.data(), sizeof(vertices[0]) * 18);
	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	vao.Unbind();
}

void CircleFill::Render(float percentage, float sx, float sy) {
	glm::vec2 center = glm::vec2(sx, resolution.y - sy);
	sx = 2.0f * sx / resolution.x - 1.0f, sy = 1.0f - 2.0f * sy / resolution.y;
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(sx, sy, 0.0f));
	//glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(size, size, 0.0f));
	shader.use();
	shader.setVec2f("_Center", glm::value_ptr(center));
	shader.setMat4f("_Model", glm::value_ptr(model));
	shader.setVec3f("_FillColor", glm::value_ptr(fillcolor));
	shader.setVec3f("_BackColor", glm::value_ptr(backcolor));
	shader.setVec2f("_Radius", glm::value_ptr(radius));
	shader.setFloat("_Progress", percentage);

	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
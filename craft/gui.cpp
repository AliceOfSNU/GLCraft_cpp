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

SolidGUIRenderObject::SolidGUIRenderObject (glm::vec3 fillcolor): fillcolor(fillcolor) {}

void SolidGUIRenderObject::Render() {
	float sx = 2.0f * position.x / GUI::SCREEN_WIDTH - 1.0f, sy = 2.0f * position.y / GUI::SCREEN_HEIGHT - 1.0f;
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(sx, sy, 0.0f));

	shader->use();
	shader->setMat4f("_Model", glm::value_ptr(model));
	shader->setVec3f("_Color", glm::value_ptr(fillcolor));

	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, idxcnt);
}

void GUIRenderObject::Render() {
	//not implemented
}

void GUIRenderObject::Build() {
	vao.Create();
	vbo.Create();
	vao.Bind();
	vbo.BufferData(vertices.data(), sizeof(vertices[0]) * vertices.size());
	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
	vao.Unbind();

	idxcnt = vertices.size() / 3;
	vertices.clear();
}

void GUIRenderObject::Destroy() {
	vao.Delete();
	vbo.Delete();
}


void GUI::Render() {
	// NOT IMPLEMENTED
}

void GUI::Destroy() {
	for (auto& child : children) child->Destroy();
}

void GUI::Build() {
	for (auto& child : children) child->Build();
}

void GUI::Update() {
	for (auto& child : children) child->Update();
}

void Panel::Build() {
	if (isBuilt) return;
	renderObj->position = { centerX, centerY };
	renderObj->vertices = {
		// X, Y, Z
		-width/SCREEN_WIDTH, height/SCREEN_HEIGHT, 0.0f,
		-width/SCREEN_WIDTH, -height/SCREEN_HEIGHT, 0.0f,
		width/SCREEN_WIDTH,  -height/SCREEN_HEIGHT, 0.0f,
		-width/SCREEN_WIDTH, height/SCREEN_HEIGHT, 0.0f,
		width/SCREEN_WIDTH, -height/SCREEN_HEIGHT, 0.0f,
		width/SCREEN_WIDTH,  height/SCREEN_HEIGHT, 0.0f,
	};
	for (auto& child : children) child->Build();
	renderObj->Build();
	isBuilt = true;
}

void Panel::Render() {
	for (auto& child : children) child->Render();
	renderObj->Render();
}

void Panel::Destroy() {
	renderObj->Destroy();
	for (auto& child : children) child->Destroy();
}

void Button::Update() {
	auto mouse = GUIManager::GetInstance().mouseXY;
	if (mouse.x > centerX - width / 2 && mouse.x < centerX + width / 2 &&
		mouse.y > centerY - height / 2 && mouse.y < centerY + height / 2) {
		if (!isHovering) {
			isHovering = true;
			OnMouseEnter(*this);
		}
		if (GUIManager::GetInstance().mouseEvent == 2) OnClick(*this);
	}
	else {
		if (isHovering) {
			isHovering = false;
			OnMouseExit(*this);
		}
	}
}

// all ui subclass definitions can go here..
void Inventory::Select(int num) {
	auto btn = dynamic_cast<Button*>(children[selected].get());
	auto solidRend = dynamic_cast<SolidGUIRenderObject*>(btn->renderObj.get());
	solidRend->fillcolor = { 0.1f, 0.1f, 0.1f };
	
	selected = num;
	btn = dynamic_cast<Button*>(children[selected].get());
	solidRend = dynamic_cast<SolidGUIRenderObject*>(btn->renderObj.get());
	solidRend->fillcolor = { 0.0f, 1.0f, 1.0f };
}


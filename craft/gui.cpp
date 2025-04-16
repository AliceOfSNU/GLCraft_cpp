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

GUIManager::GUIManager(){
	solidUIShader = std::make_shared<Shader>("resources/solidGUI.vs", "resources/solidGUI.fs");
	atlasUIShader = std::make_shared<Shader>("resources/atlasGUI.vs", "resources/atlasGUI.fs");
}

SolidGUIRenderObject::SolidGUIRenderObject (): fillcolor(glm::vec3(0.0f, 0.0f, 0.0f)) {
	shader = GUIManager::GetInstance().solidUIShader;
}

SolidGUIRenderObject::SolidGUIRenderObject (glm::vec3 fillcolor): fillcolor(fillcolor) {
	shader = GUIManager::GetInstance().solidUIShader;
}

void SolidGUIRenderObject::Render() {
	float sx = 2.0f * position.x / GUI::SCREEN_WIDTH - 1.0f, sy = 2.0f * position.y / GUI::SCREEN_HEIGHT - 1.0f;
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(sx, sy, 0.0f));

	shader->use();
	shader->setMat4f("_Model", glm::value_ptr(model));
	shader->setVec3f("_Color", glm::value_ptr(fillcolor));

	vao.Bind();
	glDrawArrays(GL_TRIANGLES, 0, idxcnt);
}


AtlasGUIRenderObject::AtlasGUIRenderObject(std::shared_ptr<TextureArray2D> tex): arr_tex(tex){
	shader = GUIManager::GetInstance().atlasUIShader;
}

void AtlasGUIRenderObject::Build(){
	vao.Create();
	vbo.Create();
	vbo_uv.Create();
	vao.Bind();
	vbo.BufferData(vertices.data(), sizeof(vertices[0]) * vertices.size());
	vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0); 
	vbo_uv.BufferData(uvs.data(), sizeof(uvs[0]) * uvs.size());
	vao.LinkAttrib(vbo_uv, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*)0); 
	vao.Unbind();

	idxcnt = vertices.size() / 3;
	vertices.clear();
	uvs.clear();
}

void AtlasGUIRenderObject::Render() {
	float sx = 2.0f * position.x / GUI::SCREEN_WIDTH - 1.0f, sy = 2.0f * position.y / GUI::SCREEN_HEIGHT - 1.0f;
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(sx, sy, 0.0f));

	shader->use();
	shader->setMat4f("_Model", glm::value_ptr(model));
	vao.Bind();
	arr_tex->Bind();
	glDrawArrays(GL_TRIANGLES, 0, idxcnt);
}

void AtlasGUIRenderObject::Destroy(){
	vao.Delete();
	vbo.Delete();
	vbo_uv.Delete();
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
			OnMouseEnter();
		}
		if (GUIManager::GetInstance().mouseEvent == 2) OnClick();
	}
	else {
		if (isHovering) {
			isHovering = false;
			OnMouseExit();
		}
	}
}

// all ui subclass definitions can go here..
InventoryButton::InventoryButton(int idx, float cx, float cy, float w, float h): itemidx(idx){
	this->centerX = cx;
	this->centerY = cy;
	this->width = w;
	this->height = h;
	renderObj = std::make_unique<SolidGUIRenderObject>(glm::vec3(0.1f, 0.1f, 0.1f));
	std::shared_ptr<InvenBtnImg> image = std::make_shared<InvenBtnImg>(itemidx);
	image->centerX = this->centerX;
	image->centerY = this->centerY;
	image->width = this->width * 0.8f;
	image->height = this->height * 0.8f;
	children.push_back(std::move(image));
}

void InventoryButton::OnClick(){
	auto inven = dynamic_cast<Inventory*>(GUIManager::GetInstance().windows["inventory"].get());
	inven->Select(itemidx);
}

void InventoryButton::OnMouseEnter(){
	auto solidRend = dynamic_cast<SolidGUIRenderObject*>(renderObj.get());
	auto inven = dynamic_cast<Inventory*>(GUIManager::GetInstance().windows["inventory"].get());
	
	if(itemidx != inven->selected) solidRend->fillcolor = { 0.2f, 0.2f, 0.2f };
}

void InventoryButton::OnMouseExit(){
	auto solidRend = dynamic_cast<SolidGUIRenderObject*>(renderObj.get());
	auto inven = dynamic_cast<Inventory*>(GUIManager::GetInstance().windows["inventory"].get());
	
	if(itemidx != inven->selected) solidRend->fillcolor = { 0.1f, 0.1f, 0.1f };
}

Inventory::Inventory(){
	centerX = 400.f;
	centerY = 400.f;
	width = 400.f;
	height = 200.f;
	renderObj = std::make_unique<SolidGUIRenderObject>(glm::vec3{0.3f, 0.3f, 0.3f});
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 5; ++j) {
			float centerX = 400.f - (60.f * 2) + j * 60.f;
			float width = 50.f;
			float centerY = 400.f + (60.f * 1) - i * 60.f;
			float height = 50.f;
			std::shared_ptr<InventoryButton> item = std::make_shared<InventoryButton>(5*i+j, centerX, centerY, width, height);
			item->id = "inventory_" + std::to_string(5 * i + j);
			children.push_back(std::move(item));
		}
	}
	auto btn = dynamic_cast<Button*>(children[selected].get());
	auto solidRend = dynamic_cast<SolidGUIRenderObject*>(btn->renderObj.get());
	solidRend->fillcolor = { 0.0f, 1.0f, 1.0f };
}

// maps btn index to actual block type
// this is the order in which the thumbnail image should be (with dirt(0) on bottom of image)
// this is also the order the blocks appear on the inventory, so should be chosen with care.
const std::vector<BlockDB::BlockType> Inventory::btnToBlkTy = { 
	BlockType::BLOCK_DIRT, BlockType::BLOCK_GRASS, BlockType::BLOCK_GRANITE, BlockType::BLOCK_WOOD, BlockType::BLOCK_COBBLESTONE,
	BlockType::BLOCK_WOODEN_STAIR_P0, BlockType::BLOCK_COBBLESTONE_STAIR_P0, BlockType::BLOCK_TORCH
};

int Inventory::selected = 0;
BlockDB::BlockType Inventory::selectedBlkTy = Inventory::btnToBlkTy[0];

void Inventory::Select(int num) {
	auto btn = dynamic_cast<Button*>(children[selected].get());
	auto solidRend = dynamic_cast<SolidGUIRenderObject*>(btn->renderObj.get());
	solidRend->fillcolor = { 0.1f, 0.1f, 0.1f };
	
	selected = num;
	if(selected < btnToBlkTy.size()){
		selectedBlkTy = btnToBlkTy[selected];
	}
	btn = dynamic_cast<Button*>(children[selected].get());
	solidRend = dynamic_cast<SolidGUIRenderObject*>(btn->renderObj.get());
	solidRend->fillcolor = { 0.0f, 1.0f, 1.0f };
}

InvenBtnImg::InvenBtnImg(int idx): imgidx(idx){
	auto image_ref = std::make_shared<TextureArray2D>("resources/thumbnails.png", 64, 64, 8, GL_RGBA);
	renderObj = std::make_unique<AtlasGUIRenderObject>(image_ref);
}

void InvenBtnImg::Build() {
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
	AtlasGUIRenderObject* atlasRenderObj = dynamic_cast<AtlasGUIRenderObject*>(renderObj.get());
	atlasRenderObj->uvs = {
		0.0f, 1.0f, (float)imgidx,
		0.0f, 0.0f, (float)imgidx,
		1.0f, 0.0f, (float)imgidx,
		0.0f, 1.0f, (float)imgidx,
		1.0f, 0.0f, (float)imgidx,
		1.0f, 1.0f, (float)imgidx
	};
	for (auto& child : children) child->Build();
	renderObj->Build();
	isBuilt = true;
}
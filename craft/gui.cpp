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
	for (auto& child : children) {
		child->Render();
	}
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
// maps button index in inventory(linear numbering from left top corner..)
// to image index in the thumbnails atlas.
// not all images in the thumbnails atlas are shown in the inventory. 
// usually, only building blocks or tools are shown
// there are empty slots at the end, which we repeat 16 for now

const std::vector<int> Inventory::btnToImgIdx = { 
	//page 1
	(int)ItemType::DIRT, (int)ItemType::GRASS, (int)ItemType::GRANITE, (int)ItemType::WOOD, (int)ItemType::COBBLESTONE, 
	(int)ItemType::WOODEN_STAIR_P0, (int)ItemType::COBBLESTONE_STAIR_P0, (int)ItemType::TORCH, (int)ItemType::COAL_ORE, (int)ItemType::IRON_ORE, 
	(int)ItemType::DIAMOND_ORE, (int)ItemType::STONE_PICKAXE, (int)ItemType::IRON_PICKAXE, (int)ItemType::DIAMOND_PICKAXE, (int)ItemType::DIAMOND_PICKAXE, 
	//page 2
	(int)ItemType::DIAMOND_PICKAXE, (int)ItemType::DIAMOND_PICKAXE,
	// end(do we need this?)
	(int)ItemType::DIAMOND_PICKAXE,
};

InventoryButton::InventoryButton(int idx, float cx, float cy, float w, float h): itemidx(idx){
	this->centerX = cx;
	this->centerY = cy;
	this->width = w;
	this->height = h;
	renderObj = std::make_unique<SolidGUIRenderObject>(glm::vec3(0.1f, 0.1f, 0.1f));
	if(idx < Inventory::MAX_ITEMS){
		std::shared_ptr<InvenBtnImg> image = std::make_shared<InvenBtnImg>(Inventory::btnToImgIdx[idx]);
		image->centerX = this->centerX;
		image->centerY = this->centerY;
		image->width = this->width * 0.8f;
		image->height = this->height * 0.8f;
		children.push_back(std::move(image));
	}
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

InventoryPageButton::InventoryPageButton(int dp): dpage(dp){
	renderObj = std::make_unique<SolidGUIRenderObject>(glm::vec3(0.2f, 0.2f, 0.2f));
}

void InventoryPageButton::OnClick(){
	auto inven = dynamic_cast<Inventory*>(GUIManager::GetInstance().windows["inventory"].get());
	inven->MovePage(dpage);
}

Inventory::Inventory(){
	centerX = INVEN_CENTER_X;
	centerY = INVEN_CENTER_Y;
	width = INVEN_WIDTH;
	height = INVEN_HEIGHT;
	renderObj = std::make_unique<SolidGUIRenderObject>(glm::vec3{0.3f, 0.3f, 0.3f});
	MakePage(0);

	auto btn = dynamic_cast<Button*>(children[selected].get());
	auto solidRend = dynamic_cast<SolidGUIRenderObject*>(btn->renderObj.get());
	solidRend->fillcolor = { 0.0f, 1.0f, 1.0f };
}

// maps btn index to actual block type
// this is the order in which the thumbnail image should be (with dirt(0) on bottom of image)
// this is also the order the blocks appear on the inventory, so should be chosen with care.
const std::vector<BlockDB::BlockType> Inventory::btnToBlkTy = { 
	BlockType::BLOCK_DIRT, BlockType::BLOCK_GRASS, BlockType::BLOCK_GRANITE, BlockType::BLOCK_WOOD, BlockType::BLOCK_COBBLESTONE,
	BlockType::BLOCK_WOODEN_STAIR_P0, BlockType::BLOCK_COBBLESTONE_STAIR_P0, BlockType::BLOCK_TORCH,
	BlockType::BLOCK_COAL_ORE, BlockType::BLOCK_IRON_ORE, BlockType::BLOCK_DIAMOND_ORE
};

const std::vector<Tool::ToolType> Inventory::btnToToolTy = {
	ToolType::TOOL_STONE_PICKAXE, ToolType::TOOL_IRON_PICKAXE, ToolType::TOOL_DIAMOND_PICKAXE, ToolType::TOOL_DIAMOND_PICKAXE, 
	//page 2
	ToolType::TOOL_DIAMOND_PICKAXE, ToolType::TOOL_DIAMOND_PICKAXE,
};

int Inventory::selected = 0;
int Inventory::page = 0;
BlockDB::BlockType Inventory::selectedBlkTy = Inventory::btnToBlkTy[0];

void Inventory::Select(int num) {
	if(num >= MAX_ITEMS) return;
	auto btn = dynamic_cast<Button*>(children[selected%ITEMS_PER_PAGE].get());
	auto solidRend = dynamic_cast<SolidGUIRenderObject*>(btn->renderObj.get());
	solidRend->fillcolor = { 0.1f, 0.1f, 0.1f };
	
	// update selected field with the newly selected index
	selected = num;

	if(selected < btnToBlkTy.size()){
		Tool::UnequipTool();
		selectedBlkTy = btnToBlkTy[selected];
	}
	else if(selected - btnToBlkTy.size() < btnToToolTy.size()){
		int toolnum = selected - btnToBlkTy.size();
		Tool::EquipTool(btnToToolTy[toolnum]);
		selectedBlkTy = BlockDB::BLOCK_COUNT;
	}

	btn = dynamic_cast<Button*>(children[selected%ITEMS_PER_PAGE].get());
	solidRend = dynamic_cast<SolidGUIRenderObject*>(btn->renderObj.get());
	solidRend->fillcolor = { 0.0f, 1.0f, 1.0f };
}

void Inventory::MakePage(int page){
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 5; ++j) {
			float centerX = INVEN_CENTER_X - (60.f * 2) + j * 60.f;
			float width = 50.f;
			float centerY = INVEN_CENTER_Y + (60.f * 1) - i * 60.f;
			float height = 50.f;
			std::shared_ptr<InventoryButton> item = std::make_shared<InventoryButton>(page*ITEMS_PER_PAGE + 5*i+j, centerX, centerY, width, height);
			item->id = "inventory_" + std::to_string(page*ITEMS_PER_PAGE + 5 * i + j);
			children.push_back(std::move(item));
		}
	}

	// make page buttons
	if(page > 0){
		std::shared_ptr<InventoryPageButton> prvBtn = std::make_shared<InventoryPageButton>(-1);
		prvBtn->centerX = INVEN_CENTER_X - INVEN_WIDTH/2 + 10.f + 15.f;
		prvBtn->width = 30.f;
		prvBtn->centerY = centerY;
		prvBtn->height = 50.f;
		prvBtn->id = "inventory_prv";
		children.push_back(std::move(prvBtn));
	}

	if(page < NUM_PAGES - 1){		
		std::shared_ptr<InventoryPageButton> nxtBtn = std::make_shared<InventoryPageButton>(1);
		nxtBtn->centerX = INVEN_CENTER_X + INVEN_WIDTH/2 - 10.f - 15.f;
		nxtBtn->width = 30.f;
		nxtBtn->centerY = centerY;
		nxtBtn->height = 50.f;
		nxtBtn->id = "inventory_nxt";
		children.push_back(std::move(nxtBtn));
	}

}

void Inventory::MovePage(int dpage){
	int newpage = page + dpage;
	if(newpage < 0 || newpage >= NUM_PAGES) return;
	
	page = newpage;
	// first destroy all existing buttons, and even inventory panel itself!
	// this is needed to make sure VAOs and VBOs are deleted
	Destroy();
	children.clear();
	MakePage(newpage);
	// all children must be rebuilt
	isBuilt = false;
	Build();
}

InvenBtnImg::InvenBtnImg(int idx): imgidx(idx){
	auto image_ref = std::make_shared<TextureArray2D>("resources/thumbnails.png", 64, 64, 17, GL_RGBA);
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
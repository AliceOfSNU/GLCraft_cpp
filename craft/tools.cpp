#include "tools.hpp"

void Tool::EquipTool(ToolType toolTy){
    switch(toolTy){
        case ToolType::TOOL_STONE_PICKAXE:
            equippedTool = std::make_shared<Pickaxe>(ToolGrade::STONE);
            break;
        case ToolType::TOOL_IRON_PICKAXE:
            equippedTool = std::make_shared<Pickaxe>(ToolGrade::IRON);
            break;
        case ToolType::TOOL_DIAMOND_PICKAXE:
            equippedTool = std::make_shared<Pickaxe>(ToolGrade::DIAMOND);
            break;
    }
}

Tool::~Tool(){
    //unimplemented
}

ItemType Tool::ResolveDrop(BlockDB::BlockType blkTy){
    BlockDB::BlockDataRow& row = BlockDB::GetInstance().tbl[blkTy];
    ItemType itmTy = row.drop;
    switch(blkTy){
        case BlockType::BLOCK_IRON_ORE:
            if(equippedTool){
                itmTy = ItemType::IRON;
            }
            break;
        case BlockType::BLOCK_COAL_ORE:
            if(equippedTool){
                itmTy = ItemType::COAL;
            }
            break;
        case BlockType::BLOCK_DIAMOND_ORE:
            if(equippedTool && 
                (equippedTool->grade == ToolGrade::IRON ||
                equippedTool->grade == ToolGrade::DIAMOND )){
                itmTy = ItemType::DIAMOND;
            }
            break;
        default:
            break; //nothing to do
    }

    return itmTy;
}


glm::mat4 Tool::ComputeModelMatrix(){
    return glm::mat4(1.0f);
}

std::shared_ptr<Tool> Tool::equippedTool = nullptr;
void Tool::UnequipTool(){
    equippedTool = nullptr;
}

Pickaxe::Pickaxe(ToolGrade gr){
    switch(gr){
        case ToolGrade::STONE:{
            renderobj.LoadModel(std::make_shared<ModelWrapper>("resources/models/tools/minecraft_stone_pickaxe/scene.gltf"), glm::vec3(0, 0, 0));
            grade = gr;
            break;
        }
        case ToolGrade::IRON:{
            renderobj.LoadModel(std::make_shared<ModelWrapper>("resources/models/tools/minecraft_iron_pickaxe/scene.gltf"), glm::vec3(0, 0, 0));
            grade = gr;
            break;
        }
        case ToolGrade::DIAMOND:{
            renderobj.LoadModel(std::make_shared<ModelWrapper>("resources/models/tools/minecraft_diamond_pickaxe/scene.gltf"), glm::vec3(0, 0, 0));    
            grade = gr;
            break;
        }
    }
    renderobj.Build();
    pitch = DEFAULT_PITCH; 
}

Pickaxe::Pickaxe(){
    // default constructor exists just because it is needed
    // it performs nothing
}

Pickaxe::~Pickaxe(){
    renderobj.DeleteBuffers();
}

glm::mat4 Pickaxe::ComputeModelMatrix(){
    auto modelmat = glm::translate(glm::mat4(1.0f),position);
    // default rotations : to look realistic, like the angles when you are about to swing a real pickaxe with your left hand.
    modelmat = glm::rotate(modelmat, glm::radians(90.f), glm::vec3(0.f,0.f,1.0f));
    modelmat = glm::rotate(modelmat, glm::radians(-25.f), glm::vec3(1.f,0.f,0.0f));
    modelmat = glm::rotate(modelmat, glm::radians(pitch), glm::vec3(0.f,1.f,0.f));
    modelmat = glm::scale(modelmat, glm::vec3(0.5f,0.5f,0.5f));
    return modelmat;
}


void Pickaxe::Trigger(){
    if(!anim){
        anim_time = 0.0f;
        anim = true;
    }
    stop_signal = false;
}

void Pickaxe::Stop(){
    stop_signal = true;
}

void Pickaxe::Update(float deltaTime){
    // the following value is tuned. be careful with modifying them
    // especially the z offset puts the pickaxe near the camera plane.
    // this allows the tool to be rendered in front of other objects.
    position = glm::vec3(-0.3f, -0.3f, -1.0f);
    if(anim){
        anim_time += deltaTime;
        if(anim_time > SWINGUP_TIME){
            pitch = DEFAULT_PITCH; 
            anim_time = 0.0f;
            if(stop_signal){
                stop_signal = false;
                anim = false;
            }
        }else if(anim_time > SWINGDOWN_TIME){
            pitch = DEFAULT_PITCH + SWING_SPEED*(SWINGUP_TIME - anim_time);
        }else{
            pitch = DEFAULT_PITCH + SWING_SPEED*anim_time;
        }
    }
}
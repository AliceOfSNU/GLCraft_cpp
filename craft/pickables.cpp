#include "pickables.h"


// TODO
// 1. render anything
// 2. make it render with proper img
// 3. make it fall to ground
std::set<std::shared_ptr<PickupItem>> PickupItem::allPickups = {};


PickupItem::PickupItem(int idx, glm::vec3 pos): itemidx(idx), position(pos), isvalid(true), elapsed(0.0f){
    renderobj.imgidx = itemidx;
    renderobj.Build();
}

void PickupItem::Update(float deltaTime){
    elapsed += deltaTime;
    glm::vec3 newpos = position - deltaTime*FALL_SPEED*glm::vec3(0.0f, 1.0f, 0.0f);
    if(!World::GetInstance().IsOccupied(newpos - glm::vec3{0.0f, 0.5f, 0.0f}, true)){
        position = newpos;
    }
}

glm::mat4 PickupItem::ComputeModelMatrix(){
    auto modelmat = glm::translate(glm::mat4(1.0f), position + glm::vec3{0.0f, 0.2*sin(elapsed), 0.0f});
    return modelmat;
}
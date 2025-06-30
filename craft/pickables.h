#ifndef PICKABLES_H
#define PICKABLES_H

#include "rendering.hpp"
#include "world.h"

#include <set>
class PickupItem{
    //using ItemType = Inventory::ItemType;
public:
    glm::vec3 position;
    int itemidx;
    static std::set<std::shared_ptr<PickupItem>> allPickups;
    const float FALL_SPEED = 7.0f;
    PickupItem(int itemidx, glm::vec3 pos);
    glm::mat4 ComputeModelMatrix();
    void Update(float deltaTime);
    PickupItemRenderObject renderobj;
    const static std::vector<int> PickupItem::itemTypeToAtlasIdx;
private:
    bool isvalid; // false after the item has been picked up and put to backpack
    float elapsed; //elapsed time since this pickup item has become live.  
};

#endif
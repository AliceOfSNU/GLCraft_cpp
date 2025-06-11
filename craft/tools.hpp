#ifndef TOOLS_H
#define TOOLS_H
#include <memory>
#include <iostream>
#include "rendering.hpp"
#include "camera.h"
#include "blocks.hpp"

enum class ToolGrade{
    STONE, IRON, DIAMOND
};

class Tool{
public:
    using BlockType = BlockDB::BlockType;
    enum ToolType{
        TOOL_STONE_PICKAXE, TOOL_IRON_PICKAXE, TOOL_DIAMOND_PICKAXE, TOOL_COUNT
    };
    
    static void EquipTool(ToolType toolTy);
    static void UnequipTool();
    static std::shared_ptr<Tool> equippedTool;
    ModelRenderObject renderobj;

    virtual void Trigger() = 0;
    virtual void Stop() = 0;
    virtual void Update(float deltaTime) = 0;
    static float ResolveMineTime(BlockDB::BlockType blkTy);
    static ItemType ResolveDrop(BlockDB::BlockType blkTy);
    virtual glm::mat4 ComputeModelMatrix();
    virtual ~Tool();

    glm::vec3 position;
    float pitch = 55.0f;
    ToolGrade grade;
};

class Pickaxe: public Tool{
public:
    Pickaxe();
    Pickaxe(ToolGrade grade);
    virtual void Trigger() override;
    virtual void Stop() override;
    virtual void Update(float deltaTime) override;
    virtual glm::mat4 ComputeModelMatrix() override;
    virtual ~Pickaxe() override;
private:
    bool anim = false;
    bool stop_signal = false;
    float anim_time = 0.0f;
    const float SWINGUP_TIME = 1.0f;
    const float SWINGDOWN_TIME = 0.5f;
    const float SWING_SPEED = 90.0f;
    const float DEFAULT_PITCH = 55.f;
};
#endif
#ifndef ANIMALS_H
#define ANIMALS_H

#include "GLObjects.h"
#include "rendering.hpp"
#include "collision.h"
#include "world.h"

#include <algorithm>
#include <vector>
class Animal{
public:
    static const int RENDER_DIST = 64;
    static const int MAX_COUNT = 128;
    static const int MIN_COUNT = 16;
    enum AnimalType {
        PIG, ANIMAL_COUNT
    };

    enum State {
        MOVE, TURN, STOP, DEAD
    };

    // all animals should live here and basically not move elsewhere
    static std::set<std::shared_ptr<Animal>> allAnimals;
	static std::map<AnimalType, std::shared_ptr<ModelWrapper>> modelzoo; 
    
    static void LoadModels();
    static void Remove(glm::vec3 playerPosition);
    static void Spawn(glm::vec3 playerPosition);

    ModelRenderObject renderobj;
    glm::vec3 position;
    glm::vec3 front = {1.0, 0.0, 0.0}, right;
    float yaw;
    float state_time = 0.f, target_time = 0.f;
    float speed = 1.0f, rotspeed = 5.0f;
    State state = STOP;
    
    Animal(AnimalType ty, glm::vec3 pos, float yaw);
    Animal(const Animal& other) = delete;
    ~Animal();

    glm::mat4 ComputeModelMatrix();
    void Update(float deltaTime);
};

#endif

#include "animals.h"

std::map<Animal::AnimalType, std::shared_ptr<ModelWrapper>> Animal::modelzoo = {};

void Animal::LoadModels(){ 
    modelzoo[AnimalType::PIG] = std::make_shared<ModelWrapper>("resources/models/minecraft_pig/scene.gltf");
	auto modelmat = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/20.0f, 1.0f/20.0f, 1.0f/20.0f));
	modelmat = glm::rotate(modelmat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelzoo[AnimalType::PIG]->modelMatrix = modelmat;
	modelzoo[AnimalType::PIG]->ApplyTransformsToMesh();
    modelzoo[AnimalType::ANIMAL_COUNT] = std::make_shared<ModelWrapper>("resources/models/minecraft_pig/scene.gltf");
}
std::set<std::shared_ptr<Animal>> Animal::allAnimals = {};

Animal::~Animal(){
    renderobj.DeleteBuffers();
}

Animal::Animal(AnimalType ty, glm::vec3 pos, float yw): position(pos), yaw(yw){
    // rendering init
    auto model = Animal::modelzoo[ty];
    renderobj.LoadModel(model, glm::vec3(0, 0, 0));
    renderobj.Build();
}

glm::mat4 Animal::ComputeModelMatrix(){
    auto modelmat = glm::translate(glm::mat4(1.0f),position);
	modelmat = glm::rotate(modelmat, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	modelmat = glm::rotate(modelmat, glm::radians(roll), right);
    return modelmat;
}

glm::vec3 updatePositionWithCollisionCheck2(glm::vec3 begin_pos, glm::vec3 end_pos, glm::vec3 box_dims) {
	//using namespace Collision;
	Collision::CollisionCheck checker(begin_pos, end_pos, box_dims);
	Collision::AABB swAABB = checker.ComputeBroadphaseAABB(); //get swept AABB

	std::vector<Collision::AABB> colliders;
	std::vector<std::pair<Chunk::ivec3, Chunk::ivec3>> collide_blocks;
	int endx = (int)(swAABB.start.x + swAABB.scale.x + 0.5f);
	int endy = (int)(swAABB.start.y + swAABB.scale.y + 0.5f);
	int endz = (int)(swAABB.start.z + swAABB.scale.z + 0.5f);
	for (int x = (int)(swAABB.start.x-0.5f); x <= endx; ++x) {
		for (int y = (int)(swAABB.start.y-0.5f); y <= endy; ++y) {
			for (int z = (int)(swAABB.start.z-0.5f); z <= endz; ++z) {
				Chunk* chunk = World::GetInstance().CurrentChunk({ x, y, z });
				if(!chunk) continue;
				Chunk::ivec3 blockidx = chunk->FindBlockIndex({ x, y, z });
				if (!BlockDB::GetInstance().isWalkThrough(chunk->grid[blockidx.x][blockidx.y][blockidx.z])) {
					colliders.push_back({ {x-0.5f, y-0.5f, z-0.5f}, {1.0f, 1.0f, 1.0f}, chunk->grid[blockidx.x][blockidx.y][blockidx.z] });
					collide_blocks.push_back({chunk->chunkIdx, blockidx});
				}
			}
		}
	}

	// check lateral collision for climbing up stairs!
	Collision::Collision lat_col = checker.GetLateralHit(colliders);
	if(lat_col.normal.x * lat_col.vel.x > 0.001 || lat_col.normal.x * lat_col.vel.x < -0.001 ||
		lat_col.normal.z * lat_col.vel.z > 0.001 || lat_col.normal.z * lat_col.vel.z < -0.001){
			auto& [cidx, bidx] = collide_blocks[lat_col.hit_index];
			BlockDB::BlockType upblock = World::GetInstance().GetChunkByIndex(cidx)->grid[bidx.x][bidx.y+1][bidx.z];
			if(upblock == BlockDB::BlockType::BLOCK_AIR){
				lat_col.remain_vel.y = 0.01f;
			}
			checker = Collision::CollisionCheck(lat_col.stop_pos, lat_col.stop_pos + lat_col.remain_vel, box_dims);
	}
	Collision::Collision col = checker.GetFirstHit(colliders);
	checker = Collision::CollisionCheck(col.stop_pos, col.stop_pos + col.remain_vel, box_dims);
	Collision::Collision col2 = checker.GetFirstHit(colliders);
	checker = Collision::CollisionCheck(col2.stop_pos, col2.stop_pos + col2.remain_vel, box_dims);
	Collision::Collision col3 = checker.GetFirstHit(colliders);

	return col3.stop_pos + col3.remain_vel;
}

void Animal::Update(float deltaTime){
    state_time += deltaTime;
    if(state_time > target_time){
		if(state == MOVE){
			state = STOP;
			target_time = (float)(rand()%5 + 3);
        }else if(state == STOP){
			state = TURN; 
			target_time = (float)(rand()%5 + 3);
        }else if(state == TURN){
			bool can_move = false;
			target_time = 0.0f;
			for(int t = rand()%5+3; t >= 1; --t){
				// check for water
				glm::vec3 sim_pos = position + speed*t*front - glm::vec3(0.0, 1.0, 0.0);
				glm::ivec3 chunkIdx = Chunk::WorldToChunkIndex(sim_pos);
				Chunk* chunk = World::GetInstance().GetChunkByIndex(chunkIdx);
				if (!chunk) continue;
				glm::ivec3 bidx = chunk->FindBlockIndex(sim_pos);
				if(chunk->grid[bidx.x][bidx.y][bidx.z] != BlockDB::BlockType::BLOCK_WATER){
					can_move = true;
					target_time = (float)(t);
					break;
				}
			}
			if(can_move) state = MOVE; // if cannot move, continue to turn!
			else{
				state = TURN;
				target_time = (float)(rand()%5 + 3);
			}
        }else if(state == DYING){
			state = DEAD;
		}
        state_time = 0.0f;
    }else if(state == State::MOVE){
		//position = position + speed * deltaTime * front;
        glm::vec3 end_pos = position + speed * deltaTime * front;
        end_pos -= deltaTime * glm::vec3(0.0f, 9.8f, 0.0f);
		position = updatePositionWithCollisionCheck2(position - glm::vec3{0.5f, 0.5f, 0.5f}, end_pos - glm::vec3{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}) + glm::vec3(0.5f,0.5f,0.5f);
    }else if(state == State::TURN){
		yaw += deltaTime * rotspeed;
        glm::vec3 dir = glm::vec3(0.f, 0.f, 0.f);
        dir.x = cos(glm::radians(-yaw));
        dir.z = sin(glm::radians(-yaw));
        front = glm::normalize(dir);
        right = glm::normalize(glm::cross(front, glm::vec3(0.0,1.0,0.0)));
    }else if(state == State::DYING){
		roll = state_time/target_time * 90.f;
	}
	
	hit_effect_time -= deltaTime;
	if(hit_effect_time < 0.0f) {
		hit_effect_time = 0;
	}

	
}

void Animal::Hit(){
	hit_effect_time = 0.5f;
	hp -= 1;
	if(hp <= 0) {
		state = State::DYING;
		state_time = 0.0, target_time = 1.0;
	}
}

void Animal::Remove(glm::vec3 playerPosition){
	std::vector<std::shared_ptr<Animal>> to_remove;
	for(auto& animal:allAnimals){
		auto pos = animal->position;
		if(
			animal->state == State::DEAD ||
			abs(pos.x - playerPosition.x) > 64 ||
			abs(pos.y - playerPosition.y) > 64 ||
			abs(pos.z - playerPosition.z) > 64
		)
		to_remove.push_back(animal);
	}

	for(auto& animal: to_remove){
		allAnimals.erase(animal);
	}
}

void Animal::Spawn(glm::vec3 playerPosition){
	if(allAnimals.size() >= MIN_COUNT) return;
	int xoffset = playerPosition.x + rand()%(2*RENDER_DIST) - RENDER_DIST; 
	int zoffset = playerPosition.z + rand()%(2*RENDER_DIST) - RENDER_DIST; 
	int yoffset = -64;

	Chunk* chunk = nullptr;
	glm::ivec3 bidx;
	while(!chunk && yoffset <= 64){
		glm::ivec3 chunkIdx = Chunk::WorldToChunkIndex(glm::vec3{xoffset, yoffset, zoffset});
		chunk = World::GetInstance().GetChunkByIndex(chunkIdx);
		if(chunk) bidx = chunk->FindBlockIndex(glm::vec3{xoffset, yoffset, zoffset});
		yoffset += 32;
	}
	if(!chunk) return;

	while(chunk){
		for(int j = 0; j <= Chunk::HEIGHT-2; ++j){
			if(chunk->grid[bidx.x][j][bidx.z] == BlockDB::BlockType::BLOCK_GRASS &&
			chunk->grid[bidx.x][j+1][bidx.z] == BlockDB::BlockType::BLOCK_AIR){
				allAnimals.insert(std::make_shared<Animal>(Animal::AnimalType::PIG, glm::vec3(xoffset, chunk->basepos.y+j+1, zoffset), 0));
				return;
			}
		}
		Chunk* upchunk = World::GetInstance().GetChunkByIndex({chunk->chunkIdx.x, chunk->chunkIdx.y + 1, chunk->chunkIdx.z});
		if(upchunk){
			if(chunk->grid[bidx.x][Chunk::HEIGHT-1][bidx.z] == BlockDB::BlockType::BLOCK_GRASS &&
				upchunk->grid[bidx.x][0][bidx.z] == BlockDB::BlockType::BLOCK_AIR){
					allAnimals.insert(std::make_shared<Animal>(Animal::AnimalType::PIG, glm::vec3(xoffset, upchunk->basepos.y, zoffset), 0));
					return;
				}
		}
		chunk = upchunk;
	}
}
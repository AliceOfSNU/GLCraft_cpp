#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "blocks.hpp"
// helper function, returns final position after collisions
glm::vec3 updatePositionWithCollisionCheck(glm::vec3 begin, glm::vec3 end, glm::vec3 boxdim);

namespace Collision {

	struct Collision {
		glm::vec3 vel; // ending(target) position - starting position
		glm::vec3 remain_vel; // (1-time)*vel
		glm::vec3 normal; // the outward facing unit vector of the surface that the object collided with.s
		float time; // t (in proportion of velocity) until first collision
		glm::vec3 stop_pos; // final position, assuming object stops immediately after colliding.
	};

	// a block has integer center positions,
	// thus the vertices are all Z+0.5f form, so is start.
	// and scales are integers.
	struct AABB {
		AABB(glm::vec3 start, glm::vec3 scale, BlockDB::BlockType ty) : start(start), scale(scale), blkTy(ty) {}
		glm::vec3 start;
		glm::vec3 scale;
		BlockDB::BlockType blkTy;
	};

	struct EntryEvent {
		float x_entry, y_entry, z_entry;
		float xt_entry, yt_entry, zt_entry;
		BlockDB::BlockType type;
	};

	class CollisionCheck {
	public:

		// constructor. sets the required parameters for computing collision
		// start_pos: position of the object at start of frame
		// end_pos: position of object at end of frame
		// velocity: vector velocity. end_pos - start_pos
		// box_dim: the x/y/z scales of the object's AABB bounding box.
		CollisionCheck(glm::vec3 start_pos, glm::vec3 end_pos, glm::vec3 box_dim) : 
			start_pos(start_pos), end_pos(end_pos), velocity(end_pos - start_pos), box_dim(box_dim) {}

		// returns the AABB describing the movement of the object 
		// the user should determine which blocks overlaps with this AABB,
		// and thus is a candidate for collision.
		AABB ComputeBroadphaseAABB();

		// returns the first hit collision info.
		// boxes: AABBs to check for collision with.
		Collision GetFirstHit(const std::vector<AABB>& boxes);

		glm::vec3 GetHitNormal(EntryEvent entry);
		// returns the second hit, assumes GetFirstHit() is already called.
		// Collision GetSecondHit();

	private:
		glm::vec3 start_pos;
		glm::vec3 end_pos;
		glm::vec3 velocity;
		glm::vec3 box_dim;
		Collision primary_collision;

	};
}
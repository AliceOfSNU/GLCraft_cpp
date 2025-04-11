#include "collision.h"

namespace Collision {

	AABB CollisionCheck::ComputeBroadphaseAABB() {
		float x, y, z, sx, sy, sz;
		if (velocity.x > 0) {
			x = start_pos.x;
			sx = velocity.x + box_dim.x;
		}
		else {
			x = start_pos.x + velocity.x;
			sx = -velocity.x + box_dim.x;
		}
		if (velocity.y > 0) {
			y = start_pos.y;
			sy = velocity.y + box_dim.y;
		}
		else {
			y = start_pos.y + velocity.y;
			sy = -velocity.y + box_dim.y;
		}
		if (velocity.z > 0) {
			z = start_pos.z;
			sz = velocity.z + box_dim.z;
		}
		else {
			z = start_pos.z + velocity.z;
			sz = -velocity.z + box_dim.z;
		}
		return AABB({x, y, z}, {sx, sy, sz}, BlockDB::BlockType::BLOCK_COUNT);
	}

	glm::vec3 CollisionCheck::GetHitNormal(EntryEvent entry) {
		float xt_entry = entry.xt_entry, yt_entry = entry.yt_entry, zt_entry = entry.zt_entry;
		float x_entry = entry.x_entry, y_entry = entry.y_entry, z_entry = entry.z_entry;
		glm::vec3 normal;
		if (xt_entry > yt_entry && xt_entry > zt_entry) {
			// hits on x first
			if (x_entry >= 0.0f) {
				normal = { -1.0f, 0.0f, 0.0f };
			}
			else {
				normal = { 1.0f, 0.0f, 0.0f };
			}
		}
		else if (yt_entry >= xt_entry && yt_entry >= zt_entry) {
			// hits on y first
			if (y_entry >= 0.0f) {
				normal = { 0.0f, -1.0f, 0.0f };
			}
			else {
				normal = { 0.0f, 1.0f, 0.0f };
			}
		}
		else {
			// hits on z first
			if (z_entry >= 0.0f) {
				normal = { 0.0f, 0.0f, -1.0f };
			}
			else {
				normal = { 0.0f, 0.0f, 1.0f };
			}
		}
		return normal;
	}

	Collision CollisionCheck::GetFirstHit(const std::vector<AABB>& boxes) {
		// sort all existing collosions in order of entry time
		
		using entry_pair = std::pair<float, EntryEvent>;
		std::vector<entry_pair> entry_events;
		for (const auto& box : boxes) {
			float entryTime, exitTime;
			// distances
			float x_entry, y_entry, z_entry;
			float x_exit, y_exit, z_exit;
			// time, as proportion of velocity
			float xt_entry, yt_entry, zt_entry;
			float xt_exit, yt_exit, zt_exit;

			// TODO: compute xyz.
			if (velocity.x > 0.0f) {
				x_entry = box.start.x - (start_pos.x + box_dim.x);
				x_exit = (box.start.x + box.scale.x) - start_pos.x;
			}
			else {
				x_entry = (box.start.x + box.scale.x) - start_pos.x;
				x_exit = box.start.x - (start_pos.x + box_dim.x);
			}
			if (velocity.y > 0.0f) {
				y_entry = box.start.y - (start_pos.y + box_dim.y);
				y_exit = (box.start.y + box.scale.y) - start_pos.y;
			}
			else {
				y_entry = (box.start.y + box.scale.y) - start_pos.y;
				y_exit = box.start.y - (start_pos.y + box_dim.y);
			}
			if (velocity.z > 0.0f) {
				z_entry = box.start.z - (start_pos.z + box_dim.z);
				z_exit = (box.start.z + box.scale.z) - start_pos.z;
			}
			else {
				z_entry = (box.start.z + box.scale.z) - start_pos.z;
				//if(box.blkTy == 7) std::cout << (box.start.z + box.scale.z) << "-" << start_pos.z << '\n';
				z_exit = box.start.z - (start_pos.z + box_dim.z);
			}
			// compute xt, yt, and zt.
			// check for division by zero
			if (velocity.x == 0.0f) {
				if (start_pos.x >= box.start.x + box.scale.x || start_pos.x + box_dim.x <= box.start.x) xt_entry = std::numeric_limits<float>::infinity();
				else xt_entry = -std::numeric_limits<float>::infinity();
				xt_exit = std::numeric_limits<float>::infinity();
			}
			else {
				xt_entry = x_entry / velocity.x;
				xt_exit = x_exit / velocity.x;
			}
			if (velocity.y == 0.0f) {
				if (start_pos.y >= box.start.y + box.scale.y || start_pos.y + box_dim.y <= box.start.y) yt_entry = std::numeric_limits<float>::infinity();
				else yt_entry = -std::numeric_limits<float>::infinity();
				yt_exit = std::numeric_limits<float>::infinity();
			}
			else {
				yt_entry = y_entry / velocity.y;
				yt_exit = y_exit / velocity.y;
			}
			if (velocity.z == 0.0f) {
				if (start_pos.z >= box.start.z + box.scale.z || start_pos.z + box_dim.z <= box.start.z) zt_entry = std::numeric_limits<float>::infinity();
				else zt_entry = -std::numeric_limits<float>::infinity();
				zt_exit = std::numeric_limits<float>::infinity();
			}
			else {
				zt_entry = z_entry / velocity.z;
				zt_exit = z_exit / velocity.z;
			}
			entryTime = std::max({ xt_entry, yt_entry, zt_entry });
			exitTime = std::min({ xt_exit, yt_exit, zt_exit });
			if (entryTime >= exitTime || entryTime < -0.0f || entryTime > 1.0f || exitTime < 0.0f) {
				continue; // no hitwww
			}
			else {
				entry_events.push_back({entryTime, {x_entry, y_entry, z_entry, xt_entry, yt_entry, zt_entry, box.blkTy}});
			}
		}

		if (entry_events.size() == 0) {
			Collision col;
			col.time = -1.0f;
			col.stop_pos = end_pos;
			col.vel = velocity;
			col.normal = { 0.0f, 0.0f, 0.0f };
			col.remain_vel = { 0.0f, 0.0f, 0.0f };
			return col;
		}

		std::sort(entry_events.begin(), entry_events.end(), [](const entry_pair& a, const entry_pair& b) {return a.first < b.first;});
		auto [entryTime, entry] = entry_events[0];

		Collision col;
		col.time = entryTime;
		col.vel = velocity;
		col.stop_pos = start_pos + velocity * entryTime;
		col.remain_vel = velocity * (1.0f - entryTime);

		glm::vec3 normal = GetHitNormal(entry);
		col.normal = normal; 
		float dotp = normal.x * col.remain_vel.x + normal.y * col.remain_vel.y + normal.z * col.remain_vel.z;
		col.remain_vel = col.remain_vel - dotp * normal;

		//if (entry_events.size() >= 2 && entry_events[1].first - entryTime < 0.01f) {
		//	auto entry2 = entry_events[1].second;
		//	normal = GetHitNormal(entry2);
		//	dotp = normal.x * col.remain_vel.x + normal.y * col.remain_vel.y + normal.z * col.remain_vel.z;
		//	col.remain_vel = col.remain_vel - dotp * normal;
		//}
		
		//std::cout << "collided with " << entry.type << std::endl;
		return col;
	}
}
#include "ray.h"

Raycaster::Raycaster(const Ray& ray, float dist):ray(ray), distance(dist){}

bool Raycaster::CheckAABB(Collision::BoxCollider& collider){
    float x0, x1, y0, y1, z0, z1;
    if(ray.dir.x >= 0){
        x0 = ray.pos.x, x1 = ray.pos.x + distance * ray.dir.x;
    }else{
        x0 = ray.pos.x + distance * ray.dir.x, x1 = ray.pos.x;
    }
    if(ray.dir.y >= 0){
        y0 = ray.pos.y, y1 = ray.pos.y + distance * ray.dir.y;
    }else{
        y0 = ray.pos.y + distance * ray.dir.y, y1 = ray.pos.y;
    }
    if(ray.dir.z >= 0){
        z0 = ray.pos.z, z1 = ray.pos.z + distance * ray.dir.z;
    }else{
        z0 = ray.pos.z + distance * ray.dir.z, z1 = ray.pos.z;
    }
    if(collider.start.x > x1 || collider.start.x + collider.scale.x < x0 ||
        collider.start.y > y1 || collider.start.y + collider.scale.y < y0 ||
        collider.start.z > z1 || collider.start.z + collider.scale.z < z0
    ) return false;

    return true;
}
bool Raycaster::GetFirstHit(OUT Collision::BoxCollider& hit){
    if(colliders.empty()) return false;
    std::vector<std::pair<float, int>> entry_events;
    for(int i = 0; i < colliders.size(); ++i){
        // distances
        float x_entry, y_entry, z_entry;
        float x_exit, y_exit, z_exit;
        // time, as proportion of velocity
        float xt_entry, yt_entry, zt_entry;
        float xt_exit, yt_exit, zt_exit;
        const auto& box = colliders[i];

        if(ray.dir.x > 0.0f){
            x_entry = box.start.x - ray.pos.x;
            x_exit = (box.start.x + box.scale.x) - ray.pos.x;
        }else{
            x_entry = (box.start.x + box.scale.x) - ray.pos.x;
            x_exit = box.start.x - ray.pos.x;
        }
        if(ray.dir.y > 0.0f){
            y_entry = box.start.y - ray.pos.y;
            y_exit = (box.start.y + box.scale.y) - ray.pos.y;
        }else{
            y_entry = (box.start.y + box.scale.y) - ray.pos.y;
            y_exit = box.start.y - ray.pos.y;
        }
        if(ray.dir.z > 0.0f){
            z_entry = box.start.z - ray.pos.z;
            z_exit = (box.start.z + box.scale.z) - ray.pos.z;
        }else{
            z_entry = (box.start.z + box.scale.z) - ray.pos.z;
            z_exit = box.start.z - ray.pos.z;
        }

        if(ray.dir.x == 0.0f){
            xt_entry = -std::numeric_limits<float>::infinity();
            xt_exit = std::numeric_limits<float>::infinity();
        }else{
            xt_entry = x_entry * ray.invDir.x;
            xt_exit = x_exit * ray.invDir.x;
        }
        if(ray.dir.y == 0.0f){
            yt_entry = -std::numeric_limits<float>::infinity();
            yt_exit = std::numeric_limits<float>::infinity();
        }else{
            yt_entry = y_entry * ray.invDir.y;
            yt_exit = y_exit * ray.invDir.y;
        }
        if(ray.dir.z == 0.0f){
            zt_entry = -std::numeric_limits<float>::infinity();
            zt_exit = std::numeric_limits<float>::infinity();
        }else{
            zt_entry = z_entry * ray.invDir.z;
            zt_exit = z_exit * ray.invDir.z;
        }

        float entryTime = std::max({ xt_entry, yt_entry, zt_entry });
        float exitTime = std::min({ xt_exit, yt_exit, zt_exit });
        if (entryTime >= exitTime || entryTime < -0.0f || exitTime < 0.0f) {
            continue; 
        } else {
            entry_events.emplace_back(entryTime, i);
        }
    }
    
    if(entry_events.size() == 0) return false;
    std::sort(entry_events.begin(), entry_events.end(), [](const std::pair<float, int>& a, const std::pair<float, int>& b) {return a.first < b.first;});
    auto [entryTime, hit_index] = entry_events[0];
    hit = colliders[hit_index];
    return true;
}
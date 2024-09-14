#pragma once
#ifndef RAY_H
#define RAY_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

/* Hanjun Kim 2024 */
class Ray {
public:
	using vec3 = glm::vec3;
	vec3 dir;
	vec3 pos;
	vec3 invDir;
	Ray() {};
	Ray(const vec3& _pos, const vec3& _dir) : dir(glm::normalize(_dir)), pos(_pos) {
		invDir = vec3(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z);
	}


};

#endif

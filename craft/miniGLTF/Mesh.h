#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>
#include <vector>
#include "Texture.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Mesh
{
public:
	std::vector <glm::vec3> positions;
	std::vector <glm::vec3> normals;
	std::vector <glm::vec2> texUVs;
	std::vector <GLuint> indices;
	std::vector <Texture> textures;
	// Initializes the mesh
	// Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);
	Mesh(
        std::vector<glm::vec3> pos,
        std::vector<glm::vec3> uvs,
        std::vector<glm::vec2> tex_uvs,
        std::vector <GLuint>& inds, 
        std::vector <Texture>& texs
    );

};
#endif
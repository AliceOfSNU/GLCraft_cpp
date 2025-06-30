#include "Mesh.h"

// Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
// {
// 	Mesh::vertices = vertices;
// 	Mesh::indices = indices;
// 	Mesh::textures = textures;

// 	VAO.Bind();
// 	// Generates Vertex Buffer Object and links it to vertices
// 	VBO VBO(vertices);
// 	// Generates Element Buffer Object and links it to indices
// 	EBO EBO(indices);
// 	// Links VBO attributes such as coordinates and colors to VAO
// 	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
// 	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
// 	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
// 	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
// 	// Unbind all to prevent accidentally modifying them
// 	VAO.Unbind();
// 	VBO.Unbind();
// 	EBO.Unbind();
// }

Mesh::Mesh(
    std::vector<glm::vec3> pos,
    std::vector<glm::vec3> uvs,
    std::vector<glm::vec2> tex_uvs,
    std::vector <GLuint>& inds, 
    std::vector <Texture>& texs
): positions(pos), normals(uvs), texUVs(tex_uvs), indices(inds), textures(texs){
}
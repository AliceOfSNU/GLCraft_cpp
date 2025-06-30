#include "Model.h"
#ifndef MODEL_H
#define MODEL_H
class ModelWrapper{
public:
    ModelWrapper() = default;
    ModelWrapper(std::string s);
    Model model;    // instance of miniGLTF::Model
    glm::mat4 modelMatrix; //global transformation
    /*
    this bakes all transformations to mesh's vertex data.
    that is, all the vertices are directly multiplied by meshes' transform matrices
    and the applied matrices are reset to identity.
    */
    void ApplyTransformsToMesh();  
};
#endif
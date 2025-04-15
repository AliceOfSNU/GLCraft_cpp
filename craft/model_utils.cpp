#include "model_utils.h"
// Model

ModelWrapper::ModelWrapper(std::string s): model(s.c_str()), modelMatrix(glm::mat4(1.0f)){}

void ModelWrapper::ApplyTransformsToMesh(){
	for(int i = 0; i < model.meshes.size(); ++i){
		glm::mat4& modelmat = modelMatrix*model.matricesMeshes[i];
		Mesh& mesh = model.meshes[i];
		std::transform(
			mesh.positions.begin(), 
			mesh.positions.end(), 
			mesh.positions.begin(), 
			[&modelmat](glm::vec3& vec){
				return glm::vec3(modelmat*glm::vec4(vec, 1.0f));
			}
		);
	}
}
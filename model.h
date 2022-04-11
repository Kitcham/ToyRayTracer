#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"
#include "BasePartList.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

class Model
{
public:
    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<shared_ptr<BasePart>>    meshes;
    string directory;
    glm::vec3 Color;
    int Matrial;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, int Matrial, glm::vec3 Color, bool gamma = false) : Matrial(Matrial),Color(Color),gammaCorrection(gamma)
    {   
        Creat(path);
    }

    // draws the model, and thus all its meshes
    void Draw()
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i]->Draw();
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void Creat(string const& path);
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    shared_ptr<BasePart> processMesh(aiMesh* mesh, const aiScene* scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};



#endif
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "SOIL2/SOIL2.h"
#include "Mesh.h"

inline GLint TextureFromFile(const char* path, const std::string& directory);

class Model
{
public:
    Model(const char* path) { loadModel(std::string(path)); }

    void Draw(const Shader& shader)
    {
        for (size_t i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    std::vector<Mesh>    meshes;
    std::string          directory;
    std::vector<Texture> textures_loaded;

    void loadModel(const std::string& path)
    {
        Assimp::Importer importer;
        // Las caras del OBJ son cuadrilateros -> Triangulate es indispensable.
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }

        size_t slash = path.find_last_of("/\\");
        directory = (slash == std::string::npos) ? "." : path.substr(0, slash);

        processNode(scene->mRootNode, scene);
        std::cout << "Modelo cargado: " << meshes.size() << " mallas, "
                  << textures_loaded.size() << " texturas" << std::endl;
    }

    void processNode(aiNode* node, const aiScene* scene)
    {
        for (GLuint i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (GLuint i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex>  vertices;
        std::vector<GLuint>  indices;
        std::vector<Texture> textures;
        MatProps             props;

        for (GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex v;
            v.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

            v.Normal = mesh->HasNormals()
                ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
                : glm::vec3(0.0f, 1.0f, 0.0f);

            v.TexCoords = mesh->mTextureCoords[0]
                ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f);

            vertices.push_back(v);
        }

        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // Colores y brillo del .mtl (Kd, Ks, Ns)
        aiColor3D c;
        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, c) == AI_SUCCESS)
            props.diffuse = glm::vec3(c.r, c.g, c.b);
        if (material->Get(AI_MATKEY_COLOR_SPECULAR, c) == AI_SUCCESS)
            props.specular = glm::vec3(c.r, c.g, c.b);
        float shin;
        if (material->Get(AI_MATKEY_SHININESS, shin) == AI_SUCCESS && shin > 1.0f)
            props.shininess = shin;

        // Solo textura difusa (map_Kd). Los map_Bump del .mtl no se usan:
        // los archivos *_bump.png no existen en la carpeta.
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        return Mesh(vertices, indices, textures, props);
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
    {
        std::vector<Texture> textures;

        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            bool skip = false;
            for (size_t j = 0; j < textures_loaded.size(); j++)
            {
                if (textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }

            if (!skip)
            {
                Texture texture;
                texture.id   = TextureFromFile(str.C_Str(), directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
    }
};

inline GLint TextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = directory + "/" + std::string(path);

    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

    if (!image)
    {
        std::cout << "ERROR::TEXTURA:: no se pudo cargar " << filename << std::endl;
        return textureID;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);   // texturas RGB con ancho no multiplo de 4 (seguridad)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // El .mtl dice "texturas repetibles": REPEAT es obligatorio
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}

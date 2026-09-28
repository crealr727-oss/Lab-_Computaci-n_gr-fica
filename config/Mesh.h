#pragma once
#include <string>
#include <vector>
#include <cstddef>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/types.h>
#include "Shader.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id;
    std::string type;
    aiString path;
};

// Propiedades del material (.mtl): Kd, Ks, Ns.
// Materiales sin textura (Metal, Vidrio, Pantalla, Lente) se pintan solo con Kd.
struct MatProps
{
    glm::vec3 diffuse   = glm::vec3(1.0f);
    glm::vec3 specular  = glm::vec3(0.0f);
    float     shininess = 32.0f;
};

class Mesh
{
public:
    std::vector<Vertex>  vertices;
    std::vector<GLuint>  indices;
    std::vector<Texture> textures;
    MatProps             material;

    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices,
         const std::vector<Texture>& textures, const MatProps& material)
    {
        this->vertices = vertices;
        this->indices  = indices;
        this->textures = textures;
        this->material = material;
        this->setupMesh();
    }

    void Draw(const Shader& shader)
    {
        GLuint prog = shader.Program;

        glUniform3f(glGetUniformLocation(prog, "material.diffuse"),
                    material.diffuse.r, material.diffuse.g, material.diffuse.b);
        glUniform3f(glGetUniformLocation(prog, "material.specular"),
                    material.specular.r, material.specular.g, material.specular.b);
        glUniform1f(glGetUniformLocation(prog, "material.shininess"), material.shininess);

        bool hasTexture = !textures.empty();
        glUniform1i(glGetUniformLocation(prog, "useTexture"), hasTexture ? 1 : 0);

        if (hasTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0].id);
            glUniform1i(glGetUniformLocation(prog, "texture_diffuse1"), 0);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        if (hasTexture) glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
    GLuint VAO, VBO, EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        // location 0: posicion
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Position));
        // location 1: normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        // location 2: coordenadas de textura
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
};

#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

class Shader
{
public:
    GLuint Program;

    Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
    {
        std::string vertexCode, fragmentCode;
        std::ifstream vFile(vertexPath), fFile(fragmentPath);

        if (!vFile.is_open() || !fFile.is_open())
        {
            std::cout << "ERROR::SHADER::No se pudo abrir el archivo:\n  "
                      << vertexPath << "\n  " << fragmentPath << std::endl;
            Program = 0;
            return;
        }

        std::stringstream vStream, fStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        vertexCode = vStream.str();
        fragmentCode = fStream.str();

        GLuint vertex   = compile(GL_VERTEX_SHADER,   vertexCode.c_str(),   "VERTEX");
        GLuint fragment = compile(GL_FRAGMENT_SHADER, fragmentCode.c_str(), "FRAGMENT");

        Program = glCreateProgram();
        glAttachShader(Program, vertex);
        glAttachShader(Program, fragment);
        glLinkProgram(Program);

        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(Program, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Use() const { glUseProgram(Program); }

private:
    GLuint compile(GLenum type, const char* code, const char* name)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &code, NULL);
        glCompileShader(shader);

        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER::" << name << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        return shader;
    }
};

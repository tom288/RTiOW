#ifndef SHADER_H
#define SHADER_H

#include <iostream>
using namespace std;
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>

// The relative path from the executable where the shaders are found
static const string FOLDER = "src/glsl/";

class Shader
{ public:

    // Assumes both files have the same names with the expected extensions
    Shader(const string& path) : Shader(path + ".vert", path + ".frag") { }

    // Constructor compiles and links shader source
    Shader(string vertPath, string fragPath)
    {
        string vertCode, fragCode;
        ifstream vShaderFile, fShaderFile;

        // Ensure ifstream objects can throw exceptions
        vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
        fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

        try
        {
            stringstream vShaderStream, fShaderStream;

            // Open files
            vShaderFile.open(FOLDER + vertPath);
            fShaderFile.open(FOLDER + fragPath);

            // Read buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // Close files
            vShaderFile.close();
            fShaderFile.close();

            // Convert streams into strings
            vertCode = vShaderStream.str();
            fragCode = fShaderStream.str();
        }
        catch (ifstream::failure &e)
        {
            cout << "ERROR: Error reading from shader files" << endl;
        }

        const char* vShaderCode = vertCode.c_str();
        const char* fShaderCode = fragCode.c_str();

        // Compile vertex shader
        GLuint vert = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert, 1, &vShaderCode, NULL);
        glCompileShader(vert);
        checkCompileErrors(vert, "VERTEX");

        // Compile fragment Shader
        GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag, 1, &fShaderCode, NULL);
        glCompileShader(frag);
        checkCompileErrors(frag, "FRAGMENT");

        // Create shader program
        id = glCreateProgram();
        glAttachShader(id, vert);
        glAttachShader(id, frag);
        glLinkProgram(id);
        checkCompileErrors(id, "PROGRAM");

        // The shaders are now linked to the program and so can be deleted
        glDeleteShader(vert);
        glDeleteShader(frag);
    }

    // Activates the shader
    void use() const
    {
        glUseProgram(id);
    }

    void setBool(const string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
    }

    void setInt(const string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(id, name.c_str()), value);
    }

    void setFloat(const string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(id, name.c_str()), value);
    }

    void setVec2(const string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void setVec2(const string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
    }

    void setVec3(const string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void setVec3(const string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
    }

    void setVec4(const string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
    }

    void setVec4(const string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
    }

    void setMat2(const string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat3(const string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:

    // Shader object reference
    GLuint id;

    // Checks for shader compilation and linking errors
    void checkCompileErrors(GLuint shader, string type) const
    {
        GLint ok;
        GLchar log[1024];

        if (type == "PROGRAM")
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &ok);
            if (!ok)
            {
                glGetProgramInfoLog(shader, 1024, NULL, log);
                cout << "ERROR: Shader program linking error";
            }
        }
        else
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
            if (!ok)
            {
                glGetShaderInfoLog(shader, 1024, NULL, log);
                cout << "ERROR: Compilation error for shader type " << type;
            }
        }

        if (!ok) cout << "\n" << log << endl;
    }
};

#endif

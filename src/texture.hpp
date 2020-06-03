#ifndef TEXTURE_H_
#define TEXTURE_H_

#define GLEW_STATIC
#include <GL/glew.h>

static const float vertices[] =
{
    // Positions    // Tex coords
    -1.0f, -1.0f,    0.0f,  0.0f, // Bottom left
    -1.0f,  1.0f,    0.0f,  1.0f, // Top left
     1.0f,  1.0f,    1.0f,  1.0f, // Top right
     1.0f, -1.0f,    1.0f,  0.0f  // Bottom right
};

static const unsigned int indices[] =
{
    0, 2, 1, // Triangle 1
    0, 3, 2  // Triangle 2
};

class Texture
{ public:

    Texture()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Positions
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinates
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    // Destructor
    ~Texture()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    // Replace pixel values
    void fill(GLsizei width, GLsizei height, GLubyte* pixels)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Prepare for use
    void bind()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
    }

    // Display
    void draw()
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

private:
    unsigned int tex, vbo, vao, ebo;
};

#endif

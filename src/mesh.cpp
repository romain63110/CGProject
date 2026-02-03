#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include "../include/mesh.h"
#include <iostream>

unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width = 0, height = 0, channels = 0;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);

    if (!data)
    {
        std::cerr << "[TEXTURE ERROR] Failed to load: " << path << std::endl;
        return 0;
    }

    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat = GL_RGB;

    if (channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }
    else if (channels == 1)
    {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
    }

    std::cout << "[DEBUG] mesh.cpp USING FIXED glTexImage2D" << std::endl;

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        width,
        height,
        0,
        dataFormat,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

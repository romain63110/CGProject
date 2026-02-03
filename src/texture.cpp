#include "texture.h"
#include <iostream>
#include <stdexcept>
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void checkGLError(const std::string& where)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "[OPENGL ERROR] " << where << " : " << err << std::endl;
    }
}

/* =========================================================
   2D TEXTURE
   ========================================================= */
Texture::Texture(const std::string& tex_file,
    GLenum wrap_mode,
    GLenum min_filter,
    GLenum mag_filter)
    : glid_(0), target_(GL_TEXTURE_2D)
{
    std::cout << "[TEXTURE] Loading " << tex_file << std::endl;

    glGenTextures(1, &glid_);
    glBindTexture(GL_TEXTURE_2D, glid_);

    stbi_set_flip_vertically_on_load(true);

    int width = 0, height = 0, channels = 0;
    unsigned char* data = stbi_load(tex_file.c_str(), &width, &height, &channels, 0);

    if (!data)
        throw std::runtime_error("Texture load failed: " + tex_file);

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

    checkGLError("glTexImage2D");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    std::cout << "[TEXTURE GPU] ID=" << glid_ << " uploaded" << std::endl;
}

/* =========================================================
   CUBEMAP (SKYBOX)
   ========================================================= */
Texture::Texture(const std::vector<std::string>& faces,
    GLenum wrap_mode,
    GLenum min_filter,
    GLenum mag_filter)
    : glid_(0), target_(GL_TEXTURE_CUBE_MAP)
{
    std::cout << "[CUBEMAP] Loading cubemap..." << std::endl;

    glGenTextures(1, &glid_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glid_);

    stbi_set_flip_vertically_on_load(false);

    int width = 0, height = 0, channels = 0;

    for (size_t i = 0; i < faces.size(); ++i)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

        if (!data)
            throw std::runtime_error("Cubemap load failed: " + faces[i]);

        GLenum internalFormat = (channels == 4) ? GL_RGBA8 : GL_RGB8;
        GLenum dataFormat = (channels == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            internalFormat,
            width,
            height,
            0,
            dataFormat,
            GL_UNSIGNED_BYTE,
            data
        );

        stbi_image_free(data);
        std::cout << "[CUBEMAP OK] " << faces[i] << std::endl;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap_mode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap_mode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap_mode);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_filter);
}

/* ========================================================= */

Texture::~Texture()
{
    if (glid_ != 0)
        glDeleteTextures(1, &glid_);
}

void Texture::bind(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(target_, glid_);
}

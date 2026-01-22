#include "texture.h"
#include <iostream>
#include <memory>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::string& tex_file, GLenum wrap_mode, GLenum min_filter, GLenum mag_filter)
    : glid_(0), target_(GL_TEXTURE_2D)
{
    glGenTextures(1, &glid_);
    glBindTexture(GL_TEXTURE_2D, glid_);

    // set the origin at the bottom
    stbi_set_flip_vertically_on_load(true);

    // load texture image
    int width, height, num_channels;
    unsigned char* data = stbi_load(tex_file.c_str(), &width, &height, &num_channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture from file: " << tex_file << std::endl;
        throw std::runtime_error("Failed to load texture from file");
    }

    GLenum format;
    if (num_channels == 1) {
        format = GL_RED;
    } else if (num_channels == 3) {
        format = GL_RGB;
    } else if (num_channels == 4) {
        format = GL_RGBA;
    } else {
        std::cerr << "Unknown texture format: " << num_channels << " channels" << std::endl;
        throw std::runtime_error("Unknown texture format");
    }

    // upload texture to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

    // generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // free image data
    stbi_image_free(data);

    std::cout << "Loaded texture " << tex_file << " (" << width << "x" << height << ", " 
              << wrap_mode << ", " << min_filter << ", " << mag_filter << ")" << std::endl;
}

Texture::Texture(const std::vector<std::string>& faces, GLenum wrap_mode, GLenum min_filter, GLenum mag_filter)
    : glid_(0), target_(GL_TEXTURE_CUBE_MAP)
{
    glGenTextures(1, &glid_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, glid_);

    // doesn't set the origin at the bottom
    stbi_set_flip_vertically_on_load(false); //?

    int width, height, num_channels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &num_channels, 0);

        if (!data) {
            std::cerr << "Failed to load texture from file: " << faces[i] << std::endl;
            throw std::runtime_error("Failed to load texture from file");
            stbi_image_free(data);
        }

        GLenum format;
        if (num_channels == 1) {
            format = GL_RED;
        }
        else if (num_channels == 3) {
            format = GL_RGB;
        }
        else if (num_channels == 4) {
            format = GL_RGBA;
        }
        else {
            std::cerr << "Unknown texture format: " << num_channels << " channels" << std::endl;
            throw std::runtime_error("Unknown texture format");
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);

        std::cout << "Loaded texture " << faces[i] << " (" << width << "x" << height << ", "
            << wrap_mode << ", " << min_filter << ", " << mag_filter << ")" << std::endl;
        
    }

    // set texture options
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, mag_filter);
}

Texture::~Texture() {
    glDeleteTextures(1, &glid_);
}

void Texture::bind(int unit) {
    glActiveTexture(GL_TEXTURE0 + unit); // Enable slot [unit]
    glBindTexture(target_, glid_);       // Bind tex to the slot [unit]
}
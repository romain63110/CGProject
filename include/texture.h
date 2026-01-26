#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>

class Texture {
public:
    Texture(const std::string& tex_file, GLenum wrap_mode = GL_REPEAT,
            GLenum min_filter = GL_LINEAR, GLenum mag_filter = GL_LINEAR_MIPMAP_LINEAR);
    Texture(const std::vector<std::string>& faces, GLenum wrap_mode = GL_REPEAT,
        GLenum min_filter = GL_LINEAR, GLenum mag_filter = GL_LINEAR_MIPMAP_LINEAR); //skyBox
    ~Texture();

    GLuint getGLid() const { return glid_; }
    GLenum getTarget() const { return target_; }
    void bind(int unit);

private:
    GLuint glid_;
    GLenum target_;
};


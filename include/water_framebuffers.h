#ifndef WATER_FRAMEBUFFERS_H
#define WATER_FRAMEBUFFERS_H

#include <GL/glew.h>

class WaterFrameBuffers {
public:
    WaterFrameBuffers();
    ~WaterFrameBuffers();

    void bindReflectionFrameBuffer();
    void bindRefractionFrameBuffer();
    void unbindCurrentFrameBuffer();

    GLuint getReflectionTexture() { return reflectionTexture; }
    GLuint getRefractionTexture() { return refractionTexture; }

private:
    void initialiseReflectionFrameBuffer();
    void initialiseRefractionFrameBuffer();
    void bindFrameBuffer(int frameBuffer, int width, int height);
    GLuint createTextureAttachment(int width, int height);
    GLuint createDepthTextureAttachment(int width, int height);
    GLuint createDepthBufferAttachment(int width, int height);

    // Résolution des textures (peut être différente de l'écran)
    const int REFLECTION_WIDTH = 1280;
    const int REFLECTION_HEIGHT = 720;
    const int REFRACTION_WIDTH = 1280;
    const int REFRACTION_HEIGHT = 720;

    GLuint reflectionFrameBuffer;
    GLuint reflectionTexture;
    GLuint reflectionDepthBuffer;

    GLuint refractionFrameBuffer;
    GLuint refractionTexture;
    GLuint refractionDepthTexture;
};

#endif
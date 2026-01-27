#include "water_framebuffers.h"
#include <iostream>

WaterFrameBuffers::WaterFrameBuffers() {
    initialiseReflectionFrameBuffer();
    initialiseRefractionFrameBuffer();
}

WaterFrameBuffers::~WaterFrameBuffers() {
    glDeleteFramebuffers(1, &reflectionFrameBuffer);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteRenderbuffers(1, &reflectionDepthBuffer);
    glDeleteFramebuffers(1, &refractionFrameBuffer);
    glDeleteTextures(1, &refractionTexture);
    glDeleteTextures(1, &refractionDepthTexture);
}

void WaterFrameBuffers::bindReflectionFrameBuffer() {
    bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}

void WaterFrameBuffers::bindRefractionFrameBuffer() {
    bindFrameBuffer(refractionTexture, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void WaterFrameBuffers::unbindCurrentFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1024, 768);
}

void WaterFrameBuffers::initialiseReflectionFrameBuffer() {
    reflectionFrameBuffer = 0;
    glGenFramebuffers(1, &reflectionFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
    reflectionTexture = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Erreur Framebuffer Reflection!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterFrameBuffers::initialiseRefractionFrameBuffer() {
    refractionFrameBuffer = 0;
    glGenFramebuffers(1, &refractionFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
    refractionTexture = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Erreur Framebuffer Refraction!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterFrameBuffers::bindFrameBuffer(int frameBuffer, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glViewport(0, 0, width, height);
}

GLuint WaterFrameBuffers::createTextureAttachment(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGB,
        width, height,
        0,
        GL_RGB, GL_UNSIGNED_BYTE,
        nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        texture,
        0
    );
    return texture;
}

GLuint WaterFrameBuffers::createDepthTextureAttachment(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
    return texture;
}

GLuint WaterFrameBuffers::createDepthBufferAttachment(int width, int height) {
    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    return depthBuffer;
}
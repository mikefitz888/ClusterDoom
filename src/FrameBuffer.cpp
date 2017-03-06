#include "../include/FrameBuffer.h"

// < See docs in header >

FrameBuffer::FrameBuffer(int width, int height, graphics::RenderManager* render_manager) {
    this->width             = width;
    this->height         = height;
    this->render_manager = render_manager;

    generateFrameBuffer();
}

// Set framebuffer as active render target
void FrameBuffer::setTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer_object_id);
}

// Reset to default context' render target
void FrameBuffer::resetTarget() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Bind framebuffer as render target
void FrameBuffer::bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->render_texture);
}

// Get framebuffer openGL texture resource
GLuint FrameBuffer::getTexture() {
    return this->render_texture;
}

// Clear framebuffer
void FrameBuffer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


// Generate framebuffer
bool FrameBuffer::generateFrameBuffer() {

    // Create framebuffer object
    glGenFramebuffers(1, &this->frame_buffer_object_id);

    // Create Diffuse texture
    glGenTextures(1, &this->render_texture);
    glBindTexture(GL_TEXTURE_2D, this->render_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Create Depth texture
    glGenTextures(1, &this->depth_texture);
    glBindTexture(GL_TEXTURE_2D, this->depth_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Bind FBO so we can modify its properties
    glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer_object_id);

    // attach a texture to FBO colour attachment
    glBindTexture(GL_TEXTURE_2D, this->render_texture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->render_texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach a renderbuffer to depth attachment point
    glBindTexture(GL_TEXTURE_2D, this->depth_texture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_texture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Check framebuffer status:
    /*
        We need to verify that the framebuffer has created successfully.
    */
    GLenum ret = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);
    if (ret != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "FRAMEBUFFER FAILED TO CREATE" << std::endl;
    } else {
        std::cout << "FRAMEBUFFER CREATED SUCCESSFULLY!" << std::endl;
    }

    // Reset binds
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

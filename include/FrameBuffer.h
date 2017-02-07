#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

/*
    This class handles the creation of "FrameBuffers". These are OpenGL constructs for off-screen rendering
    surfaces that can be used in the render process.
    They are useful for special effects such as lighting/fog of war that do not use convential rendering
    methods.

    - A framebuffer acts in the same way as other textures except it contains a rendertexture that allows
      you to dynamically modify the contents of that texture.
      It can then be bound in the same way normal .
    - Drawing to a framebuffer is the same as regular drawing, except the contents will be rendered
      to the offscreen surface.

    Functions:
        FrameBuffer( int width, int height );
        setTarget();            // Set render target to this framebuffer
        resetTarget();            // Reset render target to default window framebuffer
        bind();                    // bind the framebuffer as a texture (This uses the framebuffer for rendering)

*/

#include "gamecore.h"

class FrameBuffer {
private:
    int width, height;
    GLuint           frame_buffer_object_id;
    GLuint           render_texture;
    GLuint           depth_texture;
    graphics::RenderManager* render_manager;

    bool generateFrameBuffer(); // Generate frame buffer
public:
    FrameBuffer(int width, int height, graphics::RenderManager* renderManager);
    void setTarget();
    void resetTarget();
    void bind();
    void clear(float r, float g, float b, float a);
    GLuint getTexture();
};

#endif

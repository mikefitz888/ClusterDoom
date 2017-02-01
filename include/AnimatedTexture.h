#ifndef _ANIMATED_TEXTURE_H
#define _ANIMATED_TEXTURE_H

#include "gamecore.h"

/*
    ------------------------------------------------------------------------------------
    Animated textures are a combination of an array of simple vertex buffers and
    a sprite sheet.

    ------------------------------------------------------------------------------------
    Sprite sheets should have equally spaced out tiles, and the dimensions of the
    image will be used to determine frames.
    - A sprite sheet can be split horizontally, or both horizontally and vertically.
    (This will assume row-major sprite order).

    Functions:

        AnimatedTexture(sf::String filename, bool split_both_ways, int frame_count )
            Imports and creates an animated texture resource with the given image.

            - filename        :: Filename to the repeating texture we want to use
            - split_both_ways :: set to true if you want both vertical and horizontal splitting (horizontal only if false)
            - frame_count_h   :: Number of frames to split the image into along the horizontal
            - frame_count_v   :: Number of frames to split the image into along the vertical
            - total_frames    :: The total number of frames (set to -1 for frame_count_h*frame_count_v), this is to allow empty frames on the last row.

        void render(int frame)
            Renders a quad with the given frame. This function also binds the texture associated with this AnimatedTexture.
            (Note: each frame in the animation has its own quad)

            - frame :: Specifies the integer index of the frame we want to animate. This number is applied against the modulo of the total frame count.

        int getTotalFrames()
            Returns the number of frame associated with this image
*/
//#include <SFML/Window.hpp>
//#include <SFML/Graphics.hpp>
//#include <GL/glew.h>
//#include <SFML/OpenGL.hpp>
//#include <SFML/Main.hpp>

#include "VertexBuffer.h"
//#include <vector>
#include "manager.h"

namespace graphics {
    class AnimatedTexture {

    private:
        RenderManager               *render_manager;
        sf::Texture                   *animated_texture_source;
        std::vector<VertexBuffer*>  vertex_buffers;

        int frame_count = 0;

        void release();

    public:
        AnimatedTexture(sf::String filename, bool split_both_ways, int frame_count_h, int frame_count_v, int total_frames);
        void render(int frame);
        int getTotalFrames();
        ~AnimatedTexture();

    };
}










#endif

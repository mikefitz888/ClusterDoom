#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include "gamecore.h"

#include <SFML/Graphics.hpp>
#include <GL/glew.h>

namespace graphics {
    struct Vec2 {
        float x, y;
        Vec2();
        Vec2(float x, float y);
    };

    struct Vec3 {
        float x, y, z;
        Vec3();
        Vec3(float x, float y, float z);
    };

    struct Vec4 {
        float x, y, z, w;
        Vec4();
        Vec4(float x, float y, float z, float w);
    };

    class Vertex {
    private:
        // Vertex data
        float xyzw[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float rgba[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float uv[2] = { 0.0f, 0.0f };
        float nml[3] = { 0.0f, 0.0f, 0.0f };

    public:
        /// Data alignment information:
        // The amount of bytes an element has
        static const int elementBytes = 4;

        // Elements per parameter
        static const int positionElementCount = 4;
        static const int colorElementCount = 4;
        static const int textureElementCount = 2;
        static const int normalElementCount = 3;

        // Bytes per parameter
        static const int positionBytesCount = positionElementCount * elementBytes;
        static const int colorByteCount = colorElementCount * elementBytes;
        static const int textureByteCount = textureElementCount * elementBytes;
        static const int normalByteCount = normalElementCount * elementBytes;

        // Byte offsets per parameter
        static const int positionByteOffset = 0;
        static const int colorByteOffset = positionByteOffset + positionBytesCount;
        static const int textureByteOffset = colorByteOffset + colorByteCount;
        static const int normalByteOffset = textureByteOffset + textureByteCount;

        // The amount of elements that a vertex has
        static const int elementCount = positionElementCount + colorElementCount + textureElementCount + normalElementCount;

        // The size of a vertex in bytes, like in C/C++: sizeof(Vertex)
        static const int stride = positionBytesCount + colorByteCount + textureByteCount + normalByteCount;

        Vertex();
        Vertex(float x, float y, float z, float w, float u, float v, float r, float g, float b, float a, float nx, float ny, float nz);
        void setXYZ(float x, float y, float z);
        void setXYZW(float x, float y, float z, float w);
        void setUV(float u, float v);
        void setRGBA(float r, float g, float b, float a);
        void setNormal(float nx, float ny, float nz);
        Vec3 getXYZ();
        Vec4 getXYZW();
        Vec4 getRGBA();
        Vec3 getNormal();
        Vec2 getUV();
    };

    class VertexBuffer {
        GLuint vertex_array_object_id, vertex_buffer_positions, vertex_buffer_colours, vertex_buffer_uvs, vertex_buffer_norms;
        bool is_frozen;
        int size, max_size;
        Vertex *vertices;

        public:
            VertexBuffer();
            ~VertexBuffer();
            void freeze();
            void render();
            void renderImmediate() const;
            void addVertex(Vertex v);
            void addVertex(float x, float y, float z, float r, float g, float b, float a, float u, float v, float nx, float ny, float nz);
            void addFloor(float x1, float y1, float x2, float y2, float z);
            void addQuad(float x1, float y1, float x2, float y2);
            void addQuadRGBA(float x1, float y1, float x2, float y2, float r, float g, float b, float a);
            void addQuadExt(float x1, float y1, float x2, float y2, float tx1, float ty1, float tx2, float ty2);

        private:
            void bindBuffers() const;
            void unbindBuffers();
            void enableAttributes();
            void disableAttributes();
            void release();
            void resize();
    };
}


#endif
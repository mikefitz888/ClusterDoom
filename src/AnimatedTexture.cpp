#include "../include/AnimatedTexture.h"
#include <iostream>

namespace graphics {
	// Create a new animated texture
	AnimatedTexture::AnimatedTexture(sf::String filename, bool split_both_ways, int frame_count_h, int frame_count_v, int total_frames) {

		// Set frame count
		if (total_frames < 1) {
			total_frames = frame_count_h*frame_count_v;
		}
		this->frame_count = total_frames;

		// Check for errors
		if (frame_count < 1) {
			std::cout << "Invalid frame_count of " << frame_count << std::endl;
			// TODO: Exception?
		}

		// Load in texture
		animated_texture_source = new sf::Texture();
		if (!animated_texture_source->loadFromFile(filename)) {
			// Texture has failed to load
			std::cout << "[ERROR] ANIMATED TEXTURE' " << filename.toAnsiString() << "' FAILED TO LOAD!" << std::endl;
			// TODO: Exception?
		}

		// Determine width and height and split size:
		int width = animated_texture_source->getSize().x;
		int height = animated_texture_source->getSize().y;

		int frame_width, frame_height;
		if (split_both_ways) {

			frame_width = width / frame_count_h;
			frame_height = height / frame_count_v;

		}
		else {

			frame_width = width / frame_count_h;
			frame_height = height;

		}

		// Create vertex buffers
		int hframe_id = 0;
		int vframe_id = 0;

		for (int v = 0; v < this->frame_count; v++) {

			// Determine uv coordinates of frame
			float u1, v1, u2, v2;
			u1 = (float)(hframe_id*frame_width) / (float)width;
			v1 = (float)(vframe_id*frame_height) / (float)height;
			u2 = (float)((hframe_id + 1)*frame_width) / (float)width;
			v2 = (float)((vframe_id + 1)*frame_height) / (float)height;

			// Create vertex buffer
			VertexBuffer *vbuff = new VertexBuffer();
			vbuff->addQuadExt(0, 0, frame_width, frame_height, u1, v1, u2, v2);
			vbuff->freeze();

			// Add vertex buffer to array
			vertex_buffers.push_back(vbuff);

			// Increment counters;
			hframe_id++;
			if (hframe_id >= frame_count_h) {
				hframe_id = 0;
				vframe_id++;
			}

			std::cout << "ANIMATED TEXTURE FRAME " << v << ": UV(" << u1 << "," << v1 << ") to (" << u2 << "," << v2 << ")" << std::endl;
		}

	}

	// Render a specified frame of the animated texture.
	void AnimatedTexture::render(int frame) {
		int frame_id = frame % this->frame_count;
		VertexBuffer *vbuff = vertex_buffers[frame_id];

		this->render_manager->setTexture(this->animated_texture_source);
		vbuff->render();
	}

	// Get total frames
	int AnimatedTexture::getTotalFrames() {
		return this->frame_count;
	}
}
#include "../include/FrameBuffer.h"

// < See docs in header >

FrameBuffer::FrameBuffer(int width, int height, RenderManager* render_manager) {
	this->width			 = width;
	this->height		 = height;
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
	glBindTexture(GL_TEXTURE_2D, this->render_texture);
}

// Get framebuffer openGL texture resource
GLuint FrameBuffer::getTexture() {
	return this->render_texture;
}

// Generate framebuffer
bool FrameBuffer::generateFrameBuffer() {

	// Create framebuffer object
	glGenFramebuffers(1, &frame_buffer_object_id);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object_id);

	// Create RenderBuffer object to store depth info
	glGenRenderbuffers(1, &render_buffer_object_depth_id);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object_depth_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Create texture
	glGenTextures(1, &render_texture);
	glBindTexture(GL_TEXTURE_2D, render_texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach a texture to FBO colour attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0);

	// attach a renderbuffer to depth attachment point
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object_depth_id);

	// Check framebuffer status:
	/*
		We need to verify that the framebuffer has created successfully.
	*/

	// Reset binds
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

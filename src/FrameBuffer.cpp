#include "../include/FrameBuffer.h"

// < See docs in header >

FrameBuffer::FrameBuffer(int width, int height, RenderManager* renderManager) {
	this->width			= width;
	this->height		= height;
	this->renderManager = renderManager;

	generateFrameBuffer();
}

// Set framebuffer as active render target
void FrameBuffer::setTarget() {
	glBindFramebuffer(GL_FRAMEBUFFER, this->frameBufferID);
}

// Reset to default context' render target
void FrameBuffer::resetTarget() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Bind framebuffer as render target
void FrameBuffer::bind() {

}

// Generate framebuffer
bool FrameBuffer::generateFrameBuffer() {

}

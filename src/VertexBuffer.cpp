#include "VertexBuffer.h"
#include <iostream>

namespace graphics {

	/////////////// ------------------------------ VERTEX BUFFER CLASS METHODS ------------------------- //////////////////
	VertexBuffer::VertexBuffer() {
		// Allocate a new VBO (Vertex Buffer Object) and Vertex Array in openGL and store their id's
		//glGenBuffers(1, &vertex_buffer_object_id);
		//glGenVertexArrays(1, &vertex_array_object_id);
		max_size = 128;
		vertices = new Vertex[max_size];
		size = 0;

	}

	VertexBuffer::~VertexBuffer() {
		release();
		delete[] vertices;
	}

	// Freeze
	/*
		This function freezes the geometry in the model. This process involves binding the dynamic
		vertex buffer data to the allocated openGL structures on the GPU.

		This should be called once the Vertex Buffer needs to be rendered.
	*/
	void VertexBuffer::freeze() {

		// tmp convert to glfloat array as its not working ;x
		/*
			For some reason, using the singular vertex array didn't work, so i've opted to instead
			go for the split buffer model. All get combined into a single vertex array object anyway, so it
			makes no difference for rendering.
		*/
		GLfloat *positions = new GLfloat[size * 4];
		GLfloat *colours = new GLfloat[size * 4];
		GLfloat *uvs = new GLfloat[size * 2];
		GLfloat *normals = new GLfloat[size * 3];
		for (int i = 0; i < size; i++) {
			Vertex v = vertices[i];
			Vec4 xyzw, rgba;
			xyzw = v.getXYZW();
			rgba = v.getRGBA();
			Vec2 uv = v.getUV();
			Vec3 nml = v.getNormal();
			positions[i * 4 + 0] = xyzw.x;
			positions[i * 4 + 1] = xyzw.y;
			positions[i * 4 + 2] = xyzw.z;
			positions[i * 4 + 3] = xyzw.w;
			colours[i * 4 + 0] = rgba.x;
			colours[i * 4 + 1] = rgba.y;
			colours[i * 4 + 2] = rgba.z;
			colours[i * 4 + 3] = rgba.w;
			uvs[i * 2 + 0] = uv.x;
			uvs[i * 2 + 1] = uv.y;
			normals[i * 3 + 0] = nml.x;
			normals[i * 3 + 1] = nml.y;
			normals[i * 3 + 2] = nml.z;
		}

		// Upload buffer data
		glGenBuffers(1, &vertex_buffer_positions);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_positions);
		glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLfloat) * 4, positions, GL_STATIC_DRAW);
		delete[] positions;

		glGenBuffers(1, &vertex_buffer_colours);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_colours);
		glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLfloat) * 4, colours, GL_STATIC_DRAW);
		delete[] colours;

		glGenBuffers(1, &vertex_buffer_uvs);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_uvs);
		glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLfloat) * 2, uvs, GL_STATIC_DRAW);
		delete[] uvs;

		glGenBuffers(1, &vertex_buffer_norms);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_norms);
		glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLfloat) * 3, normals, GL_STATIC_DRAW);
		delete[] normals;


		// Bind vertex attributes
		/*
			This allows us to clearly separate each group of data that makes up a vertex so that shaders
			can bind attribute locations correctly. Without this, accessing the different properties
			(Position, Colour, Normal, Texture coords) is impossible.
		*/
		glGenVertexArrays(1, &vertex_array_object_id);
		glBindVertexArray(vertex_array_object_id);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_positions);
		glVertexAttribPointer(0, Vertex::positionElementCount, GL_FLOAT, false, 0, NULL);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_colours);
		glVertexAttribPointer(1, Vertex::colorElementCount, GL_FLOAT, false, 0, NULL);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_uvs);
		glVertexAttribPointer(2, Vertex::textureElementCount, GL_FLOAT, false, 0, NULL);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_norms);
		glVertexAttribPointer(3, Vertex::normalElementCount, GL_FLOAT, false, 0, NULL);
		glEnableVertexAttribArray(3);

		// Mark the model as frozen (i.e. ready to render)
		is_frozen = true;

		// Check for errors
		std::cout << "Vertex Buffer Object constructed, vertex count: " << size << " data: " << (size*sizeof(Vertex)) << std::endl;
		for (GLenum err; (err = glGetError()) != GL_NO_ERROR;) {
			std::cout << "GLError: " << err << std::endl;
		}

		// Unbind
		unbindBuffers();
	}

	void VertexBuffer::bindBuffers() {
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_positions);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_colours);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_uvs);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_norms);
	}

	void VertexBuffer::unbindBuffers() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::enableAttributes() {
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
	}

	void VertexBuffer::disableAttributes() {
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

	}


	void VertexBuffer::release() {

		bindBuffers();
		glDeleteBuffers(1, &vertex_buffer_positions);
		glDeleteBuffers(1, &vertex_buffer_colours);
		glDeleteBuffers(1, &vertex_buffer_uvs);
		glDeleteBuffers(1, &vertex_buffer_norms);
		glDeleteVertexArrays(1, &vertex_array_object_id);
		unbindBuffers();
	}

	void VertexBuffer::render() {
		if (!is_frozen) { std::cout << "[VBO_ERROR] Vertex Buffer has not been frozen." << std::endl; return; }

		enableAttributes();
		bindBuffers();
		glBindVertexArray(vertex_array_object_id);
		glDrawArrays(GL_TRIANGLES, 0, size);
		glBindVertexArray(0);
		unbindBuffers();
		disableAttributes();
	}

	void VertexBuffer::renderImmediate() {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < size; i++) {
			Vec3 a = vertices[i].getXYZ();
			glVertex3f(a.x, a.y, a.z);

		}
		glEnd();
	}

	void VertexBuffer::resize() {
		if (is_frozen) { std::cout << "[VBO_ERROR] Vertex Buffer cannot be resize as it is frozen." << std::endl; return; }
		max_size *= 2;
		Vertex *_new_verts = new Vertex[max_size];
		memcpy(_new_verts, vertices, size);
		delete[] vertices;
		vertices = _new_verts;
	}


	void VertexBuffer::addVertex(Vertex v) {
		// Check to see if there is space left in the array
		if (size >= max_size) resize();
		vertices[size] = v;
		size++;
	}

	void VertexBuffer::addVertex(float x, float y, float z, float r, float g, float b, float a,
		float u, float v, float nx, float ny, float nz) {
		addVertex(Vertex(x, y, z, 1.0f, u, v, r, g, b, a, nx, ny, nz));
	}

	void VertexBuffer::addFloor(float x1, float y1, float x2, float y2, float z) {
		addVertex(x1, y2, z, 1, 1, 1, 1, 0, 1, 0, 0, 1);
		addVertex(x1, y1, z, 1, 1, 1, 1, 0, 0, 0, 0, 1);
		addVertex(x2, y1, z, 1, 1, 1, 1, 1, 0, 0, 0, 1);

		addVertex(x2, y2, z, 1, 1, 1, 1, 1, 1, 0, 0, 1);
		addVertex(x1, y2, z, 1, 1, 1, 1, 0, 1, 0, 0, 1);
		addVertex(x2, y1, z, 1, 1, 1, 1, 1, 0, 0, 0, 1);
	}

	void VertexBuffer::addQuad(float x1, float y1, float x2, float y2) {
		addVertex(x1, y2, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1);
		addVertex(x1, y1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1);
		addVertex(x2, y1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1);

		addVertex(x2, y2, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1);
		addVertex(x1, y2, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1);
		addVertex(x2, y1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1);
	}

	void VertexBuffer::addQuadExt(float x1, float y1, float x2, float y2, float tx1, float ty1, float tx2, float ty2) {
		addVertex(x1, y2, 0, 1, 1, 1, 1, tx1, ty2, 0, 0, 1);
		addVertex(x1, y1, 0, 1, 1, 1, 1, tx1, ty1, 0, 0, 1);
		addVertex(x2, y1, 0, 1, 1, 1, 1, tx2, ty1, 0, 0, 1);

		addVertex(x2, y2, 0, 1, 1, 1, 1, tx2, ty2, 0, 0, 1);
		addVertex(x1, y2, 0, 1, 1, 1, 1, tx1, ty2, 0, 0, 1);
		addVertex(x2, y1, 0, 1, 1, 1, 1, tx2, ty1, 0, 0, 1);
	}



	/////////////// ------------------------------ VERTEX CLASS METHODS ------------------------- //////////////////

	Vertex::Vertex() {
		setXYZW(0.0f, 0.0f, 0.0f, 0.0f);
		setRGBA(1.0f, 1.0f, 1.0f, 1.0f);
		setUV(0.0f, 0.0f);
		setNormal(0.0f, 0.0f, 0.0f);
	}
	Vertex::Vertex(float x, float y, float z, float w, float u, float v, float r, float g, float b, float a,
		float nx, float ny, float nz) {
		setXYZW(x, y, z, w);
		setRGBA(r, g, b, a);
		setUV(u, v);
		setNormal(nx, ny, nz);
	}
	// Set the (x, y, z) position of the vertex
	void Vertex::setXYZ(float x, float y, float z) {
		setXYZW(x, y, z, 1.0f);
	}

	// Set the (x, y, z, w) position of a vertex
	void Vertex::setXYZW(float x, float y, float z, float w) {
		xyzw[0] = x;
		xyzw[1] = y;
		xyzw[2] = z;
		xyzw[3] = w;
	}

	// Set the uv texture coordinates of a vertex
	void Vertex::setUV(float u, float v) {
		uv[0] = u;
		uv[1] = v;
	}

	// Set the rgba colour component of a vertex
	void Vertex::setRGBA(float r, float g, float b, float a) {
		rgba[0] = r;
		rgba[1] = g;
		rgba[2] = b;
		rgba[3] = a;
	}

	// Set the normal of the vertex
	void Vertex::setNormal(float nx, float ny, float nz) {
		nml[0] = nx;
		nml[1] = ny;
		nml[2] = nz;
	}

	// Get the xyz position
	Vec3 Vertex::getXYZ() {
		return Vec3(xyzw[0], xyzw[1], xyzw[2]);
	}

	// Get the xyz position
	Vec4 Vertex::getXYZW() {
		return Vec4(xyzw[0], xyzw[1], xyzw[2], xyzw[3]);
	}

	// Get the RGBA colour values
	Vec4 Vertex::getRGBA() {
		return Vec4(rgba[0], rgba[1], rgba[2], rgba[3]);
	}

	// Get the vertex normal values
	Vec3 Vertex::getNormal() {
		return Vec3(nml[0], nml[1], nml[2]);
	}

	// Get the UV texture coordinates
	Vec2 Vertex::getUV() {
		return Vec2(uv[0], uv[1]);
	}


	/////////////// ------------------------------ VECTOR CONTAINER METHODS ------------------------- //////////////////
	Vec2::Vec2() {}
	Vec2::Vec2(float x, float y) {
		this->x = x;
		this->y = y;
	}
	Vec3::Vec3() {}
	Vec3::Vec3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vec4::Vec4() {}
	Vec4::Vec4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
}
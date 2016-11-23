#ifndef _BUFFER_H
#define _BUFFER_H

#include <algorithm>
typedef unsigned char byte;

class Buffer {
private:
	int   max_size = 1024;
	int   head = 0;
	byte* buffer;

	inline void resize();


public:
	inline Buffer();
	inline void seek(int position);	// Set current head position (0 = base)
	inline int tell();					// Get current head position
	inline byte* getPtr();

	template <typename T> inline void write(T data);
	template <typename T> inline void read(T* ptr);
	template <typename T> inline void operator<<(T arg) {
		write(arg);
	}

	template <typename T> inline void operator>>(T &arg) {
		read(&arg);
	}
};

template <typename T> void Buffer::write(T data) {
	int size = sizeof(T);
	*((T*)(buffer + head)) = data;
	head += size;
	if (head > max_size) { resize(); }
}

template <typename T> void Buffer::read(T*  ptr) {
	byte* dataP = buffer + head;
	head += sizeof(T);
	if (head >= max_size) {
		// Check if reading head 
	}
	*ptr = *((T*)dataP);
}

Buffer::Buffer() {
	max_size = 1024;
	buffer = new byte[max_size];
	head = 0;
}

void Buffer::resize() {
	int old_size = max_size;
	max_size *= 2;
	byte* new_data = new byte[max_size];
	memcpy(new_data, buffer, old_size);
	delete[] buffer;
	buffer = new_data;
}
void Buffer::seek(int position) {
	head = position;
}

int Buffer::tell() {
	return head;
}

byte* Buffer::getPtr() {
	return buffer;
}

#endif
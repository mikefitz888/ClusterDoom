#ifndef _BUFFER_H
#define _BUFFER_H

#include "../gamecore.h"

/*
    Buffer
    ---------------

    A generic class designed to allow easy reading and writing of data. This is created as a system for handling packets and network data.
    The buffer will automatically resize if it overflows on writing.

    The buffer  works by allocating a block of memory, which is maintained by a writing head. Any atomic data (Fixed sized structs, literal types etc) can be written 
    to a buffer, along with null-terminated strings.

    This buffer is then passed into the packet functions, using the current writing position to dictate size.

    The writing/reading head is shared for both modes and can be moved using:
    - seek(position) : Jump to a specific byte location in the buffer
    - tell() : Returns the current byte reading location in the buffer.



*/

class Buffer {
private:
    int   max_size = 1024;
    int   head = 0;
    byte* buffer;

    inline void resize();


public:
    inline Buffer();
    inline void seek(int position);    // Set current head position (0 = base)
    inline int tell() const;                    // Get current head position
    inline byte* getPtr() const;
    inline int maxSize() const;

    template <typename T> inline void write(T data);
    inline void write_str(const char* string);

    template <typename T> inline void read(T* ptr);
    inline void read_str(char* string);
    inline void operator<<(const char* arg) {
        write_str(arg);
    }
    inline void operator<<(char* arg) {
        write_str(arg);
    }
    template <typename T> inline void operator<<(T arg) {
        write(arg);
    }

    inline void operator>>(char* &arg) {
        read_str(arg);
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

void Buffer::write_str(const char* string) {
    int size = strlen(string)+1;
    //*((char*)(buffer + head)) = *string;
    strcpy((char*)(buffer + head), string);
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

void Buffer::read_str(char* string) {
    const char* dataP = (char*)buffer + head;
    head += strlen((char*)dataP)+1;
    if (head >= max_size) {
        // Check if reading head
    }
    strcpy(string, dataP);
    //*string = *((char*)dataP);
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

int Buffer::tell() const {
    return head;
}

byte* Buffer::getPtr() const {
    return buffer;
}

int Buffer::maxSize() const {
    return max_size;
}

#endif

#pragma once

#include "gamecore.h"

#include <string>
#include <iostream>

// template<typname T> is intended to cover int/long/long long/float/double/long double and unsigned types where available
class String {
public:
    String();
    String(const char* ansi_string);
    String(const String& clone);
    String(const std::string& str);
    String(bool value);
    String(int val);
    String(unsigned val);
    String(long val);
    String(unsigned long val);
    String(long long val);
    String(unsigned long long val);
    String(float val);
    String(double val);
    String(long double val);

    operator std::string() const;
    String& operator =(const String& right);
    String& operator =(const char* right);
    String& operator =(bool right);

    template<typename T>
    String& operator =(T& right);


    String& operator +=(const String& right);
    String& operator +=(const char* right);
    String& operator +=(bool right);
    String& operator +=(int &other);
    String& operator +=(unsigned &other);
    String& operator +=(long &other);
    String& operator +=(unsigned long &other);
    String& operator +=(long long &other);
    String& operator +=(unsigned long long &other);
    String& operator +=(float &other);
    String& operator +=(double &other);
    String& operator +=(long double &other);

    String operator +(const String& other);
    String operator +(const char* other);
    String operator +(bool other);
    String operator +(int &val);
    String operator +(unsigned &val);
    String operator +(long &val);
    String operator +(unsigned long &val);
    String operator +(long long &val);
    String operator +(unsigned long long &val);
    String operator +(float &val);
    String operator +(double &val);
    String operator +(long double &val);

    char& operator [](std::size_t index);

    void clear();
    std::size_t getSize() const;
    bool isEmpty() const;

    void erase(std::size_t start, std::size_t length = 1);
    void insert(std::size_t position, const String& source);
    std::size_t find(const String& needle, std::size_t start = 0);

    void replace(std::size_t position, std::size_t length, const String& replacement);
    void replace(const String& needle, const String& replacement);

    String substring(std::size_t start, std::size_t length) const;

private:
    std::string dm_string;

    friend std::ostream& operator<<(std::ostream& os, const String& out);
};

/*
**    Implementation
*/

String::String() {}
String::String(const char* ansi_string) {
    if (ansi_string) {
        dm_string = ansi_string;
    }
}
String::String(const String& clone) : dm_string(clone.dm_string) {}
String::String(const std::string& str) : dm_string(str) {}
String::String(bool value) : dm_string(value ? "true" : "false") {}

String::String(int val) : dm_string(std::to_string(val)) {}
String::String(unsigned val) : dm_string(std::to_string(val)) {}
String::String(long val) : dm_string(std::to_string(val)) {}
String::String(unsigned long val) : dm_string(std::to_string(val)) {}
String::String(long long val) : dm_string(std::to_string(val)) {}
String::String(unsigned long long val) : dm_string(std::to_string(val)) {}
String::String(float val) : dm_string(std::to_string(val)) {}
String::String(double val) : dm_string(std::to_string(val)) {}
String::String(long double val) : dm_string(std::to_string(val)) {}

String::operator std::string() const { return dm_string; }

// OPERATOR =

String& String::operator =(const String& right)
{
    dm_string = right.dm_string;
    return *this;
}

String& String::operator =(const char* right) {
    if (right) {
        dm_string = right;
    }
    return *this;
}

String& String::operator =(bool right) {
    dm_string = right ? "true" : "false";
    return *this;
}

template<typename T>
String& String::operator =(T& right) {
    if (this != &right) {
        return String(right);
    }
    return *this;
}

// OPERATOR +=

String& String::operator +=(const String& right) { dm_string += right.dm_string; return *this; }
String& String::operator +=(const char* right) { dm_string += right;    return *this; }
String& String::operator +=(bool right) { dm_string += right ? "true" : "false"; return *this; }

String& String::operator +=(int &other) { dm_string += std::to_string(other); return *this; }
String& String::operator +=(unsigned &other) { dm_string += std::to_string(other); return *this; }
String& String::operator +=(long &other) { dm_string += std::to_string(other); return *this; }
String& String::operator +=(unsigned long &other) { dm_string += std::to_string(other); return *this; }
String& String::operator +=(long long &other) { dm_string += std::to_string(other); return *this; }
String& String::operator +=(unsigned long long &other) { dm_string += std::to_string(other); return *this; }
String& String::operator +=(float &other) { dm_string += std::to_string(other); return *this; }
String& String::operator +=(double &other) { dm_string += std::to_string(other); return *this; }
String& String::operator +=(long double &other) { dm_string += std::to_string(other); return *this; }

char& String::operator [](std::size_t index) {
    return dm_string.at(index);
}

// OPERATOR +

String String::operator+(const String& other) {return String(*this) += other;}
String String::operator+(const char* other) {return String(*this) += other;}
String String::operator+(bool other) { return String(*this) += other; }
String String::operator +(int &other) { return String(*this) += other; }
String String::operator +(unsigned &other) { return String(*this) += other; }
String String::operator +(long &other) { return String(*this) += other; }
String String::operator +(unsigned long &other) { return String(*this) += other; }
String String::operator +(long long &other) { return String(*this) += other; }
String String::operator +(unsigned long long &other) { return String(*this) += other; }
String String::operator +(float &other) { return String(*this) += other; }
String String::operator +(double &other) { return String(*this) += other; }
String String::operator +(long double &other) { return String(*this) += other; }


// METHODS

void String::clear() {
    dm_string.erase();
}

std::size_t String::getSize() const {
    return dm_string.length();
}

bool String::isEmpty() const {
    return dm_string.empty();
}

void String::erase(std::size_t start, std::size_t length) {
    dm_string.erase(start, length);
}

void String::insert(std::size_t position, const String& source) {
    dm_string.insert(position, source.dm_string);
}


std::size_t String::find(const String& needle, std::size_t start) {
    return dm_string.find(needle, start);
}

void String::replace(std::size_t position, std::size_t length, const String& replacement) {
    dm_string.replace(position, length, replacement.dm_string);
}

void String::replace(const String& needle, const String& replacement) {
    dm_string.replace(dm_string.find(needle), needle.getSize(), replacement.dm_string);
}

String String::substring(std::size_t start, std::size_t length) const {
    return String(dm_string.substr(start, length));
}

std::ostream& operator<<(std::ostream& os, const String& out)
{
    os << out.dm_string;
    return os;
}

std::string String::toString(){
    return dm_string;
}
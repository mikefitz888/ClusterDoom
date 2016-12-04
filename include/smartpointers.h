#ifndef SMARTPOINTERS_H
#define SMARTPOINTERS_H

#include <iostream>
#include <memory>
#include <exception>
#include <string>

#ifdef _MSC_VER
#define not !
#define and &&
#define or ||
#endif

namespace smartpointers {
    using std::cout;
    using std::endl;

    class InvalidatedSmartPointerException : public std::exception {

        std::string name = "smart";
        std::string hint = "object";
    public:
        InvalidatedSmartPointerException() : exception() {}
        InvalidatedSmartPointerException(std::string name, std::string hint) : exception() { this->name = name; this->hint = hint; }

        virtual const char* what() const throw() override {
            return ("A " + this->name + " pointer was dereferenced or allocated with a " + this->hint + " that is null").c_str();
        }
    };

    /**
    The idea behind the watch_ptr is that the raw pointer is kept in a set place
    then watch_ptrs look at that place to determine whether it is still valid.
    They are fundamentally vulnerable to another object entering the memory it
    was watching. Use with care, if in doubt use master/slave pointers.
    */
    template <typename T> class watch_ptr {
        template <typename Y> friend inline bool operator==(const watch_ptr<Y>& lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const watch_ptr<Y>& lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const watch_ptr<Y>& lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const watch_ptr<Y>& lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const watch_ptr<Y>& lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const watch_ptr<Y>& lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator==(const watch_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator==(std::nullptr_t lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const watch_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator!=(std::nullptr_t lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const watch_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator<(std::nullptr_t lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const watch_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator>(std::nullptr_t lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const watch_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator<=(std::nullptr_t lhs, const watch_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const watch_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator>=(std::nullptr_t lhs, const watch_ptr<Y>& rhs);

        T** payload;
        std::string name;

        inline T* get() const {
            if (this->valid()) return *this->payload;
            else throw InvalidatedSmartPointerException("watch", this->name);
        }

    public:
        watch_ptr(std::nullptr_t ptr) : name("object") {
            throw InvalidatedSmartPointerException("watch", this->name);
        }

        watch_ptr(std::nullptr_t ptr, std::string name) {
            throw InvalidatedSmartPointerException("watch", name);
        }

        watch_ptr(T** ptr) : name("object") {
            this->payload = ptr;
        }

        watch_ptr(T** ptr, std::string name): watch_ptr(ptr) {
            this->name = name;
        }

        watch_ptr(const watch_ptr& r) {
            this->payload = r.payload;
            this->name = r.name;
        }

        watch_ptr(watch_ptr&& r) noexcept {
            this->payload = r.payload;
            this->name = r.name;
            r.payload = nullptr;
        }

        inline bool valid() const {
            return *this->payload != nullptr;
        }

        inline watch_ptr<T>& operator=(const watch_ptr<T>& r) noexcept {
            this->payload = r.payload;
            this->name = r.name;
            return *this;
        }

        inline watch_ptr<T>& operator=(watch_ptr<T>&& r) noexcept {
            this->payload = r.payload;
            this->name = r.name;
            r.payload = nullptr;
            return *this;
        }

        inline T& operator*() const { return *get(); }
        inline T* operator->() const { return get(); }
        inline T& operator[](std::ptrdiff_t i) { return get()[i]; }

        inline explicit operator bool () const noexcept { return valid(); }
    };

    template <typename T> class slave_ptr;
    /**
    master_ptrs are pointers that are in sole control of an object of some
    type. They can be used like pointers if necessary, but they should be
    left alone to live in data structures. Use slave_ptrs to interact with
    their contents.
    */
    template <typename T> class master_ptr {
        template <typename Y> friend class slave_ptr;
        template <typename Y> friend inline bool operator==(const master_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const master_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const master_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const master_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const master_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const master_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator==(const master_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator==(std::nullptr_t lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const master_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator!=(std::nullptr_t lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const master_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator<(std::nullptr_t lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const master_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator>(std::nullptr_t lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const master_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator<=(std::nullptr_t lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const master_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator>=(std::nullptr_t lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator==(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator==(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename U, typename V> friend inline slave_ptr<U> static_pointer_cast(const master_ptr<V>& ptr) noexcept;
        template <typename U, typename V> friend inline slave_ptr<U> dynamic_pointer_cast(const master_ptr<V>& ptr) noexcept;
        template <typename U, typename V> friend inline slave_ptr<U> const_pointer_cast(const master_ptr<V>& ptr) noexcept;
        template <typename U, typename V> friend inline bool instanceof(const master_ptr<V> ptr) noexcept;

        T* payload;
        int* validity;
        std::string name;
        
        inline T* get() const {
            return this->payload;
        }

        inline void reset()
        {
            /*if (this->payload != nullptr)*/ delete this->payload;
            this->payload = nullptr;
            if (this->validity) {
                if (*this->validity == 1) delete this->validity;
                else {
                    *this->validity = -(*this->validity - 1);
                }
            }
        }

    public:
        master_ptr(std::nullptr_t ptr) : /*payload(ptr),*/ name("object") {
            //this->validity = new int(1);
            throw InvalidatedSmartPointerException("master", name);
        }

        master_ptr(std::nullptr_t ptr, std::string name)/* : payload(ptr)*/ {
            this->validity = new int(1);
            //throw InvalidatedSmartPointerException("master", name);
        }

        master_ptr(T* ptr) : payload(ptr), name("object") {
            this->validity = new int(1);
        }

        master_ptr(T* ptr, std::string name): master_ptr(ptr) {
            this->name = name;
        }

        master_ptr(master_ptr&& ptr) noexcept {
            this->payload = ptr.payload;
            this->name = ptr.name;
            this->validity = ptr.validity;
            ptr.payload = nullptr;
            ptr.validity = nullptr;
        }

        ~master_ptr() { reset(); }

        inline void invalidate() {
            reset();
        }

        inline master_ptr<T>& operator=(master_ptr<T>&& r) noexcept {
            this->reset();
            this->payload = r.payload;
            this->name = r.name;
            this->validity = r.validity;
            r.payload = nullptr;
            r.validity = nullptr;
            return *this;
        }

        inline T* operator->() const { return get(); }
        inline T& operator[](std::ptrdiff_t i) { return get()[i]; }

        inline explicit operator bool () const noexcept { return (bool) get(); }
    };

    /**
    slave_ptrs are pointers that share a resource with a master_ptr. The idea
    is that master_ptrs live in some sort of container and you pull out
    slave_ptrs to interact with the objects. They will throw an exception
    if you attempt to dereference them and their master is invalid.
    */
    template <typename T> class slave_ptr {
        template <typename Y> friend class slave_ptr;
        template <typename Y> friend inline bool operator==(const slave_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const slave_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const slave_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const slave_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const slave_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const slave_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator==(const slave_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator==(std::nullptr_t lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const slave_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator!=(std::nullptr_t lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const slave_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator<(std::nullptr_t lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const slave_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator>(std::nullptr_t lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const slave_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator<=(std::nullptr_t lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const slave_ptr<Y>& lhs, std::nullptr_t rhs);
        template <typename Y> friend inline bool operator>=(std::nullptr_t lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator==(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator==(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator!=(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator<=(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const master_ptr<Y>& lhs, const slave_ptr<Y>& rhs);
        template <typename Y> friend inline bool operator>=(const slave_ptr<Y>& lhs, const master_ptr<Y>& rhs);
        template <typename U, typename V> friend inline slave_ptr<U> static_pointer_cast(const slave_ptr<V>& ptr);
        template <typename U, typename V> friend inline slave_ptr<U> dynamic_pointer_cast(const slave_ptr<V>& ptr);
        template <typename U, typename V> friend inline slave_ptr<U> const_pointer_cast(const slave_ptr<V>& ptr);
        template <typename U, typename V> friend inline slave_ptr<U> static_pointer_cast(const master_ptr<V>& ptr) noexcept;
        template <typename U, typename V> friend inline slave_ptr<U> dynamic_pointer_cast(const master_ptr<V>& ptr) noexcept;
        template <typename U, typename V> friend inline slave_ptr<U> const_pointer_cast(const master_ptr<V>& ptr) noexcept;
        template <typename U, typename V> friend inline bool instanceof(const slave_ptr<V> ptr) noexcept;

        T* payload;
        std::string name;
        int* validity;

        inline T* get() const {
            if (this->valid()) return this->payload;
            else throw InvalidatedSmartPointerException("slave", this->name);
        }

        template <typename Y> explicit slave_ptr(const slave_ptr<Y>& ptr, T* payload) {
            this->payload = payload;
            this->name = ptr.name;
            this->validity = ptr.validity;
            if (*this->validity > 0) ++*this->validity;
            else --*this->validity;
        }

        template <typename Y> explicit slave_ptr(const master_ptr<Y>& ptr, T* payload) {
            this->payload = payload;
            this->name = ptr.name;
            this->validity = ptr.validity;
            if (*this->validity > 0) ++*this->validity;
            else --*this->validity;
            //else throw InvalidatedSmartPointerException("slave", name);
        }

    public:
        slave_ptr(std::nullptr_t ptr) : name("object") {
            //throw InvalidatedSmartPointerException("slave", this->name);
            this->payload = ptr;
            this->validity = nullptr;
        }

        slave_ptr(const master_ptr<T>& ptr) {
            this->payload = ptr.payload;
            this->name = ptr.name;
            this->validity = ptr.validity;
            if (*this->validity > 0) ++*this->validity;
            else --*this->validity;
            //else throw InvalidatedSmartPointerException("slave", this->name);
        }

        slave_ptr(const slave_ptr& ptr) {
            this->payload = ptr.payload;
            this->name = ptr.name;
            this->validity = ptr.validity;
            if (*this->validity > 0) ++*this->validity;
            else --*this->validity;
        }

        slave_ptr(slave_ptr&& ptr) noexcept {
            this->payload = ptr.payload;
            this->name = ptr.name;
            this->validity = ptr.validity;
            ptr.payload = nullptr;
            ptr.validity = nullptr;
        }

        ~slave_ptr() {
            // Something seems fishy here, surely we must deallocate if we are some -ve number?
            if (this->validity) {
                if (*this->validity == 1) delete this->validity;
                else if (*this->validity > 0) --*this->validity;
                else ++*this->validity;
            }
        }

        inline bool valid() const {
            return this->payload && this->validity && *this->validity > 0;
        }

        inline slave_ptr<T>& operator=(const slave_ptr<T>& r) noexcept {
            this->payload = r.payload;
            this->name = r.name;
            this->validity = r.validity;
            if (*this->validity > 0) ++*this->validity;
            else --*this->valid;
        }

        inline slave_ptr<T>& operator=(slave_ptr<T>&& r) noexcept {
            this->payload = r.payload;
            this->name = r.name;
            this->validity = r.validity;
            r.payload = nullptr;
            r.validity = nullptr;
            return *this;
        }

        inline T* operator->() const { return get(); }
        inline T& operator[](std::ptrdiff_t i) { return get()[i]; }

        inline explicit operator bool () const noexcept { return valid(); }
    };

    template <typename T, typename... Args> master_ptr<T> make_master(Args&&... args) {
        return master_ptr<T>(new T(args...));
    }

    template <typename T, typename U, typename... Args> master_ptr<T> make_master(Args&&... args) {
        return master_ptr<T>(static_cast<T*>(new U(args...)));
    }

    template <typename T, typename U, typename V> inline std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os, const watch_ptr<T>& ptr) {
        os << ptr.get();
    }

    template <typename T, typename U, typename V> inline std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os, const master_ptr<T>& ptr) {
        os << ptr.get();
    }

    template <typename T, typename U, typename V> inline std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os, const slave_ptr<T>& ptr) {
        os << ptr.get();
    }

    template <typename T> inline bool operator==(const watch_ptr<T>& lhs, const watch_ptr<T>& rhs)   { return lhs.payload == rhs.payload; }
    template <typename T> inline bool operator!=(const watch_ptr<T>& lhs, const watch_ptr<T>& rhs)   { return not (lhs == rhs); }
    template <typename T> inline bool operator<(const watch_ptr<T>& lhs, const watch_ptr<T>& rhs)    { return std::less<T*>()(lhs.payload, rhs.payload); }
    template <typename T> inline bool operator>(const watch_ptr<T>& lhs, const watch_ptr<T>& rhs)    { return rhs < lhs; }
    template <typename T> inline bool operator<=(const watch_ptr<T>& lhs, const watch_ptr<T>& rhs)   { return not (rhs < lhs); }
    template <typename T> inline bool operator>=(const watch_ptr<T>& lhs, const watch_ptr<T>& rhs)   { return not (lhs < rhs); }
    template <typename T> inline bool operator==(const watch_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return not lhs; }
    template <typename T> inline bool operator==(std::nullptr_t lhs, const watch_ptr<T>& rhs)        { (void) lhs; return not rhs; }
    template <typename T> inline bool operator!=(const watch_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return (bool) lhs; }
    template <typename T> inline bool operator!=(std::nullptr_t lhs, const watch_ptr<T>& rhs)        { (void) lhs; return (bool) rhs; }
    template <typename T> inline bool operator<(const watch_ptr<T>& lhs, std::nullptr_t rhs)         { return std::less<T*>()(lhs.payload, nullptr); }
    template <typename T> inline bool operator<(std::nullptr_t lhs, const watch_ptr<T>& rhs)         { return std::less<T*>()(nullptr, rhs.payload); }
    template <typename T> inline bool operator>(const watch_ptr<T>& lhs, std::nullptr_t rhs)         { (void) rhs; return nullptr < lhs; }
    template <typename T> inline bool operator>(std::nullptr_t lhs, const watch_ptr<T>& rhs)         { (void) lhs; return rhs < nullptr; }
    template <typename T> inline bool operator<=(const watch_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return not (nullptr < lhs); }
    template <typename T> inline bool operator<=(std::nullptr_t lhs, const watch_ptr<T>& rhs)        { (void) lhs; return not (rhs < nullptr); }
    template <typename T> inline bool operator>=(const watch_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return not (lhs < nullptr); }
    template <typename T> inline bool operator>=(std::nullptr_t lhs, const watch_ptr<T>& rhs)        { (void) lhs; return not (nullptr < rhs); }
    template <typename T> inline bool operator==(const master_ptr<T>& lhs, const master_ptr<T>& rhs) { return lhs.payload == rhs.payload; }
    template <typename T> inline bool operator!=(const master_ptr<T>& lhs, const master_ptr<T>& rhs) { return not (lhs == rhs); }
    template <typename T> inline bool operator<(const master_ptr<T>& lhs, const master_ptr<T>& rhs)  { return std::less<T*>()(lhs.payload, rhs.payload); }
    template <typename T> inline bool operator>(const master_ptr<T>& lhs, const master_ptr<T>& rhs)  { return rhs < lhs; }
    template <typename T> inline bool operator<=(const master_ptr<T>& lhs, const master_ptr<T>& rhs) { return not (rhs < lhs); }
    template <typename T> inline bool operator>=(const master_ptr<T>& lhs, const master_ptr<T>& rhs) { return not (lhs < rhs); }
    template <typename T> inline bool operator==(const master_ptr<T>& lhs, std::nullptr_t rhs)       { (void) rhs; return not lhs; }
    template <typename T> inline bool operator==(std::nullptr_t lhs, const master_ptr<T>& rhs)       { (void) lhs; return not rhs; }
    template <typename T> inline bool operator!=(const master_ptr<T>& lhs, std::nullptr_t rhs)       { (void) rhs; return (bool) lhs; }
    template <typename T> inline bool operator!=(std::nullptr_t lhs, const master_ptr<T>& rhs)       { (void) lhs; return (bool) rhs; }
    template <typename T> inline bool operator<(const master_ptr<T>& lhs, std::nullptr_t rhs)        { return std::less<T*>()(lhs.payload, nullptr); }
    template <typename T> inline bool operator<(std::nullptr_t lhs, const master_ptr<T>& rhs)        { return std::less<T*>()(nullptr, rhs.payload); }
    template <typename T> inline bool operator>(const master_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return nullptr < lhs; }
    template <typename T> inline bool operator>(std::nullptr_t lhs, const master_ptr<T>& rhs)        { (void) lhs; return rhs < nullptr; }
    template <typename T> inline bool operator<=(const master_ptr<T>& lhs, std::nullptr_t rhs)       { (void) rhs; return not (nullptr < lhs); }
    template <typename T> inline bool operator<=(std::nullptr_t lhs, const master_ptr<T>& rhs)       { (void) lhs; return not (rhs < nullptr); }
    template <typename T> inline bool operator>=(const master_ptr<T>& lhs, std::nullptr_t rhs)       { (void) rhs; return not (lhs < nullptr); }
    template <typename T> inline bool operator>=(std::nullptr_t lhs, const master_ptr<T>& rhs)       { (void) lhs; return not (nullptr < rhs); }
    template <typename T> inline bool operator==(const slave_ptr<T>& lhs, const slave_ptr<T>& rhs)   { return lhs.payload == rhs.payload; }
    template <typename T> inline bool operator!=(const slave_ptr<T>& lhs, const slave_ptr<T>& rhs)   { return not (lhs == rhs); }
    template <typename T> inline bool operator<(const slave_ptr<T>& lhs, const slave_ptr<T>& rhs)    { return std::less<T*>()(lhs.payload, rhs.payload); }
    template <typename T> inline bool operator>(const slave_ptr<T>& lhs, const slave_ptr<T>& rhs)    { return rhs < lhs; }
    template <typename T> inline bool operator<=(const slave_ptr<T>& lhs, const slave_ptr<T>& rhs)   { return not (rhs < lhs); }
    template <typename T> inline bool operator>=(const slave_ptr<T>& lhs, const slave_ptr<T>& rhs)   { return not (lhs < rhs); }
    template <typename T> inline bool operator==(const slave_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return not lhs; }
    template <typename T> inline bool operator==(std::nullptr_t lhs, const slave_ptr<T>& rhs)        { (void) lhs; return not rhs; }
    template <typename T> inline bool operator!=(const slave_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return (bool) lhs; }
    template <typename T> inline bool operator!=(std::nullptr_t lhs, const slave_ptr<T>& rhs)        { (void) lhs; return (bool) rhs; }
    template <typename T> inline bool operator<(const slave_ptr<T>& lhs, std::nullptr_t rhs)         { return std::less<T*>()(lhs.payload, nullptr); }
    template <typename T> inline bool operator<(std::nullptr_t lhs, const slave_ptr<T>& rhs)         { return std::less<T*>()(nullptr, rhs.payload); }
    template <typename T> inline bool operator>(const slave_ptr<T>& lhs, std::nullptr_t rhs)         { (void) rhs; return nullptr < lhs; }
    template <typename T> inline bool operator>(std::nullptr_t lhs, const slave_ptr<T>& rhs)         { (void) lhs; return rhs < nullptr; }
    template <typename T> inline bool operator<=(const slave_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return not (nullptr < lhs); }
    template <typename T> inline bool operator<=(std::nullptr_t lhs, const slave_ptr<T>& rhs)        { (void) lhs; return not (rhs < nullptr); }
    template <typename T> inline bool operator>=(const slave_ptr<T>& lhs, std::nullptr_t rhs)        { (void) rhs; return not (lhs < nullptr); }
    template <typename T> inline bool operator>=(std::nullptr_t lhs, const slave_ptr<T>& rhs)        { (void) lhs; return not (nullptr < rhs); }
    template <typename T> inline bool operator==(const master_ptr<T>& lhs, const slave_ptr<T>& rhs)  { return lhs.payload == rhs.payload; }
    template <typename T> inline bool operator==(const slave_ptr<T>& lhs, const master_ptr<T>& rhs)  { return lhs.payload == rhs.payload; }
    template <typename T> inline bool operator!=(const master_ptr<T>& lhs, const slave_ptr<T>& rhs)  { return not (lhs == rhs); }
    template <typename T> inline bool operator!=(const slave_ptr<T>& lhs, const master_ptr<T>& rhs)  { return not (lhs == rhs); }
    template <typename T> inline bool operator<(const master_ptr<T>& lhs, const slave_ptr<T>& rhs)   { return std::less<T*>()(lhs.payload, rhs.payload); }
    template <typename T> inline bool operator<(const slave_ptr<T>& lhs, const master_ptr<T>& rhs)   { return std::less<T*>()(lhs.payload, rhs.payload); }
    template <typename T> inline bool operator>(const master_ptr<T>& lhs, const slave_ptr<T>& rhs)   { return rhs < lhs; }
    template <typename T> inline bool operator>(const slave_ptr<T>& lhs, const master_ptr<T>& rhs)   { return rhs < lhs; }
    template <typename T> inline bool operator<=(const master_ptr<T>& lhs, const slave_ptr<T>& rhs)  { return not (rhs < lhs); }
    template <typename T> inline bool operator<=(const slave_ptr<T>& lhs, const master_ptr<T>& rhs)  { return not (rhs < lhs); }
    template <typename T> inline bool operator>=(const master_ptr<T>& lhs, const slave_ptr<T>& rhs)  { return not (lhs < rhs); }
    template <typename T> inline bool operator>=(const slave_ptr<T>& lhs, const master_ptr<T>& rhs)  { return not (lhs < rhs); }

    template <typename T, typename U> inline slave_ptr<T> static_pointer_cast(const master_ptr<U>& ptr) noexcept {
        return slave_ptr<T>(ptr, static_cast<T*>(ptr.get()));
    }

    template <typename T, typename U> inline slave_ptr<T> dynamic_pointer_cast(const master_ptr<U>& ptr) noexcept {
        if (T* p = dynamic_cast<T*>(ptr.get())) {
            return slave_ptr<T>(ptr, p);
        }
        else return slave_ptr<T>(nullptr);
    }

    template <typename T, typename U> inline slave_ptr<T> const_pointer_cast(const master_ptr<U>& ptr) noexcept {
        return slave_ptr<T>(ptr, const_cast<T*>(ptr.get()));
    }

    template <typename T, typename U> inline slave_ptr<T> static_pointer_cast(const slave_ptr<U>& ptr) {
        return slave_ptr<T>(ptr, static_cast<T*>(ptr.get()));
    }

    template <typename T, typename U> inline slave_ptr<T> dynamic_pointer_cast(const slave_ptr<U>& ptr) {
        if (T* p = dynamic_cast<T*>(ptr.get())) {
            return slave_ptr<T>(ptr, p);
        }
        else return slave_ptr<T>(nullptr);
    }

    template <typename T, typename U> inline slave_ptr<T> const_pointer_cast(const slave_ptr<U>& ptr) {
        return slave_ptr<T>(ptr, const_cast<T*>(ptr.get()));
    }

    template <typename T, typename U> inline bool instanceof(const master_ptr<U> ptr) noexcept {
        return dynamic_cast<T*>(ptr.get()) != nullptr;
    }

    template <typename T, typename U> inline bool instanceof(const slave_ptr<U> ptr) noexcept {
        return dynamic_cast<T*>(ptr.get()) != nullptr;
    }
}

#endif //SMARTPOINTERS_H

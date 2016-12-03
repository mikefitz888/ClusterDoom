#ifndef OPTIONAL_H
#define OPTIONAL_H
#include <exception>

namespace containers
{
    class InvalidOptionalAccessException : public std::exception
    {
    public:
        InvalidOptionalAccessException() : exception() {}
        virtual const char* what() const throw() {
            return "An optional value was accessed, but it does not contain a value";
        }
    };

    struct none_t {};

    template <typename T> class optional
    {
        T payload;
        bool filled;
    public:
        optional() { filled = false; }
        optional(T payload) : payload(std::move(payload)) { filled = true; }
        optional(const optional<T>& other) { filled = other.filled; payload = other.payload; }
        optional(optional<T>&& other) : payload(std::move(other.payload)) { filled = other.filled; other.filled = false; }

        inline bool isPresent() const noexcept { return filled; }
        inline T value() const
        {
            if (isPresent()) return payload;
            else throw new InvalidOptionalAccessException();
        }

        inline T valueOr(T otherwise) const noexcept
        {
            if (isPresent()) return payload;
            else return otherwise;
        }

        inline explicit operator bool () const noexcept { return isPresent(); }
        inline T operator*() const { return value(); }
        inline T* operator->() const { return &value(); }

        inline optional& operator=(none_t none) { payload.T::~T(); filled = false; return *this; }
        inline optional& operator=(const optional& other) { filled = other.filled; payload = other.payload; }
        inline optional& operator=(optional&& other) { filled = other.filled; payload = std::move(other.payload); other.filled = false; }
    };

    template <typename T, typename U, typename V> inline std::basic_ostream<U, V>& operator<<(std::basic_ostream<U, V>& os, const optional<T>& opt) {
        os << ((bool) opt ? opt.get() : "nothing");
    }
}
#endif //OPTIONAL_H

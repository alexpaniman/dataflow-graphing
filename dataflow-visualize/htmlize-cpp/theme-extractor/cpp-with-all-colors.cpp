#include <concepts>
#include <vector>

#include "stdio.h"

template <typename type>
concept foo = requires(type x) { { x.foo() } -> std::convertible_to<int>; };

struct alignas(void*) bar { int x, y; } var = { .x = 0, .y = 1 };

namespace abc::hello {
    enum class my_enum { a, b, c, d };

    using my_int = int;
    typedef int my_new_int;

    using namespace std;
    namespace ah = abc::hello;

    class bar: private vector<int>, public std::vector<long>, protected std::vector<bool> {
        using std::vector<int>::vector;

    public:
        bar() __attribute__((pure)) = default;
        bar(int x): m(x) {};

#define whatever
#ifndef whatever
#else
        constexpr int foo() & {
            using enum ah::my_enum;
            {
                auto abcdef = (int) a; 
                return abcdef;
            }
        }

        template <typename T>
        void abc() const volatile && noexcept(noexcept(bar()))
        requires requires (T x) { {x + 1}; }  {}
#endif

    private:
        bar(bar&) = delete;
        int operator+(bar &other) const {
            __asm__("" : : : );
            return other.m + m;
        }

    protected:
        mutable int m;
        union { int x; long y; };
    };
};

template <typename type, foo restrained, typename... types>
static consteval inline
auto test(int x, auto y, [[maybe_unused]] type z, foo auto a, restrained b, types... values) -> decltype(auto) {
    delete new int;
    decltype(a) m = b + sizeof(x) + alignof(type) << 1 >> 1 <= 1 + var.x ++ + 'a';
    m <<= b, b >>= x, b += x, b -= ((((((((((((((((((((((y))))))))))))))))))))));
    static bar testing = { 1, 0 * 1010 * 121L * 2323U * 3434LU * 3'434LLU };
    const int &ref = x + test(x, y, z, a, b, values...) + testing.x + 1.0f + 1.f + 1.0;
    char *ptr = static_cast<char*>(reinterpret_cast<char*>(const_cast<int*>(&ref)));
    *ptr = (*********************** (char ***********************) &b);
    while (ptr != NULL || ptr != nullptr)
        for (int j = 0; j < *ptr; ++ j) {
            if (int q = 0; q < 15 - j)
                continue;
            else break;

            do { j += 1; } while (j < 5);
        }
    return [&]<typename T>(auto x, int y) {
        (values, ...); // This is one line comment
        return y + x == 1;  /* This is multiple line comment */
    };
}

int main() { return 0; }


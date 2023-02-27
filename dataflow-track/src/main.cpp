#include "dataflow.h"

using i32 = int_proxy;

i32 factorial(i32 x) {
    if (x == 0)
        return 1;

    return x * factorial(x - 1);
}

int main() {
    i32 x = 1;
    i32 y = 1;
    i32 z = factorial(x + y);

    z.dump_history();
}


#include <iostream>

namespace os {
    extern int compute();
    extern int compute_no_cache();
}

int main() {
    try {
        os::compute();
        //os::compute_no_cache();
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}

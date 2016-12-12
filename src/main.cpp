#include "cook/main.hpp"

int main(int argc, const char **argv)
{
    MSS_BEGIN(int);

    const auto rc = cook::main(argc, argv);
    MSS(rc, std::cerr << "Error: Something went wrong " << C(rc) << std::endl);

    std::cout << "Everything went OK" << std::endl;

    MSS_END();
}

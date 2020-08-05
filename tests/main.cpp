#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <tiledarray.h>

int main(int argc, char* argv[]) {
    auto& world = TA::initialize(argc, argv);

    int res = Catch::Session().run(argc, argv);

    TA::finalize();
    return res;
}

#include <catch2/catch.hpp>
#include <chemcache/chemcache.hpp>
#include <iostream>
#include <libchemist/libchemist.hpp>
#include <limits>
#include <nwchemex/nwchemex.hpp>

using namespace libchemist;
using coord_type = std::array<double, 3>;

inline auto corr_bs() {
    AOBasisSet<double> rv;
    Center<double> O(0.0, -0.1432223429807816, 0.0);
    O.add_shell(libchemist::ShellType::pure, 0,
        std::vector<double>{1.5432896730e-01,5.3532814230e-01,4.4463454220e-01},
        std::vector<double>{1.3070932140e+02,2.3808866050e+01,6.4436083130e+00});
    O.add_shell(libchemist::ShellType::pure, 0,
        std::vector<double>{-9.9967229190e-02,3.9951282610e-01,7.0011546890e-01},
        std::vector<double>{5.0331513190e+00,1.1695961250e+00,3.8038896000e-01});
    O.add_shell(libchemist::ShellType::pure, 1,
        std::vector<double>{1.5591627500e-01,6.0768371860e-01,3.9195739310e-01},
        std::vector<double>{5.0331513190e+00,1.1695961250e+00,3.8038896000e-01});
    Center<double> H1(1.6380335020342418, 1.1365568803584036, 0.0);
    H1.add_shell(libchemist::ShellType::pure, 0,
        std::vector<double>{1.5432896730e-01,5.3532814230e-01,4.4463454220e-01},
        std::vector<double>{3.4252509140e+00,6.2391372980e-01,1.6885540400e-01});
    Center<double> H2(-1.6380335020342418, 1.1365568803584036, 0.0);
    H2.add_shell(libchemist::ShellType::pure, 0,
        std::vector<double>{1.5432896730e-01,5.3532814230e-01,4.4463454220e-01},
        std::vector<double>{3.4252509140e+00,6.2391372980e-01,1.6885540400e-01});

    rv.add_center(O);
    rv.add_center(H1);
    rv.add_center(H2);
    return rv;
}

TEST_CASE("Convenience Functions") {
    SECTION("apply_basis") {
        std::cout.precision(std::numeric_limits<double>::max_digits10);
        MoleculeManager mm = chemcache::nwx_molecule_manager();
        Molecule water      = mm.at("water");
        REQUIRE(nwx::apply_basis("sto-3g", water).basis_set() ==
                corr_bs());
    }
}
#include <catch2/catch.hpp>
#include <chemcache/chemcache.hpp>
#include <iostream>
#include <chemist/chemist.hpp>
#include <limits>
#include <nwchemex/nwchemex.hpp>

using namespace chemist;
using coord_type = std::array<double, 3>;

inline auto corr_bs(const Molecule& mol) {
    AOBasisSet<double> rv;
    auto man = chemcache::nwx_basis_set_manager();
    auto O   = man.get_basis("sto-3g", 8);
    for(auto i : {0, 1, 2}) O.coord(i) = mol[0].coords()[i];
    rv.add_center(O);

    auto H1 = man.get_basis("sto-3g", 1);
    for(auto i : {0, 1, 2}) H1.coord(i) = mol[1].coords()[i];
    rv.add_center(H1);

    auto H2 = man.get_basis("sto-3g", 1);
    for(auto i : {0, 1, 2}) H2.coord(i) = mol[2].coords()[i];
    rv.add_center(H2);
    return rv;
}

TEST_CASE("Convenience Functions") {
    SECTION("apply_basis") {
        std::cout.precision(std::numeric_limits<double>::max_digits10);
        MoleculeManager mm = chemcache::nwx_molecule_manager();
        Molecule water     = mm.at("water");
        REQUIRE(nwchemex::apply_basis("sto-3g", water).basis_set() ==
                corr_bs(water));
    }
}

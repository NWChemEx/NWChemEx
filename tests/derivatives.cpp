#include "chemist/molecule/molecule.hpp"
#include "nwchemex/nwchemex.hpp"
#include "scf/property_types/derivative_types.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using pt = scf::AOGradient;

TEST_CASE("SCF Numerical Gradient") {
    auto mol               = Molecule();
    unsigned int nelectron = 2;
    for(int i = 0; i < nelectron; i++) {
        mol.push_back(Atom(Atom::AtomName{"H"},
                           Atom::Coordinates{0.0, 0.0, float(i)},
                           Atom::Mass{1.0}, Atom::AtomicNumber{1}));
    }
    auto aos             = nwchemex::apply_basis("sto-3g", mol);
    std::vector ref_grad = {0., 0., 0.365407, 0., 0., -0.365407};
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    simde::type::chemical_system chem_sys(mol);
    auto& ng_mod = mm.at("SCF Numerical Gradient");
    auto [grad]  = ng_mod.run_as<pt>(aos, chem_sys);
    REQUIRE_THAT(grad, Catch::Approx(ref_grad).margin(0.0001));
}

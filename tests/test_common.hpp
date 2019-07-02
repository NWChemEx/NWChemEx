#pragma once
#include <libchemist/libchemist.hpp>

namespace testing {

static inline auto make_molecule() {
    using libchemist::Atom;
    using c_t = typename Atom::coord_type;
    Atom H1{1ul, c_t{1.638033502034240, 1.136556880358410, 0.000000000000000}};
    Atom O{8ul, c_t{0.000000000000000, -0.143222342980786, 0.000000000000000}};
    Atom H2{1ul, c_t{-1.638033502034240, 1.136556880358410, 0.000000000000000}};
    libchemist::Molecule water(O, H1, H2);
    return std::make_tuple(water, libchemist::apply_basis("cc-pvdz", water));
}

} //End namespace

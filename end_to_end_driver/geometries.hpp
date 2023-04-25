#pragma once
#include <nwchemex/nwchemex.hpp>

using ao_basis_t = simde::type::ao_basis_set;
using bases_ref  = const std::vector<ao_basis_t>&;
using molecule_t = simde::type::molecule;
using atom_t     = simde::type::atom;
using name_t     = typename atom_t::name_type;
//using number_t   = typename atom_t::size_type;
//using coord_t    = typename atom_t::coord_type;
using shape_t    = typename simde::type::tensor::shape_type;
using tiling_t   = typename shape_t::tiling_type;

/// System geometries and selector function
molecule_t water();
molecule_t benzene();
molecule_t dgrtl();
molecule_t ubiquitin();

inline molecule_t from_name(std::string input) {
    if(input == "water") return water();
    if(input == "benzene") return benzene();
    if(input == "dgrtl") return dgrtl();
    if(input == "ubiquitin") return ubiquitin();
    return molecule_t{};
}

/// Tiling functions and selector based on system
/// These are mostly placeholders until system specific tilings are acquired
inline shape_t atomic_tiling(bases_ref bases) {
    tiling_t tiling;
    for(auto& set : bases) {
        tiling.push_back({0});
        auto& back = tiling.back();
        for(auto& center : set) {
            back.push_back(back.back() + center.n_aos());
        }
    }
    return shape_t{tiling};
}

inline shape_t one_big_tiling(bases_ref bases) {
    tiling_t tiling;
    for(auto& set : bases) { tiling.push_back({0, set.n_aos()}); }
    return shape_t{tiling};
}

inline shape_t integral_tiling(std::string input, bases_ref bases) {
    if(input == "water") return atomic_tiling(bases);
    if(input == "benzene") return one_big_tiling(bases);
    if(input == "dgrtl") return atomic_tiling(bases);
    if(input == "ubiquitin") return atomic_tiling(bases);
    return shape_t{};
}

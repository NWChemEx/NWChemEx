#pragma once
#include <nwchemex/nwchemex.hpp>

using molecule_t = simde::type::molecule;
using atom_t     = simde::type::atom;
using name_t     = typename atom_t::name_type;
using number_t   = typename atom_t::size_type;
using coord_t    = typename atom_t::coord_type;

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
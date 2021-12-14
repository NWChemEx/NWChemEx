#include "modules.hpp"
#include "nwchemex/apply_basis.hpp"
#include <simde/simde.hpp>

namespace nwchemex {

using simde::type::ao_basis_set;
using simde::type::ao_space;
using simde::type::atom;
using simde::type::molecule;

using ptype        = simde::AuxiliaryBasisSet;
using manager_type = chemist::BasisSetManager;
using coords_type  = typename atom::coord_type;

MODULE_CTOR(AuxiliaryBasis) {
    satisfies_property_type<ptype>();
    description("Produces an auxiliary basis set corresponding to the provided "
                "atomic basis set.");

    add_input<std::string>("Aux Basis Suffix").set_default("-jkfit");
}

MODULE_RUN(AuxiliaryBasis) {
    const auto& [basis_space] = ptype::unwrap_inputs(inputs);
    auto suffix = inputs.at("Aux Basis Suffix").value<std::string>();
    // TODO: Make this an additional input; basis set managers not hashable ATM
    auto manager = chemcache::nwx_basis_set_manager();

    // Use an AtomicBasisSet to make a single atom molecule instance.
    // Gets passed to apply_basis.
    auto atomic_sys = [](const chemist::AtomicBasisSet<double>& center) {
        auto Z      = center.atomic_number();
        auto coords = coords_type{center.x(), center.y(), center.z()};
        return molecule{atom{Z, coords}};
    };

    // convert atomic basis to auxiliary basis
    ao_basis_set aux_basis;
    for(auto& center : basis_space.basis_set()) {
        auto aux_name    = center.basis_set_name() + suffix;
        auto atom_as_mol = atomic_sys(center);

        auto aux_center = apply_basis(aux_name, atom_as_mol, manager);
        aux_basis       = aux_basis + aux_center.basis_set();
    }

    auto rv = results();
    return ptype::wrap_results(rv, ao_space{aux_basis});
}

} // namespace nwchemex
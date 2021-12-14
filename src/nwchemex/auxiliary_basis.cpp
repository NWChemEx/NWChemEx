#include "modules.hpp"
#include "nwchemex/apply_basis.hpp"
#include <simde/simde.hpp>

namespace nwchemex {

using simde::type::ao_basis_set;
using simde::type::atom;
using simde::type::molecule;

using ptype        = simde::AuxiliaryBasisSet;
using manager_type = chemist::BasisSetManager;
using coords_type  = typename atom::coord_type;

// Use the atomic basis set to make a single atom molecule instance.
// Gets passed to apply_basis.
molecule atomic_sys(const chemist::AtomicBasisSet<double>& center) {
    return molecule{atom{center.atomic_number(),
                         coords_type{center.x(), center.y(), center.z()}}};
}

MODULE_CTOR(AuxiliaryBasis) {
    satisfies_property_type<ptype>();
    description("Produces an auxiliary basis set corresponding to the provided "
                "atomic basis set.");

    add_input<manager_type>("Basis Set Manager");
}

MODULE_RUN(AuxiliaryBasis) {
    const auto& [atomic_basis] = ptype::unwrap_inputs(inputs);
    auto manager = inputs.at("Basis Set Manager").value<manager_type>();
    auto rv      = results();

    // convert atomic basis to auxiliary basis
    ao_basis_set aux_basis;
    for(auto& center : atomic_basis) {
        auto aux_name    = center.basis_set_name() + "-jkfit";
        auto atom_as_mol = atomic_sys(center);

        auto aux_center = apply_basis(aux_name, atom_as_mol, manager);
        aux_basis       = aux_basis + aux_center.basis_set();
    }

    return ptype::wrap_results(rv, aux_basis);
}

} // namespace nwchemex
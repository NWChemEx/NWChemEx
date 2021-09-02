#include "modules.hpp"
#include <simde/simde.hpp>

namespace nwx {

using simde::type::els_coulomb;
using simde::type::els_kinetic;
using simde::type::els_nuc_coulomb;
using simde::type::hamiltonian;
using simde::type::many_electrons;
using simde::type::nuc_coulomb;

using ptype = simde::SystemHamiltonian;

MODULE_CTOR(SystemHamiltonian) {
    satisfies_property_type<ptype>();
    description("Makes a Hamiltonian operator from a Chemical System");
}

MODULE_RUN(SystemHamiltonian) {
    const auto& [sys] = ptype::unwrap_inputs(inputs);
    auto rv           = results();

    auto n_electrons   = many_electrons{sys.nelectrons()};
    const auto& nuclei = sys.molecule();

    hamiltonian H(nuc_coulomb{nuclei}, els_kinetic{n_electrons},
                  els_nuc_coulomb{n_electrons, nuclei},
                  els_coulomb{n_electrons});

    return ptype::wrap_results(rv, H);
}

} // namespace nwx

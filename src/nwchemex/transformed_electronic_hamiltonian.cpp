#include "nwchemex/load_modules.hpp"
//#include <hdf5.h>
#include <highfive/H5File.hpp>
#include <casscf/property_types/transformed_electronic_hamiltonian.hpp>

namespace simde {

using ham_pt  = simde::TransformedElectronicHamiltonian;
using qcs_pt = QCSchemaFactory;

MODULE_CTOR(HDF5QCSchema) {
    satisfies_property_type<qcs_pt>();
    add_submodule<ham_pt>("Transformed Hamiltonian");
}

MODULE_RUN(HDF5QCSchema) {
    // Upwrap Inputs
    const auto& [H_e, phi0, fname] = qcs_pt::unwrap_inputs(inputs);

    // Form full orbitals space
    const auto occ_space = phi0.basis_set().occupied_orbitals();
    const auto vir_space = phi0.basis_set().virtual_orbitals();
    const auto gen_space = occ_space + vir_space;

    // Get Second Quantized Hamiltonian
    auto sq_mod        = submods.at("Transformed Hamiltonian").value();
    const auto& [T, V] = sq_mod.run_as<ham_pt>(H_e, gen_space);

    // Extract Dimensions
    const size_t nocc = occ_space.C().extents()[1];
    const size_t nvir = vir_space.C().extents()[1];

    const hsize_t naos = occ_space.C().extents()[0];

    HighFive::File file(fname, HighFive::File::Overwrite);

    using tensorwrapper::tensor::to_vector;
    {
        auto eps_occ = to_vector(occ_space.orbital_energies());
        auto eps_vir = to_vector(vir_space.orbital_energies());
        std::vector<double> eps(naos);
        std::copy(eps_occ.begin(), eps_occ.end(), eps.begin());
        std::copy(eps_vir.begin(), eps_vir.end(), eps.begin() + eps_occ.size());
        HighFive::DataSpace eps_space(naos);
        HighFive::DataSet eps_dset = file.createDataSet(
          "scf_eigenvalues_a", eps_space, HighFive::AtomicType<double>());
        eps_dset.write_raw(eps.data(), HighFive::AtomicType<double>());
    }

    {
        auto C = to_vector(gen_space.C());
        HighFive::DataSpace ob_space(naos, naos);
        HighFive::DataSet orb_dset = file.createDataSet(
          "scf_orbitals_a", ob_space, HighFive::AtomicType<double>());
        orb_dset.write_raw(C.data(), HighFive::AtomicType<double>());
    }

    {
        auto T_pq = to_vector(T);
        HighFive::DataSpace ob_space(naos, naos);
        HighFive::DataSet core_h_dset = file.createDataSet(
          "scf_core_h_mo_a", ob_space, HighFive::AtomicType<double>());
        core_h_dset.write_raw(T_pq.data(), HighFive::AtomicType<double>());
    }

    {
        auto V_pqrs = to_vector(V);
        HighFive::DataSpace tb_space(naos, naos, naos, naos);
        HighFive::DataSet mo_eri_dset = file.createDataSet(
          "scf_eri_mo_aa", tb_space, HighFive::AtomicType<double>());
        mo_eri_dset.write_raw(V_pqrs.data(), HighFive::AtomicType<double>());
    }

    return qcs_pt::wrap_results(results());
}

} // namespace simde

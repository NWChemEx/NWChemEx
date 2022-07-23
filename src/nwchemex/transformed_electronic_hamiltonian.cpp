#include "nwchemex/load_modules.hpp"
//#include <hdf5.h>
#include <highfive/H5File.hpp>

namespace simde {

using ham_pt  = TransformedElectronicHamiltonian;
using teri_pt = TransformedERI4;
using tkin_pt = TransformedTensorRepresentation<2, simde::type::el_kinetic>;
using tvne_pt = TransformedTensorRepresentation<2, simde::type::el_nuc_coulomb>;

MODULE_CTOR(SecondQuantizedHamiltonian) {
    satisfies_property_type<ham_pt>();
    add_submodule<teri_pt>("Transformed ERIs");
    add_submodule<tkin_pt>("Transformed Kinetic");
    add_submodule<tvne_pt>("Transformed Nuclear");
}

MODULE_RUN(SecondQuantizedHamiltonian) {
    const auto& [H_e, p] = ham_pt::unwrap_inputs(inputs);
    auto& eri_mod        = submods.at("Transformed ERIs").value();
    auto& kin_mod        = submods.at("Transformed Kinetic").value();
    auto& vne_mod        = submods.at("Transformed Nuclear").value();

    // Compute two-body term
    // TODO: Get from Hamiltonian

    simde::type::el_el_coulomb r12;
    using simde::tensor_representation;
    const auto& [V] = tensor_representation(eri_mod, p, p, r12, p, p);

    // Compute one-body term
    chemist::operators::CoreHamiltonian H_core(H_e);
    const auto t_terms = H_core.get_terms<simde::type::el_kinetic>();
    const auto v_terms = H_core.get_terms<simde::type::el_nuc_coulomb>();

    simde::type::tensor H;

    // Use T to initialize H
    const auto& [t0] = tensor_representation(kin_mod, p, *(t_terms.at(0)), p);
    H                = t0;
    for(std::size_t i = 1; i < t_terms.size(); ++i) {
        const auto& [ti] = tensor_representation(kin_mod, p, *t_terms[i], p);
        H("p,q")         = H("p,q") + ti("p,q");
    }

    // Add V terms
    for(const auto& v : v_terms) {
        const auto& [vi] = tensor_representation(vne_mod, p, *v, p);
        H("p,q")         = H("p,q") + vi("p,q");
    }

    // Return results
    auto rv = results();
    return ham_pt::wrap_results(rv, H, V);
}

using qcs_pt = QCSchemaFactory;
MODULE_CTOR(HDF5QCSchema) {
    satisfies_property_type<qcs_pt>();
    add_submodule<ham_pt>("Transformed Hamiltonian");

    add_input<std::string>("Filename")
      .set_description("HDF5 Filename")
      .set_default("data.hdf5");
}

MODULE_RUN(HDF5QCSchema) {
    // Upwrap Inputs
    const auto& [H_e, phi0] = qcs_pt::unwrap_inputs(inputs);
    const auto& fname       = inputs.at("Filename").value<std::string>();

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

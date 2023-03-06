#include "geometries.hpp"
#include <integrals/property_types/integral_shape.hpp>
#include <nwchemex/nwchemex.hpp>
#include <tiledarray.h>

int main(int argc, char* argv[]) {
    auto& world = TA::initialize(argc, argv);

    /// Arg parsing
    std::vector<std::string> args(argv + 1, argv + argc);
    const auto mol_name       = args.at(0);
    const auto basis_name     = args.at(1);
    const auto aux_basis_name = args.at(2);
    std::cout << mol_name << std::endl;
    std::cout << basis_name << std::endl;
    std::cout << aux_basis_name << std::endl;

    /// NWX setup
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    /// Hacky way to check if we have GPU modules
    bool have_gpu_modules = mm.count("DFJ_JEngine");

    /// Job specifications
    auto mol = from_name(mol_name);
    auto aos = nwchemex::apply_basis(basis_name, mol, true);
    auto aux = nwchemex::apply_basis(aux_basis_name, mol);
    auto cs  = simde::type::chemical_system(mol);

    // Set inputs
    mm.change_input("Overlap CS", "Screening Threshold", 1e-12);
    mm.change_input("Kinetic CS", "Screening Threshold", 1e-12);
    mm.change_input("Nuclear CS", "Screening Threshold", 1e-12);
    mm.change_input("DFJ", "Fitting Basis", aux);
    mm.change_input("DFK", "Fitting Basis", aux);
    if(have_gpu_modules) {
        mm.change_input("DFJ_JEngine", "Fitting Basis", aux);
        mm.change_input("GauXC Quadrature Batches", "On GPU", true);
        mm.change_input("snLinK", "On GPU", true);
        mm.change_input("XC", "On GPU", true);
        // mm.at("GauXC Basis Converter").turn_off_memoization();
        // mm.at("GauXC Molecule Converter").turn_off_memoization();
        // mm.at("GauXC Quadrature Batches").turn_off_memoization();
        // mm.at("snLinK").turn_off_memoization();
        // mm.at("XC").turn_off_memoization();
    }

    /// Lambda module to determine the shapes based on system
    auto shape_mod = pluginplay::make_lambda<integrals::IntegralShape>(
      [mol_name](auto&& bases) { return integral_tiling(mol_name, bases); });

    /// Setup modules
    mm.at("ERI2").change_submod("Tensor Shape", shape_mod);
    mm.at("ERI3").change_submod("Tensor Shape", shape_mod);
    mm.at("Nuclear CS").change_submod("Tensor Shape", shape_mod);
    mm.at("Kinetic CS").change_submod("Tensor Shape", shape_mod);
    mm.at("Overlap CS").change_submod("Tensor Shape", shape_mod);

    mm.change_submod("CoreH", "Electron-Nuclear Attraction", "Nuclear CS");
    mm.change_submod("CoreH", "Kinetic Energy", "Kinetic CS");
    mm.change_submod("CoreGuess", "Overlap", "Overlap CS");
    mm.change_submod("SCF Step", "Overlap", "Overlap CS");
    mm.change_submod("DIIS Fock Matrix", "Overlap", "Overlap CS");
    mm.change_submod("SCFDIIS Step", "Overlap", "Overlap CS");
    mm.change_submod("Fock Matrix", "J Builder", "DFJ");
    mm.change_submod("Fock Matrix", "K Builder", "DFK");
    if(have_gpu_modules) {
        mm.change_submod("Fock Matrix", "J Builder", "DFJ_JEngine");
        mm.change_submod("Fock Matrix", "K Builder", "snLinK");
    }
    mm.change_submod("SCF Energy", "Reference Wave Function",
                     "SCFDIIS Wavefunction");

    // Zero mods for J and K
    auto shape = integral_tiling(mol_name, std::vector(2, aos.basis_set()));
    auto zero_tensor = [shape](auto&& bra, auto&& o, auto&& ket) {
        auto l = [](const auto& idx) { return 0.0; };
        auto s = shape.clone();
        auto a = tensorwrapper::tensor::default_allocator<
          tensorwrapper::tensor::field::Scalar>();
        return simde::type::tensor(l, std::move(s), std::move(a));
    };
    auto fake_j = pluginplay::make_lambda<simde::MeanFieldJ>(zero_tensor);
    auto fake_k = pluginplay::make_lambda<simde::MeanFieldK>(zero_tensor);
    // mm.at("Fock Matrix").change_submod("J Builder", fake_j);
    // mm.at("Fock Matrix").change_submod("K Builder", fake_k);

    /// For just one fock build
    mm.change_input("SCF Loop", "MaxIt", simde::type::size{0});
    mm.change_input("SCFDIIS Loop", "MaxIt", simde::type::size{0});
    auto ee_mod = pluginplay::make_lambda<simde::CanonicalElectronicEnergy>(
      [](auto&& a, auto&& b, auto&& c) { return 0.0; });
    mm.at("Converged").change_submod("Energy", ee_mod);
    auto e_mod = pluginplay::make_lambda<simde::TotalCanonicalEnergy>(
      [](auto&& a, auto&& b, auto&& c) { return 0.0; });
    mm.at("SCF Energy").change_submod("Reference Energy", e_mod);

    // Run and print profile info
    auto [E] = mm.at("SCF Energy").run_as<simde::AOEnergy>(aos, cs);
    world.gop.fence();
    std::cout << "SCF Energy = " << E << std::endl;
    std::cout << "SCF Profile:" << std::endl;
    std::cout << mm.at("SCF Energy").profile_info() << std::endl;

    TA::finalize();
    return 0;
}
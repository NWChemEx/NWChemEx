#include <nwchemex/nwchemex.hpp>
#include <tiledarray.h>

using mol_bs_pt   = simde::MolecularBasisSet;
using molecule_pt = simde::MoleculeFromString;

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
    auto mol  = mm.at("NWX Molecules").run_as<molecule_pt>(mol_name);
    auto obs  = mm.at(basis_name).run_as<mol_bs_pt>(mol);
    auto axbs = mm.at(aux_basis_name).run_as<mol_bs_pt>(mol);

    for(auto si = 0; si < obs.n_shells(); ++si) {
        obs.shell(si).pure() = chemist::ShellType::cartesian;
    }
    //for(auto si = 0; si < axbs.n_shells(); ++si) {
    //    axbs.shell(si).pure() = chemist::ShellType::cartesian;
    //}

    simde::type::ao_space aos(obs);
    simde::type::ao_space aux(axbs);
    simde::type::chemical_system cs(mol);

    // Set auxiliary basis set
    mm.change_input("DFJ", "Fitting Basis", aux);
    mm.change_input("DFK", "Fitting Basis", aux);
    mm.change_submod("Fock Matrix", "J Builder", "DFJ");
    mm.change_submod("Fock Matrix", "K Builder", "DFK");

    // Set screening thresholds
    mm.change_input("Overlap CS", "Screening Threshold", 1e-12);
    mm.change_input("Kinetic CS", "Screening Threshold", 1e-12);
    mm.change_input("Nuclear CS", "Screening Threshold", 1e-12);

    // Set shapes for integrals
    mm.change_submod("ERI2", "Tensor Shape", "CenterTiledShape");
    mm.change_submod("ERI3", "Tensor Shape", "CenterTiledShape");
    mm.change_submod("ERI4", "Tensor Shape", "CenterTiledShape");
    mm.change_submod("Nuclear CS", "Tensor Shape", "CenterTiledShape");
    mm.change_submod("Kinetic CS", "Tensor Shape", "CenterTiledShape");
    mm.change_submod("Overlap CS", "Tensor Shape", "CenterTiledShape");
    mm.change_submod("Nuclear", "Tensor Shape", "CenterTiledShape");
    mm.change_submod("Kinetic", "Tensor Shape", "CenterTiledShape");
    mm.change_submod("Overlap", "Tensor Shape", "CenterTiledShape");

    // Use gpu modules
    if(have_gpu_modules) {
        mm.change_input("GauXC Quadrature Batches", "On GPU", true);
        mm.change_input("snLinK", "On GPU", true);
        mm.change_input("XC", "On GPU", true);
        mm.change_input("DFJ_JEngine", "Fitting Basis", aux);
        mm.change_submod("Fock Matrix", "J Builder", "DFJ_JEngine");
        mm.change_submod("Fock Matrix", "K Builder", "snLinK");
    }

    // Use CS screened integrals
    mm.change_submod("CoreH", "Electron-Nuclear Attraction", "Nuclear CS");
    mm.change_submod("CoreH", "Kinetic Energy", "Kinetic CS");
    mm.change_submod("CoreGuess", "Overlap", "Overlap CS");
    mm.change_submod("SCF Step", "Overlap", "Overlap CS");
    mm.change_submod("DIIS Fock Matrix", "Overlap", "Overlap CS");
    mm.change_submod("SCFDIIS Step", "Overlap", "Overlap CS");

    // Use DIIS
    mm.change_submod("SCF Energy", "Reference Wave Function",
                     "SCFDIIS Wavefunction");

    // Run and print profile info
    auto E = mm.at("SCF Energy").run_as<simde::AOEnergy>(aos, cs);
    world.gop.fence();
    std::cout << "SCF Energy = " << E << std::endl;
    std::cout << "SCF Profile:" << std::endl;
    std::cout << mm.at("SCF Energy").profile_info() << std::endl;

    TA::finalize();
    return 0;
}

#include "modules.hpp"
#include "nwchemex/load_modules.hpp"
#include <integrals/integrals.hpp>
#include <mp2/mp2.hpp>
#include <scf/scf.hpp>

namespace {

void set_integrals_default_modules(pluginplay::ModuleManager& mm) {
    mm.change_submod("Transformed K", "integral kernel", "CanJK");
    mm.change_submod("Transformed Fock", "integral kernel", "Fock Matrix");
}

void set_scf_default_modules(pluginplay::ModuleManager& mm) {
    mm.change_submod("CanJK", "ERI Builder", "ERI4");
    mm.change_submod("CanJ", "ERI Builder", "ERI4");
    mm.change_submod("DFJ", "ERI Builder", "Transformed ERI3");
    mm.change_submod("DFJK", "ERI Builder", "Transformed ERI3");
    mm.change_submod("MetricChol", "M Builder", "ERI2");
    mm.change_submod("CoreH", "Kinetic Energy", "Kinetic");
    mm.change_submod("CoreH", "Electron-Nuclear Attraction", "Nuclear");
    mm.change_submod("CoreGuess", "Overlap", "Overlap");
    mm.change_submod("SCF", "Overlap", "Overlap");
    mm.change_submod("DIIS Fock Matrix", "Overlap", "Overlap");
    mm.change_submod("SCFDIIS", "Overlap", "Overlap");
    // mm.change_submod("DiagonalUpdate", "S Builder", "Overlap");
}

void set_mp2_default_modules(pluginplay::ModuleManager& mm) {
    mm.change_submod("MP1 Wavefunction", "Transformed ERIS",
                     "Transformed ERI4");
    mm.change_submod("MP2", "Transformed ERIs", "Transformed ERI4");
    mm.change_submod("PAOs", "S Builder", "Overlap");
    mm.change_submod("QC LMOs", "Fock builder", "Transformed Fock");
    mm.change_submod("QC PAOs", "Fock builder", "Transformed Fock");
    mm.change_submod("MP2 Dipole", "dipole", "EDipole");
    mm.change_submod("L(LMO->LMO) (prescreening)", "DOIs", "DOI");
    mm.change_submod("DOI Sparsity", "DOIs", "DOI");

    mm.change_submod("CABS", "Overlap", "Overlap");
    mm.change_submod("RIBS", "Overlap", "Overlap");
    mm.change_submod("MP2-F12 Coupling", "Fock builder", "Transformed Fock");
    mm.change_submod("MP2-F12 Coupling", "(ai|f12|pj)", "Transformed STG4");
    mm.change_submod("MP2-F12 V", "(mn|f/r|ls)", "Transformed Yukawa4");
    mm.change_submod("MP2-F12 V", "(mn|1/r|ls)", "Transformed ERI4");
    mm.change_submod("MP2-F12 V", "(ia|f12|jb)", "Transformed STG4");
    mm.change_submod("MP2-F12 X", "(mn|r|ls)", "Transformed STG4");
    mm.change_submod("MP2-F12 X", "(m|f|n)", "Transformed Fock");
    mm.change_submod("MP2-F12 B Approx C", "(mn|df12*df12|ls)",
                     "Transformed STG 4 Center dfdr Squared");
    mm.change_submod("MP2-F12 B Approx C", "Exchange builder", "Transformed K");
    mm.change_submod("MP2-F12 B Approx C", "Fock builder", "Transformed Fock");
    mm.change_submod("MP2-F12 B Approx C", "(ia|f12|jb)", "Transformed STG4");
}

} // namespace

namespace nwchemex {

void load_modules(pluginplay::ModuleManager& mm) {
    integrals::load_modules(mm);
    scf::load_modules(mm);
    mp2::load_modules(mm);

    mm.add_module<SystemHamiltonian>("SystemHamiltonian");
    mm.add_module<AuxiliaryBasis>("AuxiliaryBasis");

    set_integrals_default_modules(mm);
    set_scf_default_modules(mm);
    set_mp2_default_modules(mm);
}

} // namespace nwchemex

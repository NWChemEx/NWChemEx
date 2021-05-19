#include "nwchemex/load_modules.hpp"
#include <integrals/integrals.hpp>
#include <mp2/mp2.hpp>
#include <scf/scf.hpp>

namespace {

void set_scf_default_modules(sde::ModuleManager& mm) {
    mm.change_submod("CanJK", "ERI Builder", "ERI4");
    mm.change_submod("CanJ", "ERI Builder", "ERI4");
    mm.change_submod("CoreH", "T Builder", "Kinetic");
    mm.change_submod("CoreH", "V Builder", "Nuclear");
    mm.change_submod("DFJ", "3CERI Builder", "ERI3");
    mm.change_submod("DFJK", "3CERI Builder", "ERI3");
    mm.change_submod("DiagonalUpdate", "S Builder", "Overlap");
    mm.change_submod("MetricChol", "M Builder", "ERI2");
    mm.change_submod("SCFDIIS", "S Builder", "Overlap");

    using fock_type     = mp2::pt::dense_fock<double>;
    using coulomb_type  = mp2::pt::dense_j<double>;
    using exchange_type = mp2::pt::dense_k<double>;
    using core_type     = mp2::pt::dense_h<double>;
    integrals::register_transformed_integral<fock_type>(mm, "Fock");
    integrals::register_transformed_integral<core_type>(mm, "CoreH");
    integrals::register_transformed_integral<coulomb_type>(mm, "CanJK");
    mm.rename_module("Transformed CanJK", "Transformed Coulomb");
    integrals::register_transformed_integral<exchange_type>(mm, "CanJK");
    mm.rename_module("Transformed CanJK", "Transformed Exchange");
}

void set_mp2_default_modules(sde::ModuleManager& mm) {
    mm.change_submod("Sparse LMOs", "Fock", "Fock");
    mm.change_submod("Sparse QC PAOs", "Fock builder", "Fock");
    mm.change_submod("QC PNOs", "Fock builder", "Fock");
    mm.change_submod("Pair-specific QC PAOs", "Fock builder", "Fock");
    mm.change_submod("Sparse MP2 Dipole", "dipole", "EDipole");
    mm.change_submod("DOI SparseMap Builder", "dois", "DOI");
    mm.change_submod("PAOs", "S Builder", "Overlap");
    mm.change_submod("Sparse K", "(mn|ls)", "ERI4");
    mm.change_submod("DLPNO Residuals", "Fock", "Transformed Fock");
    mm.change_submod("PNO Overlap", "(mu|nu)", "Overlap");
    mm.change_submod("L(LMO->PAO)", "dois", "DOI");
    mm.change_submod("L(Pair->PAO)", "dois", "DOI");
    mm.change_submod("Dense MP2 Amplitudes", "(ia|jb)", "Transformed ERI4");
    mm.change_submod("MP2", "(ia|jb)", "Transformed ERI4");
    mm.change_submod("MP2 Dipole", "dipole", "EDipole");
    mm.change_submod("CABS", "Overlap", "Overlap");
    mm.change_submod("RIBS", "Overlap", "Overlap");
    mm.change_submod("MP2-F12 B Approx C", "(ia|f12|jb)",
                     "Transformed STG 4 Center Correlation Factor");
    mm.change_submod("MP2-F12 B Approx C", "(mn|df12*df12|ls)",
                     "Transformed STG 4 Center dfdr Squared");
    mm.change_submod("MP2-F12 B Approx C", "(mn|f12*f12|ls)",
                     "Transformed STG 4 Center Correlation Factor Squared");
    mm.change_submod("MP2-F12 B Approx C", "Core builder", "Transformed CoreH");
    mm.change_submod("MP2-F12 B Approx C", "Coulomb builder",
                     "Transformed Coulomb");
    mm.change_submod("MP2-F12 B Approx C", "Exchange builder",
                     "Transformed Exchange");
    mm.change_submod("MP2-F12 B Approx C", "Fock builder", "Transformed Fock");
    mm.change_submod("MP2-F12 Coupling", "(ai|f12|pj)",
                     "Transformed STG 4 Center Correlation Factor");
    mm.change_submod("MP2-F12 Coupling", "Fock builder", "Transformed Fock");
    mm.change_submod("MP2-F12 V", "(ia|f12|jb)",
                     "Transformed STG 4 Center Correlation Factor");
    mm.change_submod("MP2-F12 V", "(mn|1/r|ls)", "Transformed ERI4");
    mm.change_submod("MP2-F12 V", "(mn|f/r|ls)", "Transformed STG 4 Center GR");
    mm.change_submod("MP2-F12 X", "(mn|f12*f12|ls)",
                     "Transformed STG 4 Center Correlation Factor Squared");
    mm.change_submod("MP2-F12 X", "(mn|r|ls)",
                     "Transformed STG 4 Center Correlation Factor");
    mm.change_submod("MP2-F12 X", "(m|f|n)", "Transformed Fock");
}

} // namespace

namespace nwx {

void load_modules(sde::ModuleManager& mm) {
    integrals::load_modules(mm);
    scf::load_modules(mm);
    mp2::load_modules(mm);

    set_scf_default_modules(mm);
    set_mp2_default_modules(mm);
}

} // namespace nwx

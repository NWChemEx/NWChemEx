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
    mm.change_submod("DIIS", "DIIS Cache", "CacheDIIS");
    mm.change_submod("DIIS", "S Builder", "Overlap");

    using fock_type = mp2::pt::dense_fock<double>;
    integrals::register_transformed_integral<fock_type>(mm, "Fock");
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

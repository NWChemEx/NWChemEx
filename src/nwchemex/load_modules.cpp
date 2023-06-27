/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "drivers/driver_modules.hpp"
#include "modules.hpp"
#include "nwchemex/load_modules.hpp"
#include <chemcache/chemcache.hpp>
#include <integrals/integrals.hpp>
// #include <mp2/mp2.hpp>
#include <scf/scf.hpp>
#include <cc/ccsd.hpp>

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
    mm.change_submod("MOs Fock", "Overlap", "Overlap");
    mm.change_submod("DIIS Fock Matrix", "Overlap", "Overlap");
    mm.change_submod("XC", "Tensor Shape", "OneTileShape");
}

// Uncomment once MP2 is working again
// void set_mp2_default_modules(pluginplay::ModuleManager& mm) {
//     mm.change_submod("MP1 Wavefunction", "Transformed ERIS",
//                      "Transformed ERI4");
//     mm.change_submod("MP2", "Transformed ERIs", "Transformed ERI4");
//     mm.change_submod("PAOs", "S Builder", "Overlap");
//     mm.change_submod("QC LMOs", "Fock builder", "Transformed Fock");
//     mm.change_submod("QC PAOs", "Fock builder", "Transformed Fock");
//     mm.change_submod("MP2 Dipole", "dipole", "EDipole");
//     mm.change_submod("L(LMO->LMO) (prescreening)", "DOIs", "DOI");
//     mm.change_submod("DOI Sparsity", "DOIs", "DOI");

//     mm.change_submod("CABS", "Overlap", "Overlap");
//     mm.change_submod("RIBS", "Overlap", "Overlap");
//     mm.change_submod("MP2-F12 Coupling", "Fock builder", "Transformed Fock");
//     mm.change_submod("MP2-F12 Coupling", "(ai|f12|pj)", "Transformed STG4");
//     mm.change_submod("MP2-F12 V", "(mn|f/r|ls)", "Transformed Yukawa4");
//     mm.change_submod("MP2-F12 V", "(mn|1/r|ls)", "Transformed ERI4");
//     mm.change_submod("MP2-F12 V", "(ia|f12|jb)", "Transformed STG4");
//     mm.change_submod("MP2-F12 X", "(mn|r|ls)", "Transformed STG4");
//     mm.change_submod("MP2-F12 X", "(m|f|n)", "Transformed Fock");
//     mm.change_submod("MP2-F12 B Approx C", "(mn|df12*df12|ls)",
//                      "Transformed STG 4 Center dfdr Squared");
//     mm.change_submod("MP2-F12 B Approx C", "Exchange builder", "Transformed
//     K"); mm.change_submod("MP2-F12 B Approx C", "Fock builder", "Transformed
//     Fock"); mm.change_submod("MP2-F12 B Approx C", "(ia|f12|jb)",
//     "Transformed STG4");
// }

void set_ccsd_default_modules(pluginplay::ModuleManager& mm) {
    mm.change_submod("CCSD", "Fock Builder", "Fock Matrix");
    mm.change_submod("CCSD", "Transformed ERIs", "Transformed ERI4");
    mm.change_submod("CCSD", "Electronic Energy", "Electronic Energy");
}

} // namespace

namespace nwchemex {

void set_defaults(pluginplay::ModuleManager& mm) {
    mm.change_submod("SCF Energy", "System Hamiltonian", "SystemHamiltonian");
    mm.change_submod("SCF Energy", "Reference Wave Function",
                     "SCF Wavefunction");
    mm.change_submod("SCF Energy", "Reference Energy", "Total Energy");

    mm.change_submod("SCF Energy From Density", "System Hamiltonian",
                     "SystemHamiltonian");
    mm.change_submod("SCF Energy From Density", "Reference Density",
                     "SCF Density Driver");
    mm.change_submod("SCF Energy From Density", "Reference Energy",
                     "Total Energy From Density");

    mm.change_submod("SCF Numerical Gradient", "System Hamiltonian",
                     "SystemHamiltonian");
    mm.change_submod("SCF Numerical Gradient", "Reference Energy",
                     "Total Energy");
    mm.change_submod("SCF Numerical Gradient", "Reference Wave Function",
                     "SCF Wavefunction");

    // mm.change_submod("MP2 Correlation Energy", "System Hamiltonian",
    //                  "SystemHamiltonian");
    // mm.change_submod("MP2 Correlation Energy", "Reference Wave Function",
    //                  "SCF Wavefunction");
    // mm.change_submod("MP2 Correlation Energy", "Many Body Wave Function",
    //                  "MP1 Wavefunction");
    // mm.change_submod("MP2 Correlation Energy", "Correlation Energy", "MP2");

    // mm.change_submod("MP2 Energy", "System Hamiltonian",
    // "SystemHamiltonian");
    // mm.change_submod("MP2 Energy", "Reference Wave Function",
    //                  "SCF Wavefunction");
    // mm.change_submod("MP2 Energy", "Reference Energy", "Total Energy");
    // mm.change_submod("MP2 Energy", "Many Body Wave Function",
    //                  "MP1 Wavefunction");
    // mm.change_submod("MP2 Correlation Energy", "Correlation Energy", "MP2");
}

void load_modules(pluginplay::ModuleManager& mm) {
    integrals::load_modules(mm);
    scf::load_modules(mm);
    chemcache::load_modules(mm);
    // mp2::load_modules(mm);
    ccsd::load_modules(mm);
    drivers::load_modules(mm);

    mm.add_module<SystemHamiltonian>("SystemHamiltonian");

    set_integrals_default_modules(mm);
    set_scf_default_modules(mm);
    // set_mp2_default_modules(mm);
    set_ccsd_default_modules(mm);
    set_defaults(mm);
}

} // namespace nwchemex

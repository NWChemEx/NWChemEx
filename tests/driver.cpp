#include <catch2/catch.hpp>
#include <nwchemex/load_modules.hpp>
#include <tamm/tamm.hpp>
#include <libchemist/libchemist.hpp>

//2. Include the property type for the module you want to run
#include <property_types/reference_wavefunction.hpp>

//3. Change this to the property type you want to run as
using pt_type = property_types::ReferenceWavefunction<double>;

TEST_CASE("Driving NWX from C++"){
    sde::ModuleManager mm;
    nwx::load_modules(mm);
    libchemist::MoleculeManager mols;

    auto mol = mols.at("water");
    auto bs  = libchemist::apply_basis("cc-pvdz", mol);

    //4. Change to the module key you want to run
    const auto module_key = "SCF";
    //5. (Optional) Change options

    //6. Run the module
    const auto& [egy, mos] =
        mm.run_as<pt_type>(module_key, mol, bs, std::size_t{0});

    std::cout <<
      tamm::get_scalar(const_cast<tamm::Tensor<double>&>(egy)) <<
      std::endl;
}


set(NWX_SRCS DefaultModuleManager.cpp
)
set(NWX_INCLUDES DefaultModuleManager.hpp
		 MethodWrappers.hpp
)
cpp_add_library(
    NWChemEx
    SOURCES "${NWX_SRCS}"
    INCLUDES "${NWX_INCLUDES}"
    DEPENDS sde::sde scf::scf libchemist::chemist
)

if(BUILD_PYBINDINGS)
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    find_package(Cppyy REQUIRED)
    cppyy_make_python_package(NWChemEx NAMESPACE NWChemEx)
endif()

cpp_install(TARGETS NWChemEx)

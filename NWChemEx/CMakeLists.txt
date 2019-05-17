set(NWX_SRCS default_module_manager.cpp
	     method_wrappers.cpp
)
set(NWX_INCLUDES default_module_manager.hpp
		 method_wrappers.hpp
)
cpp_add_library(
    NWChemEx
    SOURCES "${NWX_SRCS}"
    INCLUDES "${NWX_INCLUDES}"
    DEPENDS sde::sde
)

if(BUILD_PYBINDINGS)
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    find_package(Cppyy REQUIRED)
    cppyy_make_python_package(NWChemEx NAMESPACE NWChemEx)
endif()

cpp_install(TARGETS NWChemEx)

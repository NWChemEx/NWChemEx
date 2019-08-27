set(NWX_SRCS load_modules.cpp)
set(NWX_INCLUDES load_modules.hpp)
cpp_add_library(
    nwchemex
    SOURCES "${NWX_SRCS}"
    INCLUDES "${NWX_INCLUDES}"
    DEPENDS property_types::property_types
            integrals::integrals
            scf::scf
)

cpp_install(TARGETS nwchemex)

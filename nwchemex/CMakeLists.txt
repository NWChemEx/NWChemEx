set(NWX_HEADERS nwchemex_mm.hpp)

set(NWX_SRC nwchemex_mm.cpp)

cpp_add_library(
        nwx
        SOURCES  ${NWX_SRC}
        INCLUDES ${NWX_HEADERS}
        DEPENDS sde::sde property_types::property_types integrals::integrals
                scf::scf
)

cpp_install(TARGETS nwx)

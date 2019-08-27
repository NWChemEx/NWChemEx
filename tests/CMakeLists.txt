set(sources driver.cpp
            test_main.cpp)

cpp_add_executable(
        test_nwx
        SOURCES ${sources}
        DEPENDS nwchemex Catch2::Catch2
)
add_test(NAME test_nwx COMMAND test_nwx)

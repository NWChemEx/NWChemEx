#
# This CMake module ensures that your Python interpreter has the cppyy module
# installed. 
#
# Set minimal cppyy version
#
set(cppyy_version "2.2.0")
#
# Make sure Python3 is installed
#
find_package(Python3 COMPONENTS Interpreter REQUIRED)
#
# Check if cppyy Python package exists
#
execute_process(
    COMMAND Python3::Interpreter -c "import cppyy"
    RESULT_VARIABLE _fcppyy_result
)
#
# If cppyy exists, check version
#
if("${_fcppyy_result}" STREQUAL "")
    execute_process(
        COMMAND Python3::Interpreter -c "import cppyy; print(cppyy.__version__)"
        RESULT_VARIABLE _fcppyy_result2
    )
endif()
#
# Try to install cppyy Python package, if it doesn't exist or is incorrect version
#
if(NOT "${_fcppyy_result}" STREQUAL "" OR NOT "${_fcppyy_result2}" STREQUAL "${cppyy_version}")
    if(DEFINED ENV{VIRTUAL_ENV} OR DEFINED ENV{CONDA_PREFIX})
      set(_pip_args)
    else()
      set(_pip_args "--user")
    endif()
    set(_pypkg_name "cppyy==${cppyy_version}")
    execute_process(COMMAND ${Python3_EXECUTABLE} -m pip install ${_pypkg_name} ${_pip_args})
    #
    # Check again if cppyy Python works
    #
    execute_process(COMMAND Python3::Interpreter -c "import cppyy" RESULT_VARIABLE _fcppyy_result)
    if(NOT "${_fcppyy_result}" STREQUAL "")
       set(Cppyy_FOUND FALSE)
    endif()
endif()

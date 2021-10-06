#
# This CMake module ensures that your Python interpreter has the cppyy module
# installed. 
#

find_package(Python3 COMPONENTS Interpreter REQUIRED)

#Check if cppyy Python package exists
execute_process(
    COMMAND Python3::Interpreter -c "import cppyy"
    RESULT_VARIABLE _fcppyy_result
)
if(NOT "${_fcppyy_result}" STREQUAL "")
    #Try to install cppyy Python package, as it doesn't exist
    if(DEFINED ENV{VIRTUAL_ENV} OR DEFINED ENV{CONDA_PREFIX})
      set(_pip_args)
    else()
      set(_pip_args "--user")
    endif()
    set(_pypkg_name "cppyy")
    execute_process(COMMAND ${Python3_EXECUTABLE} -m pip install ${_pypkg_name} ${_pip_args})
    #Check again if cppyy Python works
    execute_process(COMMAND Python3::Interpreter -c "import cppyy" RESULT_VARIABLE _fcppyy_result)
    if(NOT "${_fcppyy_result}" STREQUAL "")
       set(Cppyy_FOUND FALSE)
    endif()
endif()

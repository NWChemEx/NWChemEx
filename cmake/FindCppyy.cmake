# Copyright 2022 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This CMake module ensures that your Python interpreter has the cppyy module
# installed. 
#
# Set minimal cppyy version
#
set(cppyy_version "2.4.1")
#
# Make sure Python3 is installed
#
find_package(Python3 COMPONENTS Interpreter REQUIRED)
#
# Check if cppyy Python package exists
#
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import cppyy"
    OUTPUT_VARIABLE _fcppyy_result
)
#
# If cppyy exists, check version
#
if("${_fcppyy_result}" STREQUAL "")
    set(Cppyy_FOUND TRUE)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import cppyy; print(cppyy.__version__),"
        OUTPUT_VARIABLE _fcppyy_result2
    )
else()
    set(Cppyy_FOUND FALSE)
endif()
#
# Try to install cppyy Python package, if it doesn't exist or is incorrect version
#
if("${_fcppyy_result}" STREQUAL "" OR NOT "${_fcppyy_result2}" VERSION_EQUAL "${cppyy_version}")
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
    execute_process(COMMAND ${Python3_EXECUTABLE} -c "import cppyy" OUTPUT_VARIABLE _fcppyy_result)
    if("${_fcppyy_result}" STREQUAL "")
        set(Cppyy_FOUND TRUE)
    	execute_process(
            COMMAND ${Python3_EXECUTABLE} -c "import cppyy; print(cppyy.__version__),"
        	OUTPUT_VARIABLE _fcppyy_result2
    	)
    else()
        set(Cppyy_FOUND FALSE)
    endif()
    #
    # Check the version again
    #
    if("${_fcppyy_result2}" VERSION_EQUAL "${cppyy_version}")
        set(Cppyy_FOUND TRUE)
    else()
        set(Cppyy_FOUND FALSE)
    endif()
else()
    set(Cppyy_FOUND TRUE)
endif()

if(NOT Cppyy_FOUND)
    message("Python3: ${Python3_EXECUTABLE}")
    message("import output: ${_fcppyy_result}")
    message("version output: ${_fcppyy_result2}")
    message(FATAL_ERROR "CMake could not install cppyy, try installing cppyy ${cppyy_version} manually.")
endif()

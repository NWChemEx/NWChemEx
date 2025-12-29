# Copyright 2024 NWChemEx-Project
#
# Fix for CMaize v1.1.10 ALIAS target handling bugs
# This file overrides problematic functions to handle ALIAS targets correctly
# 
# These function overrides fix issues where CMaize v1.1.10 fails when
# interacting with ALIAS targets (like Eigen3::Eigen and Libxc::xc).
# The fix resolves ALIAS targets to their underlying real targets before
# CMaize stores them.

include_guard()

# Helper function to resolve ALIAS targets to their real targets
function(_resolve_alias_target output_var target_name)
    if(TARGET "${target_name}")
        # Check if it's an ALIAS using get_property (not get_target_property to avoid recursion)
        get_property(_is_alias_set TARGET "${target_name}" PROPERTY ALIASED_TARGET SET)
        if(_is_alias_set)
            get_property(_aliased TARGET "${target_name}" PROPERTY ALIASED_TARGET)
            set(${output_var} "${_aliased}" PARENT_SCOPE)
            message(VERBOSE "Resolved ALIAS ${target_name} -> ${_aliased}")
        else()
            set(${output_var} "${target_name}" PARENT_SCOPE)
        endif()
    else()
        set(${output_var} "${target_name}" PARENT_SCOPE)
    endif()
endfunction()

# Override CMaizeTarget constructor to resolve ALIAS targets immediately
cpp_member(ctor CMaizeTarget desc)
function("${ctor}" self my_target)
    
    # Resolve ALIAS to real target before storing
    _resolve_alias_target(resolved_target "${my_target}")
    
    # Store the resolved target name
    CMaizeTarget(SET "${self}" target "${resolved_target}")
    
endfunction()

# Override CMaizeTarget has_property to handle ALIAS targets
cpp_member(has_property CMaizeTarget desc desc)
function("${has_property}" self has_property property_name)

    CMaizeTarget(target "${self}" my_name)

    # Check if the target exists before trying to get properties
    if(NOT TARGET "${my_name}")
        set("${has_property}" FALSE)
        cpp_return("${has_property}")
    endif()

    # Resolve ALIAS targets to real targets
    _resolve_alias_target(resolved_name "${my_name}")

    # Call CMake's built-in `get_target_property()` method.
    get_target_property(property_value "${resolved_name}" "${property_name}")

    # 'get_target_property()' returns either an empty string or the given
    # return variable name with "-NOTFOUND" appended when the property is
    # not found.
    if("${property_value}" STREQUAL "" OR property_value MATCHES "NOTFOUND")
        set("${has_property}" FALSE)
    else()
        set("${has_property}" TRUE)
    endif()

    cpp_return("${has_property}")

endfunction()

# Override CMaizeTarget get_property to handle ALIAS targets  
cpp_member(get_property CMaizeTarget desc desc)
function("${get_property}" self property_value property_name)

    CMaizeTarget(target "${self}" my_name)

    # Check if the target exists before trying to get properties
    if(NOT TARGET "${my_name}")
        cpp_raise(PropertyNotFound "Property not found: ${property_name} (target ${my_name} does not exist)")
    endif()

    CMaizeTarget(has_property "${self}" prop_exists "${property_name}")
    if(NOT prop_exists)
        cpp_raise(PropertyNotFound "Property not found: ${property_name}")
    endif()

    # Resolve ALIAS targets to real targets
    _resolve_alias_target(resolved_name "${my_name}")

    # Call CMake's built-in `get_target_property()` method.
    get_target_property("${property_value}" "${resolved_name}" "${property_name}")

    cpp_return("${property_value}")

endfunction()

# Override cmaize_find_or_build_optional_dependency to handle ALIAS targets
function(cmaize_find_or_build_optional_dependency _cfobod_name _cfobod_flag)

    _check_optional_flag("${_cfobod_flag}")

    # Get the top CMaize project
    cpp_get_global(_cfobod_top_proj_obj CMAIZE_TOP_PROJECT)

    set(_cfobod_tgt_obj "")
    if("${${_cfobod_flag}}")
        message(VERBOSE "\"${_cfobod_name}\" enabled with ${_cfobod_flag} = ${${_cfobod_flag}}")

        # Find or build the dependency target
        cmaize_find_or_build_dependency("${_cfobod_name}" ${ARGN})

        # Get the target from the project
        CMaizeProject(get_target
            "${_cfobod_top_proj_obj}"
            _cfobod_tgt_obj
            "${_cfobod_name}"
            ALL
        )

        if("${_cfobod_tgt_obj}" STREQUAL "")
            cpp_raise(TargetNotFound
                "Target was not found in the project, but it should have been \
                added by cmaize_find_or_build_dependency()!"
            )
        endif()

        # Get the target name (already resolved by CMaizeTarget ctor)
        CMaizeTarget(target "${_cfobod_tgt_obj}" _cfobod_tgt_name)
        
        # Add the compile definition
        target_compile_definitions(
            "${_cfobod_tgt_name}" INTERFACE "${_cfobod_flag}"
        )
    else()
        message(VERBOSE "\"${_cfobod_name}\" disabled with ${_cfobod_flag} = ${${_cfobod_flag}}")

        # Check if target already exists
        CMaizeProject(check_target
            "${_cfobod_top_proj_obj}"
            _cfobod_tgt_found
            "${_cfobod_name}"
            ALL
        )

        # Create dummy target if needed
        if(NOT "${_cfobod_tgt_found}")
            CMaizeTarget(ctor
                _cfobod_tgt_obj
                "${_cfobod_name}"
            )

            CMaizeProject(add_target
                "${_cfobod_top_proj_obj}"
                "${_cfobod_name}"
                "${_cfobod_tgt_obj}"
            )

            if(NOT TARGET "${_cfobod_name}")
                add_library("${_cfobod_name}" INTERFACE)
            endif()
        endif()
    endif()

endfunction()

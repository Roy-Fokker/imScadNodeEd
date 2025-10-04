# get platform specific configs
if (MSVC)
	include(${CMAKE_MODULE_PATH}/Win32Presets.cmake)
endif ()

# name of this application/library
set(PRJ_APP_NAME "sned")

# create executable for initial setup
add_executable(${PRJ_APP_NAME})

# ensure it's Standard C++ 23
target_compile_features(${PRJ_APP_NAME} PRIVATE cxx_std_23)

# Shader Binary type
# 	Implemented options, SPIRV or DXIL
set(SHADER_FORMAT "DXIL")

# set preprocessor defines
target_compile_definitions(${PRJ_APP_NAME}
	PRIVATE
		$<$<CONFIG:Debug>:DEBUG>    # Ensure DEBUG macro is defined in debug build
		${platform_definitions}     # Get platform specific definitions
		${SHADER_FORMAT}            # Shader format we want to use for SDL GPU
)

# precompiled headers
target_precompile_headers(${PRJ_APP_NAME}
	PRIVATE
		pch_std.hpp
		pch.hpp
)

# source files for this application
target_sources(${PRJ_APP_NAME}
	# Non C++ module source files
	PRIVATE
		main.cpp

	# magic encantations for C++ Modules Support in CMake
	PRIVATE FILE_SET app_modules TYPE CXX_MODULES FILES
		helper.cppm
		sdl.cppm
		imgui.cppm
		application.cppm
		nodes.cppm
		nodes_constants.cppm
		nodes_operators.cppm
)

# libraries used by this application
target_link_libraries(${PRJ_APP_NAME}
	PRIVATE
		SDL3::SDL3      # SDL3 Library
		imgui::imgui    # Dear Imgui library
		imgui::nodeflow # ImNodeFlow library
)

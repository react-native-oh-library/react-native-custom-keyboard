cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

set(rnoh_custom_keyboard_package_generated_dir "${RNOH_GENERATED_DIR}/_react_native_oh_tpl_react_native_custom_keyboard")
file(GLOB_RECURSE rnoh_custom_keyboard_package_generated_SRC "${rnoh_custom_keyboard_package_generated_dir}/**/*.cpp")
file(GLOB rnoh_custom_keyboard_package_SRC CONFIGURE_DEPENDS *.cpp)
add_library(rnoh_custom_keyboard_package SHARED ${rnoh_custom_keyboard_package_SRC} ${rnoh_custom_keyboard_package_generated_SRC})
target_include_directories(rnoh_custom_keyboard_package PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${rnoh_custom_keyboard_package_generated_dir})
target_link_libraries(rnoh_custom_keyboard_package PUBLIC rnoh)

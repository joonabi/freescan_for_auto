if (CMAKE_VERSION VERSION_LESS 3.14)
    message(FATAL_ERROR "freescansdk requires at least CMake version 3.14.0")
endif()

get_filename_component(_freescansdk_install_prefix "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

add_library(FreeScanSDK INTERFACE IMPORTED)

set_property(TARGET FreeScanSDK PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<CONFIG:Release>:${_freescan_install_prefix}/lib/SDKCWrapper.lib>
)

set_property(TARGET FreeScanSDKSDK PROPERTY INTERFACE_INCLUDE_DIRECTORIES
    ${_freescan_install_prefix}/include
)

set(FreeScanSDKSDK_BIN_DIR ${_freescan_install_prefix}/bin)
set(FreeScanSDKSDK_LIB_DIR ${_freescan_install_prefix}/lib)
set(FreeScanSDKSDK_INC_DIR ${_freescan_install_prefix}/include)

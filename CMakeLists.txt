cmake_minimum_required(VERSION 2.8.8)
project(Concurrency)

#should maybe check whether chosen compiler supports c++11

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11")
endif(UNIX)

include(GNUInstallDirs)

add_subdirectory(lib)
add_subdirectory(tests)

# Create ConcurrencyConfig.cmake
set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/Concurrency)
install(EXPORT ConcurrencyTargets
        FILE ConcurrencyTargets.cmake
        NAMESPACE Concurrency::
        DESTINATION ${ConfigPackageLocation})

include(CMakePackageConfigHelpers)
configure_package_config_file(ConcurrencyConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyConfig.cmake
                              INSTALL_DESTINATION ${ConfigPackageLocation}
                              PATH_VARS CMAKE_INSTALL_INCLUDEDIR)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyConfigVersion.cmake
                                    VERSION 0.1
                                    COMPATIBILITY ExactVersion )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/ConcurrencyConfigVersion.cmake
              DESTINATION ${ConfigPackageLocation})

# Uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
cmake_minimum_required(VERSION 3.25)

if (VCPKG_TARGET_ANDROID)
    # If we are building for Android, we must load vcpkg_android.cmake before the project() declaration.
    # This ensures that the CMAKE_TOOLCHAIN_FILE is set correctly.
    # (we cannot set CMAKE_TOOLCHAIN_FILE from Gradle, unfortunately, so this is the only place we can do it.)
    include("UI/Android/vcpkg_android.cmake")
endif()

# vcpkg flags depend on what linker we are using
include("Meta/CMake/use_linker.cmake")

if (APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    if (ENABLE_SWIFT)
        set(CMAKE_OSX_DEPLOYMENT_TARGET 15.0)
    else()
        set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0)
    endif()
endif()

# Pass additional information to vcpkg toolchain files if we are using vcpkg.
if (CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake$")
    set(CMAKE_PROJECT_ladybird_INCLUDE_BEFORE "Meta/CMake/vcpkg/generate_vcpkg_toolchain_variables.cmake")
endif()

if (APPLE AND NOT CMAKE_OSX_SYSROOT)
    set(CMAKE_OSX_SYSROOT macosx)
endif()

project(ladybird
        VERSION 0.1.0
        LANGUAGES C CXX
        DESCRIPTION "Ladybird Web Browser"
        HOMEPAGE_URL "https://ladybird.org"
)

if (ANDROID OR IOS)
    set(BUILD_SHARED_LIBS OFF)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LADYBIRD_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${LADYBIRD_SOURCE_DIR}/Meta/CMake")

include(UI/cmake/EnableLagom.cmake)
include(lagom_options NO_POLICY_SCOPE)
include(lagom_compile_options)

if (HAIKU)
    # Haiku needs some extra compile definitions to make important stuff in its headers available.
    add_compile_definitions(_DEFAULT_SOURCE)
    add_compile_definitions(_GNU_SOURCE)
    add_compile_definitions(__USE_GNU)
endif()

add_cxx_compile_options(-Wno-expansion-to-defined)
add_cxx_compile_options(-Wno-user-defined-literals)

if (ANDROID AND ENABLE_QT)
    message(STATUS "Disabling Qt for Android")
    set(ENABLE_QT OFF CACHE BOOL "" FORCE)
endif()

if (ENABLE_QT AND ENABLE_GUI_TARGETS AND (NOT WIN32 OR NOT ENABLE_WINDOWS_CI))
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
endif()

# We need to find OpenSSL in order to link it explicitly with all targets.
find_package(OpenSSL REQUIRED)

include(CTest) # for BUILD_TESTING option, default ON

if (ENABLE_GUI_TARGETS AND (NOT WIN32 OR NOT ENABLE_WINDOWS_CI))
    add_subdirectory(Services)
    add_subdirectory(UI)
endif()

add_custom_target(lint-shell-scripts
    COMMAND "${ladybird_SOURCE_DIR}/Meta/lint-shell-scripts.sh"
    USES_TERMINAL
)

find_package(Python3 COMPONENTS Interpreter)
if (Python3_FOUND)
    add_custom_target(check-style
        COMMAND ${Python3_EXECUTABLE} "${ladybird_SOURCE_DIR}/Meta/check-style.py"
        USES_TERMINAL
    )
endif()

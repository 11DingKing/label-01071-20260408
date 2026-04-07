cmake_minimum_required(VERSION 3.16)

project(ScreenshotTool VERSION 1.0.0 LANGUAGES CXX)

# ===== C++ Standard =====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== Qt Configuration =====
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# ===== Find Qt6 =====
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

# ===== Source Files =====
set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp
    src/StartWindow.h
    src/StartWindow.cpp
    src/ScreenCapture.h
    src/ScreenCapture.cpp
    src/SelectionOverlay.h
    src/SelectionOverlay.cpp
    src/ToolBar.h
    src/ToolBar.cpp
    src/Logger.h
    src/Logger.cpp
    src/StyleManager.h
    src/StyleManager.cpp
)

# ===== Resources =====
set(PROJECT_RESOURCES
    resources/resources.qrc
)

# ===== Create Executable =====
if(APPLE)
    # macOS bundle
    set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.screenshot.tool")
    set(MACOSX_BUNDLE_BUNDLE_NAME "ScreenshotTool")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
    )
elseif(WIN32)
    # Windows GUI application (no console)
    add_executable(${PROJECT_NAME} WIN32
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
    )
else()
    # Linux
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_RESOURCES}
    )
endif()

# ===== Link Libraries =====
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# ===== Include Directories =====
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ===== Compiler Options =====
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ===== Installation =====
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ===== Qt Deploy (for packaging) =====
if(Qt6_VERSION VERSION_GREATER_EQUAL "6.3.0")
    qt_generate_deploy_app_script(
        TARGET ${PROJECT_NAME}
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()

# ===== Print Configuration =====
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt Version: ${Qt6_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "========================================")

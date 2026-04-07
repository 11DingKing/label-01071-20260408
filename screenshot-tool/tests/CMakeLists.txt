cmake_minimum_required(VERSION 3.16)

project(ScreenshotToolTests LANGUAGES CXX)

# ===== C++ Standard =====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== Qt Configuration =====
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ===== Find Qt6 =====
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Test
)

# ===== Include Source Directory =====
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# ===== Test: ScreenCapture =====
add_executable(test_screen_capture
    test_screen_capture.cpp
    ${SRC_DIR}/ScreenCapture.cpp
    ${SRC_DIR}/ScreenCapture.h
)

target_include_directories(test_screen_capture PRIVATE ${SRC_DIR})
target_link_libraries(test_screen_capture PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)

# ===== Test: Logger =====
add_executable(test_logger
    test_logger.cpp
    ${SRC_DIR}/Logger.cpp
    ${SRC_DIR}/Logger.h
)

target_include_directories(test_logger PRIVATE ${SRC_DIR})
target_link_libraries(test_logger PRIVATE
    Qt6::Core
    Qt6::Test
)

# ===== Test: StyleManager =====
add_executable(test_style_manager
    test_style_manager.cpp
    ${SRC_DIR}/StyleManager.cpp
    ${SRC_DIR}/StyleManager.h
)

target_include_directories(test_style_manager PRIVATE ${SRC_DIR})
target_link_libraries(test_style_manager PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)

# ===== Test: SelectionOverlay =====
add_executable(test_selection_overlay
    test_selection_overlay.cpp
    ${SRC_DIR}/SelectionOverlay.cpp
    ${SRC_DIR}/SelectionOverlay.h
    ${SRC_DIR}/ToolBar.cpp
    ${SRC_DIR}/ToolBar.h
)

target_include_directories(test_selection_overlay PRIVATE ${SRC_DIR})
target_link_libraries(test_selection_overlay PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Test
)

# ===== Enable Testing =====
enable_testing()

add_test(NAME TestScreenCapture COMMAND test_screen_capture)
add_test(NAME TestLogger COMMAND test_logger)
add_test(NAME TestStyleManager COMMAND test_style_manager)
add_test(NAME TestSelectionOverlay COMMAND test_selection_overlay)

# ===== Print Configuration =====
message(STATUS "========================================")
message(STATUS "Tests Configuration")
message(STATUS "Qt Version: ${Qt6_VERSION}")
message(STATUS "========================================")

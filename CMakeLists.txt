cmake_minimum_required(VERSION 3.16)

set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "F:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(nebula-music VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6     REQUIRED COMPONENTS Quick Multimedia Sql Core)
find_package(utf8cpp CONFIG REQUIRED)
find_package(ZLIB    REQUIRED)
find_package(taglib  CONFIG REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appnebula-music
    main.cpp
)

qt_add_qml_module(appnebula-music
    URI nebula-music
    VERSION 1.0
    QML_FILES
        Main.qml
        QML_FILES
        QML_FILES Consts.js
        QML_FILES Navbar.qml
        RESOURCES
        QML_FILES Seperator.qml
        QML_FILES Navbar_button.qml

    RESOURCES
    icons/CupertinoIcons.ttf
    test_assets/cover.jpg
    QML_FILES Songs_page.qml
    QML_FILES Player.qml
    QML_FILES Song_item.qml
    QML_FILES Player_controls.qml
    SOURCES
    SOURCES player.h player.cpp
    SOURCES database.h database.cpp
    QML_FILES Songs_header.qml
    QML_FILES Songs_title.qml
    QML_FILES Now_playing.qml
    SOURCES
    SOURCES songmodel.h songmodel.cpp
    QML_FILES Queue.qml
    QML_FILES Queue_item.qml
    QML_FILES Queue_item_draggable.qml
    QML_FILES Queue_tile.qml
    SOURCES tag_reader.h tag_reader.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appnebula-music PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appnebula-music
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appnebula-music
    PRIVATE Qt6::Quick Qt6::Multimedia Qt6::Sql Qt6::Core TagLib::TagLib TagLib::tag utf8::cpp utf8cpp::utf8cpp ZLIB::ZLIB
)

include(GNUInstallDirs)
install(TARGETS appnebula-music
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

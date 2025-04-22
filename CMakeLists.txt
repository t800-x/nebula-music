cmake_minimum_required(VERSION 3.16)

project(nebula-music VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia)

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
    icons/add.svg
    icons/close.svg
    icons/home.svg
    icons/search.svg
    icons/settings.svg
    icons/music.svg
    icons/next.svg
    icons/pause.svg
    icons/play.svg
    icons/prev.svg
    test_assets/cover.jpg
    QML_FILES Songs_page.qml
    QML_FILES Player.qml
    QML_FILES Song_item.qml
    QML_FILES Player_controls.qml
    SOURCES
    SOURCES player.h player.cpp

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
    PRIVATE Qt6::Quick Qt6::Multimedia
)

include(GNUInstallDirs)
install(TARGETS appnebula-music
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

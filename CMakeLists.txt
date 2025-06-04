cmake_minimum_required(VERSION 3.16)

set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "F:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project(nebula-music VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6     REQUIRED COMPONENTS Quick Multimedia Sql Core Concurrent Core5Compat)
find_package(utf8cpp CONFIG REQUIRED)
find_package(ZLIB    REQUIRED)
find_package(taglib  CONFIG REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appnebula-music
    main.cpp
)

target_include_directories(appnebula-music
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

qt_add_qml_module(appnebula-music
    URI nebula-music
    VERSION 1.0
    QML_FILES
        Main.qml
        Consts.js

        test.qml

        ui/nav/Navbar.qml
        ui/nav/Navbar_button.qml

        ui/utils/Acrylic.qml
        ui/utils/Acrylic_native.qml
        ui/utils/Seperator.qml
        ui/utils/CustomButton.qml
        ui/utils/Button1.qml
        ui/utils/IconButton.qml
        ui/utils/TextBox.qml
        ui/utils/BackButton.qml
        ui/utils/Pic.qml
        ui/utils/PlaybackButton.qml
        ui/utils/NormalButton.qml

        ui/player/Player.qml
        ui/player/Player_controls.qml
        ui/player/Now_playing.qml

        ui/songs_page/Songs_page.qml
        ui/songs_page/Songs_header.qml
        ui/songs_page/Songs_title.qml
        ui/songs_page/Song_item.qml

        ui/queue/Queue.qml
        ui/queue/Queue_item.qml
        ui/queue/Queue_tile.qml

        ui/lyrics/LyricsPane.qml
        ui/lyrics/LyricItem.qml

        ui/albums_page/AlbumsPage.qml
        ui/albums_page/AlbumItem.qml
        ui/albums_page/AlbumHeader.qml
        ui/albums_page/AlButton.qml
        ui/albums_page/AlbumView.qml
        ui/albums_page/ListDelegate.qml
    SOURCES
        src/player.cpp src/headers/player.h
        src/database.cpp src/headers/database.h
        src/tag_reader.cpp src/headers/tag_reader.h
        src/models/songmodel.cpp src/models/songmodel.h
        src/models/SyncedLyrics.h
        src/models/syncedlyricsmodel.h src/models/syncedlyricsmodel.cpp
        src/headers/eventbus.h src/eventbus.cpp
        src/models/songfilterproxymodel.h src/models/songfilterproxymodel.cpp
    RESOURCES
        icons/CupertinoIcons.ttf
        icons/placeholder.png
        Main.qml
        SOURCES src/models/albummodel.h src/models/albummodel.cpp
        SOURCES src/models/albumfilterproxymodel.h src/models/albumfilterproxymodel.cpp
        QML_FILES ui/utils/SongItemContextMenu.qml
)

qt6_add_resources(appnebula-music resources.qrc)

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
    PRIVATE Qt6::Quick Qt6::Multimedia Qt6::Sql Qt6::Core Qt6::Concurrent
            TagLib::TagLib TagLib::tag utf8::cpp utf8cpp::utf8cpp ZLIB::ZLIB
            Qt6::Core5Compat
)

# On Windows, link dwmapi
if (WIN32)
    target_link_libraries(appnebula-music PRIVATE dwmapi)
endif()

include(GNUInstallDirs)
install(TARGETS appnebula-music
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

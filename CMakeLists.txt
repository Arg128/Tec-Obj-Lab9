cmake_minimum_required(VERSION 3.16)

project(managmentApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Sql)

qt_standard_project_setup(REQUIRES 6.8)

# Content of downloaded design from Figma to Qt or similar
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appmanagmentApp
    main.cpp
)

qt_add_qml_module(appmanagmentApp
    URI managmentApp
    QML_FILES
        Main.qml
        SOURCES models/activity.cpp
        SOURCES models/activityfactory.h models/activityfactory.cpp
        SOURCES models/activity.h
        SOURCES models/task.h models/task.cpp
        SOURCES models/project.h models/project.cpp
        SOURCES models/exam.h models/exam.cpp
        SOURCES models/submission.h models/submission.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appmanagmentApp PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appmanagmentApp
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appmanagmentApp
    PRIVATE Qt6::Quick Qt6::Sql
)

install(TARGETS appmanagmentApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

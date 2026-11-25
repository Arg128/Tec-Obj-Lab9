cmake_minimum_required(VERSION 3.16)

project(managmentApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# Activa AUTOMOC, AUTOUIC y AUTORCC
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appmanagmentApp
    main.cpp

    # Modelo
    models/activity.h models/activity.cpp
    models/activityfactory.h models/activityfactory.cpp
    models/task.h models/task.cpp
    models/exam.h models/exam.cpp
    models/project.h models/project.cpp
    models/submission.h models/submission.cpp
    models/IRepository.h
    models/legacycsvsource.h models/legacycsvsource.cpp
    models/csvlegacyadapter.h models/csvlegacyadapter.cpp
    models/sqliteactivityrepository.h models/sqliteactivityrepository.cpp
    models/activityservicefacade.h models/activityservicefacade.cpp

    # Vistas (Qt Designer + clases)
    views/login.ui
    views/dashboard.ui
    views/actividad_form.ui
    views/actividades_curso.ui
    views/viewtypes.h
    views/loginview.h views/loginview.cpp
    views/dashboardview.h views/dashboardview.cpp
    views/actividadformview.h views/actividadformview.cpp
    views/actividadescursoview.h views/actividadescursoview.cpp

    # Controlador
    controllers/actividadcontroller.h controllers/actividadcontroller.cpp
)

target_include_directories(appmanagmentApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

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
    PRIVATE Qt6::Widgets Qt6::Sql
)

install(TARGETS appmanagmentApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

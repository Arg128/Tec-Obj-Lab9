#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>

#include "models/sqliteactivityrepository.h"
#include "models/activityservicefacade.h"
#include "views/loginview.h"
#include "views/dashboardview.h"
#include "views/dashboardestudianteview.h"
#include "views/actividadformview.h"
#include "views/actividadescursoview.h"
#include "controllers/actividadcontroller.h"

// Raíz de composición: aquí se crean y conectan todas las piezas.
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("SeguimientoAcademico");

    // --- Modelo: base de datos SQLite + repositorio + facade ---
    // La base vive en <proyecto>/data/seguimiento.db (visible y versionable).
    // Se puede cambiar con la variable de entorno SEGUIMIENTO_DATA_DIR.
    // Si esa carpeta no es escribible, se usa la carpeta de datos de la aplicación.
    QString carpetaDatos = qEnvironmentVariable("SEGUIMIENTO_DATA_DIR");
#ifdef APP_DATA_DIR
    if (carpetaDatos.isEmpty()) {
        carpetaDatos = QStringLiteral(APP_DATA_DIR);
    }
#endif
    if (carpetaDatos.isEmpty() || !QDir().mkpath(carpetaDatos)
        || !QFileInfo(carpetaDatos).isWritable()) {
        carpetaDatos = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(carpetaDatos);
    }
    qInfo() << "Base de datos SQLite:" << carpetaDatos + "/seguimiento.db";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(carpetaDatos + "/seguimiento.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Base de datos",
                              "No se pudo abrir la base de datos:\n" + db.lastError().text());
        return 1;
    }

    SqliteActivityRepository repositorio(db);
    if (!repositorio.initializeSchema()) {
        QMessageBox::critical(nullptr, "Base de datos", "No se pudo crear el esquema de la base de datos.");
        return 1;
    }
    ActivityServiceFacade facade(&repositorio);

    // --- Vistas ---
    LoginView login;
    DashboardView dashboard;
    DashboardEstudianteView estudiante;
    ActividadFormView formulario;
    ActividadesCursoView actividades;

    // --- Controlador: conecta vistas y modelo ---
    ActividadController controlador(&facade, &login, &dashboard, &estudiante, &formulario, &actividades);
    controlador.setCursos({
        "Tecnología de Objetos", "Bases de Datos",
        "Estructuras de Datos", "Ingeniería de Software"
    });
    controlador.setEstudiantes({"20231001", "20231002"}); // CUIs de ejemplo
    controlador.iniciar();

    return app.exec();
}

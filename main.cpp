#include <QApplication>
#include <QDir>
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
    const QString carpetaDatos = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(carpetaDatos);

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

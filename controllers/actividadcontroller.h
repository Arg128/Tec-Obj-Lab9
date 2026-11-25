#ifndef ACTIVIDADCONTROLLER_H
#define ACTIVIDADCONTROLLER_H

#include <QObject>
#include <QStringList>
#include "models/activity.h"
#include "views/viewtypes.h"

class LoginView;
class DashboardView;
class ActividadFormView;
class ActividadesCursoView;
class ActivityServiceFacade;

// Controlador (MVC): conecta las señales de las vistas con el facade del modelo.
// - Las vistas no conocen el modelo.
// - El modelo no conoce las vistas.
// - Aquí se traducen los datos de la vista a los del modelo y viceversa.
class ActividadController : public QObject {
    Q_OBJECT
public:
    ActividadController(ActivityServiceFacade *facade,
                        LoginView *login,
                        DashboardView *dashboard,
                        ActividadFormView *formulario,
                        ActividadesCursoView *actividades,
                        QObject *parent = nullptr);

    void setCursos(const QStringList &cursos);
    void setEstudiantes(const QStringList &cuis);

    void iniciar(); // muestra la primera pantalla (login)

private slots:
    // Eventos de la vista
    void onLoginSolicitado(const QString &usuario, const QString &password);
    void onCrearActividadSolicitada(TipoVista tipo);
    void onGuardarSolicitado(const ActividadFormData &datos);
    void onCancelarSolicitado();
    void onCursoSeleccionado(const QString &curso);
    void onVolverSolicitado();
    void onImportarCsvSolicitado(const QString &ruta);

    // Respuestas del modelo (facade)
    void onActividadPublicada(Activity *actividad, int estudiantesNotificados);
    void onOperacionFallida(const QString &motivo);
    void onImportacionTerminada(int importadas, int omitidas);

private:
    ActivityServiceFacade *m_facade;
    LoginView *m_login;
    DashboardView *m_dashboard;
    ActividadFormView *m_formulario;
    ActividadesCursoView *m_actividades;

    QStringList m_cursos;
    QStringList m_estudiantes;
};

#endif // ACTIVIDADCONTROLLER_H

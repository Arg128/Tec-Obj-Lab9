#ifndef ACTIVIDADCONTROLLER_H
#define ACTIVIDADCONTROLLER_H

#include <QObject>
#include <QStringList>
#include "models/activity.h"
#include "views/viewtypes.h"

class LoginView;
class DashboardView;
class ActividadFormView;
class ActivityServiceFacade;

// Controlador (MVC): conecta las señales de las vistas con el facade del modelo.
// - Las vistas no conocen el modelo.
// - El modelo no conoce las vistas.
// - Aquí se traducen los datos de la vista (ActividadFormData) a los del modelo.
class ActividadController : public QObject {
    Q_OBJECT
public:
    ActividadController(ActivityServiceFacade *facade,
                        LoginView *login,
                        DashboardView *dashboard,
                        ActividadFormView *formulario,
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

    // Respuestas del modelo (facade)
    void onActividadPublicada(Activity *actividad, int estudiantesNotificados);
    void onOperacionFallida(const QString &motivo);

private:
    ActivityServiceFacade *m_facade;
    LoginView *m_login;
    DashboardView *m_dashboard;
    ActividadFormView *m_formulario;

    QStringList m_cursos;
    QStringList m_estudiantes;
};

#endif // ACTIVIDADCONTROLLER_H

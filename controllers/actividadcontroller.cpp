#include "actividadcontroller.h"
#include "models/activityservicefacade.h"
#include "views/loginview.h"
#include "views/dashboardview.h"
#include "views/dashboardestudianteview.h"
#include "views/actividadformview.h"
#include "views/actividadescursoview.h"
#include "models/task.h"
#include "models/exam.h"
#include "models/project.h"
#include <QUrl>

// Traduce el tipo de la vista al tipo del modelo
static Activity::Type aTipoModelo(TipoVista tipo)
{
    switch (tipo) {
    case TipoVista::Tarea:    return Activity::Type::Task;
    case TipoVista::Examen:   return Activity::Type::Exam;
    case TipoVista::Proyecto: return Activity::Type::Project;
    }
    return Activity::Type::Task;
}

// Traduce el tipo del modelo al texto que muestra la vista
static QString nombreTipo(Activity::Type tipo)
{
    switch (tipo) {
    case Activity::Type::Task:    return "Tarea";
    case Activity::Type::Exam:    return "Examen";
    case Activity::Type::Project: return "Proyecto";
    }
    return "Actividad";
}

// Datos propios de cada tipo de actividad, en texto para la vista
static QString detalleTipo(const Activity *actividad)
{
    if (const auto *t = qobject_cast<const Task*>(actividad)) {
        return QString("Entrega tardía: %1").arg(t->allowLate ? "permitida" : "no permitida");
    }
    if (const auto *e = qobject_cast<const Exam*>(actividad)) {
        return QString("Duración: %1 min").arg(e->durationMinutes);
    }
    if (const auto *p = qobject_cast<const Project*>(actividad)) {
        QString texto = QString("Formato de entrega: %1").arg(p->submissionFormat);
        if (!p->teamMemberIds.isEmpty()) {
            texto += QString(" · Integrantes: %1").arg(p->teamMemberIds.join(", "));
        }
        return texto;
    }
    return QString();
}

// Traduce una entrega del modelo a una fila para el dashboard del estudiante
static EntregaFila aFilaEntrega(const StudentAssignment &a)
{
    EntregaFila f;
    f.submissionId = a.submission->id;
    f.curso = a.activity->courseId;
    f.tipo = nombreTipo(a.activity->type());
    f.titulo = a.activity->title;
    f.fechaLimite = a.activity->dueDate;
    f.vencida = a.activity->isOverdue();
    f.nota = a.submission->grade;
    switch (a.submission->status) {
    case Submission::Status::Pending:   f.estado = EstadoEntrega::Pendiente;  break;
    case Submission::Status::Submitted: f.estado = EstadoEntrega::Entregada;  break;
    case Submission::Status::Graded:    f.estado = EstadoEntrega::Calificada; break;
    }
    return f;
}

// Traduce los datos del formulario a las claves que espera la fábrica del modelo
static QVariantMap aDatosModelo(const ActividadFormData &d)
{
    QVariantMap datos;
    datos["course_id"] = d.curso;
    datos["title"] = d.titulo;
    datos["markdown_content"] = d.notasMarkdown;
    datos["due_date"] = d.fechaEntrega.toString(Qt::ISODate);

    // Enlaces + archivos adjuntos (como URL local) forman los recursos de la actividad
    QStringList recursos = d.enlaces;
    for (const QString &ruta : d.archivos) {
        recursos << QUrl::fromLocalFile(ruta).toString();
    }
    datos["resource_urls"] = recursos;

    // Campos propios de cada tipo
    switch (d.tipo) {
    case TipoVista::Tarea:
        datos["allow_late"] = d.entregaTardia;
        break;
    case TipoVista::Examen:
        datos["duration_minutes"] = d.duracionMinutos;
        break;
    case TipoVista::Proyecto:
        datos["submission_format"] = d.formatoEntrega;
        datos["team_member_ids"] = d.integrantes;
        break;
    }
    return datos;
}

ActividadController::ActividadController(ActivityServiceFacade *facade,
                                         LoginView *login,
                                         DashboardView *dashboard,
                                         DashboardEstudianteView *estudiante,
                                         ActividadFormView *formulario,
                                         ActividadesCursoView *actividades,
                                         QObject *parent)
    : QObject(parent),
      m_facade(facade),
      m_login(login),
      m_dashboard(dashboard),
      m_estudiante(estudiante),
      m_formulario(formulario),
      m_actividades(actividades)
{
    // Vista -> Controlador
    connect(m_login, &LoginView::loginSolicitado,
            this, &ActividadController::onLoginSolicitado);
    connect(m_dashboard, &DashboardView::crearActividadSolicitada,
            this, &ActividadController::onCrearActividadSolicitada);
    connect(m_dashboard, &DashboardView::cursoSeleccionado,
            this, &ActividadController::onCursoSeleccionado);
    connect(m_dashboard, &DashboardView::cerrarSesionSolicitado,
            this, &ActividadController::onCerrarSesionSolicitadoDocente);
    connect(m_dashboard, &DashboardView::importarCsvSolicitado,
            this, &ActividadController::onImportarCsvSolicitado);
    connect(m_estudiante, &DashboardEstudianteView::entregarSolicitado,
            this, &ActividadController::onEntregarSolicitado);
    connect(m_estudiante, &DashboardEstudianteView::cerrarSesionSolicitado,
            this, &ActividadController::onCerrarSesionSolicitado);
    connect(m_formulario, &ActividadFormView::guardarSolicitado,
            this, &ActividadController::onGuardarSolicitado);
    connect(m_formulario, &ActividadFormView::cancelarSolicitado,
            this, &ActividadController::onCancelarSolicitado);
    connect(m_actividades, &ActividadesCursoView::volverSolicitado,
            this, &ActividadController::onVolverSolicitado);

    // Modelo (facade) -> Controlador
    connect(m_facade, &ActivityServiceFacade::activityPublished,
            this, &ActividadController::onActividadPublicada);
    connect(m_facade, &ActivityServiceFacade::operationFailed,
            this, &ActividadController::onOperacionFallida);
    connect(m_facade, &ActivityServiceFacade::importFinished,
            this, &ActividadController::onImportacionTerminada);
    connect(m_facade, &ActivityServiceFacade::assignmentSubmitted,
            this, &ActividadController::onEntregaRegistrada);
    connect(m_facade, &ActivityServiceFacade::submissionFailed,
            this, &ActividadController::onEntregaFallida);
}

void ActividadController::setCursos(const QStringList &cursos)
{
    m_cursos = cursos;
}

void ActividadController::setEstudiantes(const QStringList &cuis)
{
    m_estudiantes = cuis;
}

void ActividadController::iniciar()
{
    m_login->show();
}

void ActividadController::onLoginSolicitado(const QString &usuario, const QString &password)
{
    if (usuario.isEmpty() || password.isEmpty()) {
        m_login->mostrarError("Ingresa usuario y contraseña.");
        return;
    }

    // Marcador: la contraseña todavía no se valida (no existe un modelo de usuarios).
    // El rol se decide por el usuario: "docente" o el CUI de un estudiante matriculado.
    if (usuario.compare("docente", Qt::CaseInsensitive) == 0) {
        m_login->limpiar();
        m_login->hide();
        m_dashboard->mostrarCursos(m_cursos);
        m_dashboard->show();
        return;
    }

    if (m_estudiantes.contains(usuario)) {
        m_cuiActual = usuario;
        m_login->limpiar();
        m_login->hide();
        m_estudiante->setEstudiante(usuario);
        refrescarEntregas();
        m_estudiante->show();
        return;
    }

    m_login->mostrarError("Usuario no reconocido. Usa \"docente\" o el CUI de un estudiante.");
}

void ActividadController::onCrearActividadSolicitada(TipoVista tipo)
{
    m_formulario->limpiar();
    m_formulario->setCursos(m_cursos);
    m_formulario->setTipo(tipo);
    m_formulario->show();
}

void ActividadController::onGuardarSolicitado(const ActividadFormData &datos)
{
    // El resultado llega por las señales activityPublished / operationFailed
    m_facade->publishActivity(aTipoModelo(datos.tipo), aDatosModelo(datos), m_estudiantes);
}

void ActividadController::onCancelarSolicitado()
{
    m_formulario->limpiar();
    m_formulario->hide();
}

// Flujo "listar / cargar": pide las actividades al modelo y las convierte en filas para la vista
void ActividadController::onCursoSeleccionado(const QString &curso)
{
    QList<Activity*> actividades = m_facade->loadCourseActivities(curso);

    QList<ActividadFila> filas;
    for (const Activity *actividad : actividades) {
        ActividadFila fila;
        fila.tipo = nombreTipo(actividad->type());
        fila.titulo = actividad->title;
        fila.fechaEntrega = actividad->dueDate;
        fila.vencida = actividad->isOverdue();
        fila.recursos = actividad->resourceCount();
        fila.markdown = actividad->markdownContent;
        fila.listaRecursos = actividad->resourceUrls;
        fila.detalle = detalleTipo(actividad);
        filas << fila;
    }
    // El repositorio las crea con el facade como padre; ya no se necesitan
    qDeleteAll(actividades);

    m_actividades->mostrarActividades(curso, filas);
    m_dashboard->hide();
    m_actividades->show();
}

void ActividadController::onVolverSolicitado()
{
    m_actividades->hide();
    m_dashboard->show();
}

void ActividadController::onActividadPublicada(Activity *actividad, int estudiantesNotificados)
{
    m_formulario->limpiar();
    m_formulario->hide();
    m_dashboard->mostrarMensaje(
        QString("Actividad \"%1\" creada.\nEntregas pendientes asignadas: %2")
            .arg(actividad->title)
            .arg(estudiantesNotificados));
}

void ActividadController::onOperacionFallida(const QString &motivo)
{
    m_formulario->mostrarError(motivo);
}

// Flujo "importar": el controlador solo delega; el Adapter vive dentro del modelo
void ActividadController::onImportarCsvSolicitado(const QString &ruta)
{
    m_facade->importLegacyCsv(ruta, m_estudiantes);
}

void ActividadController::onImportacionTerminada(int importadas, int omitidas)
{
    m_dashboard->mostrarMensaje(
        QString("Importación del CSV heredado terminada.\n"
                "Actividades importadas: %1\n"
                "Registros omitidos (inválidos o repetidos): %2")
            .arg(importadas)
            .arg(omitidas));
}

// --- Dashboard del estudiante ------------------------------------------------

void ActividadController::refrescarEntregas()
{
    const QList<StudentAssignment> asignaciones = m_facade->loadStudentAssignments(m_cuiActual);

    QList<EntregaFila> filas;
    for (const StudentAssignment &a : asignaciones) {
        filas << aFilaEntrega(a);
    }
    // Ya se copiaron los datos que necesita la vista
    for (const StudentAssignment &a : asignaciones) {
        delete a.activity;
        delete a.submission;
    }

    m_estudiante->mostrarEntregas(filas);
}

void ActividadController::onEntregarSolicitado(const QString &submissionId, const QString &rutaArchivo)
{
    // El resultado llega por las señales assignmentSubmitted / submissionFailed
    m_facade->submitAssignment(m_cuiActual, submissionId, QUrl::fromLocalFile(rutaArchivo).toString());
}

void ActividadController::onEntregaRegistrada(const QString &)
{
    refrescarEntregas();
    m_estudiante->mostrarEstado("Entrega registrada correctamente.");
}

void ActividadController::onEntregaFallida(const QString &motivo)
{
    m_estudiante->mostrarError(motivo);
}

void ActividadController::onCerrarSesionSolicitado()
{
    m_cuiActual.clear();
    m_estudiante->hide();
    m_login->show();
}

void ActividadController::onCerrarSesionSolicitadoDocente(){
    m_dashboard->hide();
    m_login->show();
  }

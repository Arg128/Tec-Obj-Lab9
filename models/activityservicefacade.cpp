#include "activityservicefacade.h"
#include "activityfactory.h"
#include "project.h"
#include "csvlegacyadapter.h"

// A quién se le asignan entregas pendientes: a los matriculados, o solo al equipo si es un Proyecto
static QStringList targetStudents(const Activity *activity, const QStringList &enrolledStudentCuis) {
    if (const auto *project = qobject_cast<const Project*>(activity)) {
        return project->teamMemberIds;
    }
    return enrolledStudentCuis;
}

ActivityServiceFacade::ActivityServiceFacade(IRepository *repository, QObject *parent)
    : QObject(parent), m_repository(repository) {}

bool ActivityServiceFacade::publishActivity(Activity::Type type,
                                            const QVariantMap &formData,
                                            const QStringList &enrolledStudentCuis) {
    // 1. Creation via Factory Method
    Activity *activity = ActivityFactory::createActivity(type, formData, this);
    if (!activity) {
        emit operationFailed("No se pudo instanciar la actividad.");
        return false;
    }

    // 2. Domain Validation (Polymorphic isValid() check)
    if (!activity->isValid()) {
        emit operationFailed("Datos inválidos: verifique el título, curso o parámetros específicos.");
        activity->deleteLater();
        return false;
    }

    // 3. Persistence (Save Activity + Generate Pending Submissions for student CUIs)
    if (!m_repository->saveActivity(activity)) {
        emit operationFailed("Error al guardar la actividad en la base de datos.");
        activity->deleteLater();
        return false;
    }

    const QStringList targetCuis = targetStudents(activity, enrolledStudentCuis);

    if (!targetCuis.isEmpty() &&
        !m_repository->createPendingSubmissions(activity->id, targetCuis)) {
        emit operationFailed("Actividad creada, pero falló la asignación a los estudiantes.");
        activity->deleteLater();
        return false;
    }

    // 4. Notification (Emits signal to refresh Views / alert students)
    emit activityPublished(activity, targetCuis.size());
    return true;
}

QList<Activity*> ActivityServiceFacade::loadCourseActivities(const QString &courseId) {
    return m_repository->getActivitiesByCourse(courseId, this);
}

ActivityServiceFacade::ImportResult ActivityServiceFacade::importFrom(IRepository *source,
                                                                     const QStringList &enrolledStudentCuis) {
    ImportResult result;
    if (!source) return result;

    const QList<Activity*> activities = source->getAllActivities(this);
    for (Activity *activity : activities) {
        if (activity->isValid() && m_repository->saveActivity(activity)) {
            ++result.imported;
            // Sin entregas pendientes, los estudiantes no verían la actividad importada
            const QStringList targets = targetStudents(activity, enrolledStudentCuis);
            if (!targets.isEmpty()) {
                m_repository->createPendingSubmissions(activity->id, targets);
            }
        } else {
            ++result.skipped; // inválida o ya existente
        }
        activity->deleteLater();
    }
    return result;
}

void ActivityServiceFacade::importLegacyCsv(const QString &path, const QStringList &enrolledStudentCuis) {
    CsvLegacyAdapter adapter(path);
    ImportResult result = importFrom(&adapter, enrolledStudentCuis);
    result.skipped += adapter.malformedLines(); // líneas del CSV que no se pudieron convertir
    emit importFinished(result.imported, result.skipped);
}

QList<StudentAssignment> ActivityServiceFacade::loadStudentAssignments(const QString &studentCui) {
    return m_repository->getAssignmentsByStudent(studentCui, this);
}

bool ActivityServiceFacade::submitAssignment(const QString &studentCui,
                                             const QString &submissionId,
                                             const QString &fileUrl) {
    const QList<StudentAssignment> assignments = m_repository->getAssignmentsByStudent(studentCui, this);

    bool ok = false;
    QString reason = "No se encontró la entrega.";
    for (const StudentAssignment &a : assignments) {
        if (a.submission->id != submissionId) continue;

        if (a.submission->status != Submission::Status::Pending) {
            reason = "Esta actividad ya fue entregada.";
        } else if (!a.activity->canSubmit()) {
            reason = "El plazo de entrega ya venció.";
        } else {
            a.submission->markAsSubmitted(fileUrl);
            if (m_repository->saveSubmission(a.submission)) {
                ok = true;
            } else {
                reason = "No se pudo guardar la entrega.";
            }
        }
        break;
    }

    for (const StudentAssignment &a : assignments) {
        delete a.activity;
        delete a.submission;
    }

    if (ok) {
        emit assignmentSubmitted(submissionId);
    } else {
        emit submissionFailed(reason);
    }
    return ok;
}

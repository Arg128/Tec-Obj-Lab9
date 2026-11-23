#include "activityservicefacade.h"
#include "activityfactory.h"
#include "project.h"

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


    QStringList targetCuis = enrolledStudentCuis;
    if (auto *project = qobject_cast<Project*>(activity)) {
      targetCuis = project->teamMemberIds; // Only assign pending submissions to the project's team!
    }

    if (!targetCuis.isEmpty() &&
      !m_repository->createPendingSubmissions(activity->id, targetCuis)) {
      emit operationFailed("Actividad creada, pero falló la asignación a los estudiantes.");
      return false;
    }
    // 4. Notification (Emits signal to refresh Views / alert students)
    emit activityPublished(activity, enrolledStudentCuis.size());
    return true;

}

QList<Activity*> ActivityServiceFacade::loadCourseActivities(const QString &courseId) {
    return m_repository->getActivitiesByCourse(courseId, this);
}
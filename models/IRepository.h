#ifndef IREPOSITORY_H
#define IREPOSITORY_H
#include "activity.h"
#include "submission.h"
#include <QList>
#include <QString>

// Una entrega junto con la actividad a la que pertenece.
// Ambos objetos pertenecen a quien los pide (se crean con el parent indicado).
struct StudentAssignment {
    Activity *activity = nullptr;
    Submission *submission = nullptr;
};

class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool saveActivity(Activity *activity) = 0;
    virtual QList<Activity*> getActivitiesByCourse(const QString &courseId, QObject *parent = nullptr) = 0;
    virtual QList<Activity*> getAllActivities(QObject *parent = nullptr) = 0;
    virtual bool createPendingSubmissions(const QString &activityId, const QStringList &studentCuis) = 0;

    // Entregas de un estudiante (con su actividad), ordenadas por fecha límite
    virtual QList<StudentAssignment> getAssignmentsByStudent(const QString &studentCui, QObject *parent = nullptr) = 0;
    // Actualiza una entrega existente (archivo, fecha, nota y estado)
    virtual bool saveSubmission(Submission *submission) = 0;
};

#endif // IREPOSITORY_H

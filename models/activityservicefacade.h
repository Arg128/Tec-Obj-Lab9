#ifndef ACTIVITYSERVICEFACADE_H
#define ACTIVITYSERVICEFACADE_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>
#include "activity.h"
#include "IRepository.h"

class ActivityServiceFacade : public QObject {
    Q_OBJECT
public:
    explicit ActivityServiceFacade(IRepository *repository, QObject *parent = nullptr);

    // Single entry point: creates, validates, persists, assigns to students, and notifies
    bool publishActivity(Activity::Type type,
                         const QVariantMap &formData,
                         const QStringList &enrolledStudentCuis);

    QList<Activity*> loadCourseActivities(const QString &courseId);

signals:
    // Notification subsystem (Views/Controllers subscribe to these)
    void activityPublished(Activity *activity, int notifiedStudentsCount);
    void operationFailed(const QString &reason);

private:
    IRepository *m_repository; // Non-owning pointer (or manage via std::unique_ptr)
};
#endif // ACTIVITYSERVICEFACADE_H

#ifndef IREPOSITORY_H
#define IREPOSITORY_H
#include "activity.h"
#include <QList>
#include <QString>

class IRepository {
public:
    virtual ~IRepository() = default;

    virtual bool saveActivity(Activity *activity) = 0;
    virtual QList<Activity*> getActivitiesByCourse(const QString &courseId, QObject *parent = nullptr) = 0;
    virtual QList<Activity*> getAllActivities(QObject *parent = nullptr) = 0;
    virtual bool createPendingSubmissions(const QString &activityId, const QStringList &studentCuis) = 0;
};

#endif // IREPOSITORY_H

#ifndef SQLITEACTIVITYREPOSITORY_H
#define SQLITEACTIVITYREPOSITORY_H

#include "IRepository.h"
#include <QSqlDatabase>

class SqliteActivityRepository : public IRepository {
public:
    explicit SqliteActivityRepository(const QSqlDatabase &db);

    bool initializeSchema();
    bool saveActivity(Activity *activity) override;
    QList<Activity*> getActivitiesByCourse(const QString &courseId, QObject *parent = nullptr) override;
    bool createPendingSubmissions(const QString &activityId, const QStringList &studentCuis) override;

private:
    QSqlDatabase m_db;
};

#endif // SQLITEACTIVITYREPOSITORY_H
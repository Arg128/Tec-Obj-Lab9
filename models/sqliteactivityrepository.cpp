#include "sqliteactivityrepository.h"
#include "activityfactory.h"
#include "submission.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QUuid>

SqliteActivityRepository::SqliteActivityRepository(const QSqlDatabase &db)
    : m_db(db) {}

bool SqliteActivityRepository::initializeSchema() {
    QSqlQuery query(m_db);
    bool okActivities = query.exec(R"(
        CREATE TABLE IF NOT EXISTS activities (
            id TEXT PRIMARY KEY,
            course_id TEXT NOT NULL,
            title TEXT NOT NULL,
            markdown_content TEXT,
            resource_urls TEXT,
            due_date TEXT,
            type INTEGER NOT NULL,
            allow_late INTEGER DEFAULT 0,
            duration_minutes INTEGER DEFAULT 0,
            team_member_ids TEXT
        )
    )");

    bool okSubmissions = query.exec(R"(
        CREATE TABLE IF NOT EXISTS submissions (
            id TEXT PRIMARY KEY,
            activity_id TEXT NOT NULL,
            student_cui TEXT NOT NULL,
            submitted_file_url TEXT,
            submitted_at TEXT,
            grade REAL DEFAULT 0.0,
            status INTEGER DEFAULT 0,
            FOREIGN KEY(activity_id) REFERENCES activities(id)
        )
    )");

    return okActivities && okSubmissions;
}

bool SqliteActivityRepository::saveActivity(Activity *activity) {
    if (!activity) return false;

    if (activity->id.isEmpty()) {
        activity->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    QVariantMap data = activity->toVariantMap();
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO activities (
            id, course_id, title, markdown_content, resource_urls,
            due_date, type, allow_late, duration_minutes, max_team_size
        ) VALUES (
            :id, :course_id, :title, :markdown_content, :resource_urls,
            :due_date, :type, :allow_late, :duration_minutes, :max_team_size
        )
    )");

    query.bindValue(":id", data.value("id"));
    query.bindValue(":course_id", data.value("course_id"));
    query.bindValue(":title", data.value("title"));
    query.bindValue(":markdown_content", data.value("markdown_content"));
    query.bindValue(":resource_urls", data.value("resource_urls"));
    query.bindValue(":due_date", data.value("due_date"));
    query.bindValue(":type", data.value("type"));
    query.bindValue(":allow_late", data.value("allow_late", false).toInt());
    query.bindValue(":duration_minutes", data.value("duration_minutes", 0).toInt());
    query.bindValue(":team_member_ids", data.value("team_member_ids").toString());

    return query.exec();
}

QList<Activity*> SqliteActivityRepository::getActivitiesByCourse(const QString &courseId, QObject *parent) {
    QList<Activity*> list;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM activities WHERE course_id = :course_id");
    query.bindValue(":course_id", courseId);

    if (query.exec()) {
        while (query.next()) {
            if (Activity *act = ActivityFactory::fromSqlRecord(query, parent)) {
                list.append(act);
            }
        }
    }
    return list;
}

bool SqliteActivityRepository::createPendingSubmissions(const QString &activityId, const QStringList &studentCuis) {
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO submissions (id, activity_id, student_cui, status)
        VALUES (:id, :activity_id, :student_cui, :status)
    )");

    for (const QString &cui : studentCuis) {
        query.bindValue(":id", QUuid::createUuid().toString(QUuid::WithoutBraces));
        query.bindValue(":activity_id", activityId);
        query.bindValue(":student_cui", cui);
        query.bindValue(":status", static_cast<int>(Submission::Status::Pending));
        if (!query.exec()) return false;
    }
    return true;
}
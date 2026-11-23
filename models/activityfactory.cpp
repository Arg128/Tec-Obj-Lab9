#include "activityfactory.h"
#include "task.h"
#include "exam.h"
#include "project.h"

// Accepts a QStringList (from the UI) or a ";"-separated string (from SQLite)
static QStringList toStringList(const QVariant &value) {
    if (value.typeId() == QMetaType::QStringList) {
        return value.toStringList();
    }
    return value.toString().split(";", Qt::SkipEmptyParts);
}

Activity* ActivityFactory::createActivity(Activity::Type type, const QVariantMap &data, QObject *parent) {
    Activity *activity = nullptr;

    // 1. Instantiate the specific subclass and populate its unique fields
    switch (type) {
        case Activity::Type::Task: {
            auto *task = new Task(parent);
            task->allowLate = data.value("allow_late", false).toBool();
            activity = task;
            break;
        }
        case Activity::Type::Exam: {
            auto *exam = new Exam(parent);
            exam->durationMinutes = data.value("duration_minutes", 60).toInt();
            activity = exam;
            break;
        }
        case Activity::Type::Project: {
            auto *project = new Project(parent);
            project->teamMemberIds = toStringList(data.value("team_member_ids"));
            project->submissionFormat = data.value("submission_format").toString();
            activity = project;
            break;
        }
    }

    // 2. Populate the shared base attributes
    if (activity) {
        activity->id = data.value("id").toString();
        activity->courseId = data.value("course_id").toString();
        activity->title = data.value("title").toString();
        activity->markdownContent = data.value("markdown_content").toString();
        activity->resourceUrls = toStringList(data.value("resource_urls"));
        activity->dueDate = QDateTime::fromString(data.value("due_date").toString(), Qt::ISODate);
    }

    return activity;
}

Activity* ActivityFactory::fromSqlRecord(const QSqlQuery &query, QObject *parent) {
    QVariantMap data;
    data["id"] = query.value("id");
    data["course_id"] = query.value("course_id");
    data["title"] = query.value("title");
    data["markdown_content"] = query.value("markdown_content");
    data["resource_urls"] = query.value("resource_urls");
    data["due_date"] = query.value("due_date");
    data["allow_late"] = query.value("allow_late");
    data["duration_minutes"] = query.value("duration_minutes");
    data["team_member_ids"] = query.value("team_member_ids");
    data["submission_format"] = query.value("submission_format");

    auto type = static_cast<Activity::Type>(query.value("type").toInt());
    return createActivity(type, data, parent);
}

#include "activityfactory.h"

Activity* ActivityFactory::createActivity(Activity::Type type, const QVariantMap& data, QObject* parent) {
    Activity *activity = nullptr;
    switch (type) {
        case Activity::Type::Task: {
            auto* task = new Task(parent);
            task->estimatedMinutes = data.value("duration", 3600).toInt();
            activity = task;
            break;
        }
        case Activity::Type::Exam: {
            auto* exam = new Exam(parent);
            exam->durationMinutes = data.value("duration", 60).toInt();
            activity = exam;
            break;
        }
        case Activity::Type::Project: {
            auto* project = new Project(parent);
            project->teamMemberIds = data.value("teamMemberIds").toStringList();
            activity = project;
            break;
        }
    }

    if (activity) {
        activity->id = data.value("id").toString();
        activity->courseId = data.value("courseId").toString();
        activity->title = data.value("title").toString();
        activity->markdownContent = data.value("markdownContent").toString();
        activity->type = type;
    }

    return activity;
}

Activity* ActivityFactory::fromSqlRecord(const QSqlQuery& query, QObject* parent) {
    QVariantMap data;
    data["id"] = query.value("id");
    data["courseId"] = query.value("course_id");
    data["title"] = query.value("title");
    data["markdownContent"] = query.value("markdown_content");
    data["duration"] = query.value("duration_minutes");
    data["maxTeamSize"] = query.value("max_team_size");
    data["allowLate"] = query.value("allow_late");

    auto type = static_cast<Activity::Type>(query.value("type").toInt());
    return createActivity(type, data, parent);
}
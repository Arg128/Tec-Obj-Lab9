#include "activity.h"

Activity::Activity(QObject *parent)
    : QObject(parent)
{
}

bool Activity::isOverdue() const {
    return dueDate.isValid() && QDateTime::currentDateTime() > dueDate;
}

int Activity::resourceCount() const {
    return resourceUrls.size();
}

// Default submission rule: allowed if before dueDate (Task can override this for late submissions)
bool Activity::canSubmit(const QDateTime &atTime) const {
    if (!dueDate.isValid()) return true;
    return atTime <= dueDate;
}

// Base validation: every activity needs a course and a non-empty title
bool Activity::isValid() const {
    return !courseId.trimmed().isEmpty() && !title.trimmed().isEmpty();
}

// Base serialization: subclasses call Activity::toVariantMap() and append their own fields
QVariantMap Activity::toVariantMap() const {
    QVariantMap map;
    map["id"] = id;
    map["course_id"] = courseId;
    map["title"] = title;
    map["markdown_content"] = markdownContent;
    map["resource_urls"] = resourceUrls.join(";");
    map["due_date"] = dueDate.toString(Qt::ISODate);
    map["type"] = static_cast<int>(type());
    return map;
}
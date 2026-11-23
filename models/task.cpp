#include "task.h"

Task::Task(QObject *parent)
    : Activity(parent)
{
}

bool Task::canSubmit(const QDateTime &atTime) const {
    if (isTimed) {
        return true; // Always accepts submissions even after dueDate
    }
    return Activity::canSubmit(atTime);
}

QVariantMap Task::toVariantMap() const {
    QVariantMap map = Activity::toVariantMap();
    map["allow_late"] = isTimed;
    return map;
}
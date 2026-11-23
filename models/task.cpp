#include "task.h"

Task::Task(QObject *parent)
    : Activity(parent)
{
}

bool Task::isValid() const {
    return Activity::isValid() && estimatedMinutes > 0;
}

bool Task::canSubmit(const QDateTime &atTime) const {
    if (allowLate) {
        return true; // Always accepts submissions even after dueDate
    }
    return Activity::canSubmit(atTime);
}

QVariantMap Task::toVariantMap() const {
    QVariantMap map = Activity::toVariantMap();
    map["allow_late"] = allowLate;
    return map;
}

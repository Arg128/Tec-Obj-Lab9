#include "exam.h"

Exam::Exam(QObject *parent)
    : Activity(parent)
{
}

QVariantMap Exam::toVariantMap() const {
    QVariantMap map = Activity::toVariantMap();
    map["duration_minutes"] = durationMinutes;
    return map;
}

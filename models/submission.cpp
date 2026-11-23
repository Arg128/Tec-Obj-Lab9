#include "submission.h"

Submission::Submission(QObject *parent)
    : QObject(parent)
{
}

void Submission::markAsSubmitted(const QString &fileUrl, const QDateTime &timestamp) {
    submittedFileUrl = fileUrl;
    submittedAt = timestamp;
    status = Status::Submitted;
}

void Submission::assignGrade(double score) {
    grade = score;
    status = Status::Graded;
}

QVariantMap Submission::toVariantMap() const {
    QVariantMap map;
    map["id"] = id;
    map["activity_id"] = activityId;
    map["student_cui"] = studentCui;
    map["submitted_file_url"] = submittedFileUrl;
    map["submitted_at"] = submittedAt.toString(Qt::ISODate);
    map["grade"] = grade;
    map["status"] = static_cast<int>(status);
    return map;
}

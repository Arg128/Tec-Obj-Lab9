#ifndef SUBMISSION_H
#define SUBMISSION_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QVariantMap>

class Submission : public QObject {
    Q_OBJECT
public:
    enum class Status { Pending, Submitted, Graded };
    Q_ENUM(Status)

    explicit Submission(QObject *parent = nullptr);

    // Domain behavior methods
    void markAsSubmitted(const QString &fileUrl, const QDateTime &timestamp = QDateTime::currentDateTime());
    void assignGrade(double score);
    QVariantMap toVariantMap() const;

    // Attributes
    QString id;                // Primary key in SQLite
    QString activityId;        // Foreign key -> links to Activity::id
    QString studentCui;        // Student identifier (CUI)
    QString submittedFileUrl;  // Path or URL to uploaded student file
    QDateTime submittedAt;
    double grade = 0.0;
    Status status = Status::Pending;
};

#endif // SUBMISSION_H
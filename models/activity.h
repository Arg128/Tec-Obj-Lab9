#ifndef ACTIVITY_H
#define ACTIVITY_H

#include <QObject>
#include <QDateTime>

// 1. Base Class (Replaces duplicated attributes across Task, Exam, Project)
class Activity : public QObject {
    Q_OBJECT
public:
    enum class Type { Task, Exam, Project };
    Q_ENUM(Type) // Registers the enum with Qt's meta-object system

    QString id;
    QString courseId;          // Links to a Course object instead of a hardcoded enum
    QString title;
    QString markdownContent;   // Info in Markdown
    QStringList resourceUrls;  // Links to educational documents
    QDateTime dueDate;
    Type type;

    explicit Activity(Type type, QObject *parent = nullptr)
        : QObject(parent), type(type) {}

    virtual ~Activity() = default;


    virtual bool isValid() const = 0;

private:
    QObject *parent;
};


class Task : public Activity {
    Q_OBJECT

public:
    explicit Task(QObject *parent = nullptr)
        : Activity(Type::Task, parent) {}

    int estimatedMinutes = 0;

    bool isValid() const override {
        return !title.isEmpty() && estimatedMinutes >= 0;
    }
};

class Project : public Activity {
    Q_OBJECT

public:
    explicit Project(QObject *parent = nullptr)
        : Activity(Type::Project, parent) {}

    QStringList teamMemberIds;
    QString submissionFormat;

    bool isValid() const override {
        return !title.isEmpty() && !submissionFormat.isEmpty();
    }

};

class Exam : public Activity {
    Q_OBJECT

public:
    explicit Exam(QObject *parent = nullptr)
        : Activity(Type::Exam, parent) {}

    int durationMinutes = 0;
    bool isTimed = true;

    bool isValid() const override {
        return !title.isEmpty() && durationMinutes > 0;
    }
};

// 3. Submission Model (Connects Students [CUI] to Activities)
class Submission : public QObject {
    Q_OBJECT
public:
    enum class Status { Pending, Submitted, Graded };

    QString activityId;
    QString studentCui;        // Student identifier
    QString submittedFileUrl;  // Replaces the "auto-created upload field" in the Model
    QDateTime submittedAt;
    double grade = 0.0;
    Status status = Status::Pending;
};


#endif // ACTIVITY_H
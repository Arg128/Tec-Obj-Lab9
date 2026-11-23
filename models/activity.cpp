#include <QObject>
#include <QDateTime>

// 1. Base Class (Replaces duplicated attributes across Task, Exam, Project)
class Activity : public QObject {
    Q_OBJECT
public:
    enum class Type { Task, Exam, Project };

    QString id;
    QString courseId;          // Links to a Course object instead of a hardcoded enum
    QString title;
    QString markdownContent;   // Info in Markdown
    QStringList resourceUrls;  // Links to educational documents
    QDateTime dueDate;
    Type type;
};

// 2. Specialized Subclasses (Only add what is truly unique!)
class Task : public Activity {
    // e.g., bool allowLateSubmission;
};

class Exam : public Activity {
    // e.g., int durationMinutes; (The View sees this and renders the "Start" button)
};

class Project : public Activity {
    // e.g., int maxTeamSize;
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
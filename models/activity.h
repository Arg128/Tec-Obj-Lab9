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

    bool isOverdue() const;
    int resourceCount() const;

    explicit Activity(QObject *parent = nullptr);

    virtual ~Activity() = default;
    virtual bool isValid() const;

// --- Virtual polymorphic domain rules (overridden by Task, Exam, Project) ---
    virtual Type type() const = 0; // Pure virtual -> makes Activity an abstract base class
    virtual bool canSubmit(const QDateTime &atTime = QDateTime::currentDateTime()) const;
    virtual QVariantMap toVariantMap() const;

private:
    QObject *parent;
};

#endif // ACTIVITY_H
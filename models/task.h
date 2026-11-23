#ifndef TASK_H
#define TASK_H

#include "activity.h"

class Task : public Activity {
    Q_OBJECT
public:
    explicit Task(QObject *parent = nullptr);

    Type type() const override { return Type::Task; }
    bool isValid() const override;
    bool canSubmit(const QDateTime &atTime = QDateTime::currentDateTime()) const override;
    QVariantMap toVariantMap() const override;

    int estimatedMinutes = 60;
    bool allowLate = false; // true: accepts submissions even after dueDate
};

#endif // TASK_H

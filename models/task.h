#ifndef TASK_H
#define TASK_H

#include "activity.h"

class Task : public Activity {
    Q_OBJECT
public:
    explicit Task(QObject *parent = nullptr);

    Type type() const override { return Type::Exam; }
    bool isValid() const override;
    QVariantMap toVariantMap() const override;

    int estimatedMinutes = 60;
    bool isTimed;

};

#endif // TASK_H

#ifndef EXAM_H
#define EXAM_H

#include "activity.h"

class Exam : public Activity {
    Q_OBJECT
public:
    explicit Exam(QObject *parent = nullptr);

    Type type() const override { return Type::Exam; }
    QVariantMap toVariantMap() const override;

    int durationMinutes = 60;
    bool isTimed = true;

    bool isValid() const override {
        return Activity::isValid() && durationMinutes > 0;
    }
};

#endif // EXAM_H

#ifndef PROJECT_H
#define PROJECT_H

#include "activity.h"

class Project : public Activity {
    Q_OBJECT
public:
    explicit Project(QObject *parent = nullptr);

    Type type() const override { return Type::Project; }
    QVariantMap toVariantMap() const override;

    QStringList teamMemberIds;
    QString submissionFormat;

    bool isGroupProject() const;

    bool isValid() const override {
        return Activity::isValid() && !submissionFormat.isEmpty();
    }
};
#endif // PROJECT_H

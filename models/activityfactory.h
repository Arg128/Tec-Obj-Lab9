#pragma once
#include "activity.h"
#include <QSqlQuery>
#include <QVariantMap>

class ActivityFactory {
public:
    // 1. Used when creating a new activity from the Controller/UI
    static Activity* createActivity(Activity::Type type, const QVariantMap& data, QObject* parent = nullptr);

    // 2. Used when loading an existing activity from a SQLite row
    static Activity* fromSqlRecord(const QSqlQuery& query, QObject* parent = nullptr);
};

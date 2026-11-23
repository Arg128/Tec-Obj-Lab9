#include "project.h"

Project::Project(QObject *parent)
    : Activity(parent)
{
}

bool Project::isGroupProject() const {
    return teamMemberIds.size() > 1;
}

QVariantMap Project::toVariantMap() const {
    QVariantMap map = Activity::toVariantMap();
    map["team_member_ids"] = teamMemberIds;
    return map;
}

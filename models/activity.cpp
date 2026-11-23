#include "activity.h"

bool Activity::isValid() const {
    return dueDate.isValid() && QDateTime::currentDateTime() > dueDate;
}
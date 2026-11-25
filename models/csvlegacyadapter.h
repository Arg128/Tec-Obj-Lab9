#ifndef CSVLEGACYADAPTER_H
#define CSVLEGACYADAPTER_H

#include "IRepository.h"
#include "legacycsvsource.h"

// Adapter (patrón estructural): hace que la fuente CSV heredada (Adaptee)
// cumpla la interfaz IRepository que espera el resto del sistema (Target).
//
// Traducciones que realiza:
//   TAR / EXA / PRY      <->  Activity::Type::Task / Exam / Project
//   dd/MM/yyyy           <->  QDateTime ISO (23:59)
//   PESO                 ->   se conserva como texto en markdownContent
//                             (el modelo nuevo no tiene peso)
//   Proyecto sin formato ->   submissionFormat = "No especificado"
class CsvLegacyAdapter : public IRepository {
public:
    explicit CsvLegacyAdapter(const QString &csvPath);

    bool saveActivity(Activity *activity) override;
    QList<Activity*> getActivitiesByCourse(const QString &courseId, QObject *parent = nullptr) override;
    QList<Activity*> getAllActivities(QObject *parent = nullptr) override;

    // El formato antiguo no tiene entregas: no hay nada que guardar (no-op).
    bool createPendingSubmissions(const QString &activityId, const QStringList &studentCuis) override;
    QList<StudentAssignment> getAssignmentsByStudent(const QString &studentCui, QObject *parent = nullptr) override;
    bool saveSubmission(Submission *submission) override;

    // Registros del CSV que no se pudieron convertir en la última lectura
    int malformedLines() const { return m_malformed; }

private:
    LegacyCsvSource m_source;
    int m_malformed = 0;
};

#endif // CSVLEGACYADAPTER_H

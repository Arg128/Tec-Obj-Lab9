#ifndef ACTIVITYSERVICEFACADE_H
#define ACTIVITYSERVICEFACADE_H

#include <QObject>
#include <QVariantMap>
#include <QStringList>
#include "activity.h"
#include "IRepository.h"

class ActivityServiceFacade : public QObject {
    Q_OBJECT
public:
    explicit ActivityServiceFacade(IRepository *repository, QObject *parent = nullptr);

    // Single entry point: creates, validates, persists, assigns to students, and notifies
    bool publishActivity(Activity::Type type,
                         const QVariantMap &formData,
                         const QStringList &enrolledStudentCuis);

    QList<Activity*> loadCourseActivities(const QString &courseId);

    // Importa todas las actividades de otra fuente (cualquier IRepository)
    // al repositorio propio. Las inválidas o duplicadas se cuentan como omitidas.
    struct ImportResult {
        int imported = 0;
        int skipped = 0;
    };
    ImportResult importFrom(IRepository *source, const QStringList &enrolledStudentCuis = {});

    // Caso de uso: importar el CSV heredado usando el Adapter
    void importLegacyCsv(const QString &path, const QStringList &enrolledStudentCuis = {});

    // Entregas de un estudiante (actividad + entrega). Los objetos pertenecen al llamador.
    QList<StudentAssignment> loadStudentAssignments(const QString &studentCui);

    // Registra la entrega de un estudiante respetando las reglas del modelo
    // (estado Pendiente y Activity::canSubmit). El resultado llega por señal.
    bool submitAssignment(const QString &studentCui, const QString &submissionId, const QString &fileUrl);

signals:
    // Notification subsystem (Views/Controllers subscribe to these)
    void activityPublished(Activity *activity, int notifiedStudentsCount);
    void operationFailed(const QString &reason);
    void importFinished(int imported, int skipped);
    void assignmentSubmitted(const QString &submissionId);
    void submissionFailed(const QString &reason);

private:
    IRepository *m_repository; // Non-owning pointer (or manage via std::unique_ptr)
};
#endif // ACTIVITYSERVICEFACADE_H

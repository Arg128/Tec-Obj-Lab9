#include "csvlegacyadapter.h"
#include "activityfactory.h"
#include <QDate>
#include <QUuid>
#include <QVariantMap>

// --- Traducción de tipos -----------------------------------------------------

static bool tipoModelo(const QString &codigo, Activity::Type &out)
{
    const QString c = codigo.trimmed().toUpper();
    if (c == "TAR") { out = Activity::Type::Task;    return true; }
    if (c == "EXA") { out = Activity::Type::Exam;    return true; }
    if (c == "PRY") { out = Activity::Type::Project; return true; }
    return false;
}

static QString tipoLegado(Activity::Type tipo)
{
    switch (tipo) {
    case Activity::Type::Task:    return "TAR";
    case Activity::Type::Exam:    return "EXA";
    case Activity::Type::Project: return "PRY";
    }
    return "TAR";
}

// --- Registro heredado -> Activity (vía Factory Method) --------------------

static Activity* aActividad(const LegacyRecord &r, QObject *parent)
{
    Activity::Type tipo;
    if (!tipoModelo(r.tipo, tipo)) return nullptr;

    const QDate fecha = QDate::fromString(r.fechaEntrega, "dd/MM/yyyy");
    if (!fecha.isValid()) return nullptr;

    QVariantMap data;
    data["id"] = r.cod;
    data["course_id"] = r.curso;
    data["title"] = r.titulo;
    data["due_date"] = QDateTime(fecha, QTime(23, 59)).toString(Qt::ISODate);
    if (!r.peso.isEmpty()) {
        data["markdown_content"] = QString("Peso (dato heredado): %1").arg(r.peso);
    }
    if (tipo == Activity::Type::Project) {
        data["submission_format"] = "No especificado";
    }
    return ActivityFactory::createActivity(tipo, data, parent);
}

// --- IRepository -------------------------------------------------------------

CsvLegacyAdapter::CsvLegacyAdapter(const QString &csvPath)
    : m_source(csvPath)
{
}

QList<Activity*> CsvLegacyAdapter::getAllActivities(QObject *parent)
{
    QList<Activity*> lista;
    int descartadas = 0;
    m_malformed = 0;

    const QList<LegacyRecord> registros = m_source.readAll(&descartadas);
    for (const LegacyRecord &r : registros) {
        if (Activity *a = aActividad(r, parent)) {
            lista.append(a);
        } else {
            ++m_malformed;
        }
    }
    m_malformed += descartadas;
    return lista;
}

QList<Activity*> CsvLegacyAdapter::getActivitiesByCourse(const QString &courseId, QObject *parent)
{
    QList<Activity*> lista;
    const QList<Activity*> todas = getAllActivities(parent);
    for (Activity *a : todas) {
        if (a->courseId.compare(courseId, Qt::CaseInsensitive) == 0) {
            lista.append(a);
        } else {
            delete a;
        }
    }
    return lista;
}

bool CsvLegacyAdapter::saveActivity(Activity *activity)
{
    if (!activity) return false;
    if (activity->id.isEmpty()) {
        activity->id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    // El separador ";" no puede aparecer dentro de un campo del formato antiguo
    auto limpio = [](QString texto) { return texto.replace(';', ','); };

    LegacyRecord r;
    r.cod = activity->id;
    r.tipo = tipoLegado(activity->type());
    r.titulo = limpio(activity->title);
    r.curso = limpio(activity->courseId);
    r.fechaEntrega = activity->dueDate.toString("dd/MM/yyyy");
    r.peso = "0";
    return m_source.append(r);
}

bool CsvLegacyAdapter::createPendingSubmissions(const QString &, const QStringList &)
{
    return true;
}

// El formato antiguo no tiene entregas: no hay nada que listar ni guardar.
QList<StudentAssignment> CsvLegacyAdapter::getAssignmentsByStudent(const QString &, QObject *)
{
    return {};
}

bool CsvLegacyAdapter::saveSubmission(Submission *)
{
    return false;
}

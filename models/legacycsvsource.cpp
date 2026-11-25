#include "legacycsvsource.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>

LegacyCsvSource::LegacyCsvSource(const QString &path)
    : m_path(path)
{
}

QList<LegacyRecord> LegacyCsvSource::readAll(int *malformed) const
{
    QList<LegacyRecord> records;
    if (malformed) *malformed = 0;

    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return records;
    }

    QTextStream in(&file);
    bool primeraLinea = true;
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        // Cabecera del formato antiguo
        if (primeraLinea) {
            primeraLinea = false;
            if (line.startsWith("COD;", Qt::CaseInsensitive)) continue;
        }

        const QStringList f = line.split(';');
        if (f.size() < 6) {
            if (malformed) ++(*malformed);
            continue;
        }
        records.append({f[0].trimmed(), f[1].trimmed(), f[2].trimmed(),
                        f[3].trimmed(), f[4].trimmed(), f[5].trimmed()});
    }
    return records;
}

bool LegacyCsvSource::append(const LegacyRecord &r) const
{
    QFile file(m_path);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return false;
    }
    QTextStream out(&file);
    out << r.cod << ';' << r.tipo << ';' << r.titulo << ';'
        << r.curso << ';' << r.fechaEntrega << ';' << r.peso << '\n';
    return true;
}

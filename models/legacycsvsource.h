#ifndef LEGACYCSVSOURCE_H
#define LEGACYCSVSOURCE_H

#include <QList>
#include <QString>

// Componente heredado (Adaptee): lee y escribe el CSV con la estructura antigua.
//
//   COD;TIPO;TITULO;CURSO;FECHA_ENTREGA;PESO
//   TIPO          -> TAR | EXA | PRY
//   FECHA_ENTREGA -> dd/MM/yyyy
//   Separador     -> ";"
//
// Esta clase NO se modifica para encajar en el modelo nuevo: tiene su propia
// interfaz (LegacyRecord, readAll, append). De la traducción se encarga
// CsvLegacyAdapter.
struct LegacyRecord {
    QString cod;
    QString tipo;
    QString titulo;
    QString curso;
    QString fechaEntrega;
    QString peso;
};

class LegacyCsvSource {
public:
    explicit LegacyCsvSource(const QString &path);

    // Ignora la cabecera y las líneas con menos de 6 columnas
    // (la cantidad de líneas descartadas se devuelve en *malformed).
    QList<LegacyRecord> readAll(int *malformed = nullptr) const;
    bool append(const LegacyRecord &record) const;

private:
    QString m_path;
};

#endif // LEGACYCSVSOURCE_H

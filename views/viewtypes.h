#ifndef VIEWTYPES_H
#define VIEWTYPES_H

#include <QDateTime>
#include <QString>
#include <QStringList>

// Tipos propios de la capa de vista.
// La vista NO incluye ninguna clase del modelo: el controlador traduce
// estos tipos a Activity::Type y QVariantMap.

enum class TipoVista { Tarea = 0, Examen = 1, Proyecto = 2 }; // mismo orden que comboTipo

struct ActividadFormData {
    TipoVista tipo = TipoVista::Tarea;
    QString curso;
    QString titulo;
    QString notasMarkdown;
    QStringList enlaces;
    QStringList archivos;
    QDateTime fechaEntrega;
    bool entregaTardia = false;      // solo Tarea
    int duracionMinutos = 60;        // solo Examen
    QString formatoEntrega;          // solo Proyecto
    QStringList integrantes;         // solo Proyecto (CUIs)
};

#endif // VIEWTYPES_H

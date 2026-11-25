#ifndef VIEWTYPES_H
#define VIEWTYPES_H

#include <QDateTime>
#include <QString>
#include <QStringList>

// Tipos propios de la capa de vista.
// La vista NO incluye ninguna clase del modelo: el controlador traduce
// estos tipos a Activity::Type y QVariantMap (y viceversa).

enum class TipoVista { Tarea = 0, Examen = 1, Proyecto = 2 }; // mismo orden que comboTipo

// Datos que recoge el formulario de creación
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

// Una fila de la tabla de actividades de un curso (solo datos para mostrar)
struct ActividadFila {
    QString tipo;                    // "Tarea", "Examen" o "Proyecto"
    QString titulo;
    QDateTime fechaEntrega;
    bool vencida = false;
    int recursos = 0;                // cantidad de enlaces y archivos
};

// Estado de una entrega, tal como lo muestra el dashboard del estudiante
enum class EstadoEntrega { Pendiente, Entregada, Calificada };

// Una fila de la tabla del dashboard del estudiante (solo datos para mostrar)
struct EntregaFila {
    QString submissionId;            // identifica la entrega al pedir "entregar"
    QString curso;
    QString tipo;
    QString titulo;
    QDateTime fechaLimite;
    bool vencida = false;
    EstadoEntrega estado = EstadoEntrega::Pendiente;
    double nota = 0.0;
};

#endif // VIEWTYPES_H

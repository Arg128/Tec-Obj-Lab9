#ifndef DASHBOARDESTUDIANTEVIEW_H
#define DASHBOARDESTUDIANTEVIEW_H

#include <QWidget>
#include <QList>
#include "viewtypes.h"

namespace Ui { class DashboardEstudianteView; }

// Dashboard del estudiante: lista sus entregas y permite entregar una.
// Solo muestra filas ya preparadas y emite señales; las reglas (plazo,
// doble entrega) las decide el modelo.
class DashboardEstudianteView : public QWidget {
    Q_OBJECT
public:
    explicit DashboardEstudianteView(QWidget *parent = nullptr);
    ~DashboardEstudianteView();

    void setEstudiante(const QString &cui);
    void mostrarEntregas(const QList<EntregaFila> &filas);
    void mostrarEstado(const QString &mensaje);
    void mostrarError(const QString &mensaje);

signals:
    void entregarSolicitado(const QString &submissionId, const QString &rutaArchivo);
    void cerrarSesionSolicitado();

private:
    void repintarTabla(); // aplica el filtro elegido sobre las filas recibidas

    Ui::DashboardEstudianteView *ui;
    QList<EntregaFila> m_filas;
};

#endif // DASHBOARDESTUDIANTEVIEW_H

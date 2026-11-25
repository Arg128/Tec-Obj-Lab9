#ifndef DASHBOARDVIEW_H
#define DASHBOARDVIEW_H

#include <QWidget>
#include <QStringList>
#include "viewtypes.h"

namespace Ui { class DashboardView; }

// Vista del dashboard: botones de creación y grilla de cursos.
class DashboardView : public QWidget {
    Q_OBJECT
public:
    explicit DashboardView(QWidget *parent = nullptr);
    ~DashboardView();

    // Reemplaza los cursos mostrados (y los marcadores del diseño) por botones reales.
    void mostrarCursos(const QStringList &cursos);
    void mostrarMensaje(const QString &mensaje);


signals:
    void crearActividadSolicitada(TipoVista tipo);
    void cursoSeleccionado(const QString &curso);
    void importarCsvSolicitado(const QString &ruta);
    void cerrarSesionSolicitado();

private:
    Ui::DashboardView *ui;
};

#endif // DASHBOARDVIEW_H

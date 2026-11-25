#ifndef ACTIVIDADESCURSOVIEW_H
#define ACTIVIDADESCURSOVIEW_H

#include <QWidget>
#include <QList>
#include "viewtypes.h"

namespace Ui { class ActividadesCursoView; }

// Lista las actividades de un curso. Solo muestra filas ya preparadas.
class ActividadesCursoView : public QWidget {
    Q_OBJECT
public:
    explicit ActividadesCursoView(QWidget *parent = nullptr);
    ~ActividadesCursoView();

    void mostrarActividades(const QString &curso, const QList<ActividadFila> &filas);

signals:
    void volverSolicitado();

private slots:
    void mostrarDetalle();

private:
    Ui::ActividadesCursoView *ui;
    QList<ActividadFila> m_filas;
};

#endif // ACTIVIDADESCURSOVIEW_H

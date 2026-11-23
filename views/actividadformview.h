#ifndef ACTIVIDADFORMVIEW_H
#define ACTIVIDADFORMVIEW_H

#include <QWidget>
#include <QStringList>
#include "viewtypes.h"

namespace Ui { class ActividadFormView; }

// Formulario para crear tarea, examen o proyecto.
// Solo recoge datos y los emite; las reglas de validación viven en el modelo.
class ActividadFormView : public QWidget {
    Q_OBJECT
public:
    explicit ActividadFormView(QWidget *parent = nullptr);
    ~ActividadFormView();

    void setCursos(const QStringList &cursos);
    void setTipo(TipoVista tipo);
    void mostrarError(const QString &mensaje);
    void limpiar();

signals:
    void guardarSolicitado(const ActividadFormData &datos);
    void cancelarSolicitado();

private slots:
    void actualizarCamposPorTipo();
    void adjuntarArchivos();
    void emitirGuardar();

private:
    Ui::ActividadFormView *ui;
};

#endif // ACTIVIDADFORMVIEW_H

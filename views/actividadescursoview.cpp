#include "actividadescursoview.h"
#include "ui_actividades_curso.h"
#include <QHeaderView>
#include <QTableWidgetItem>

ActividadesCursoView::ActividadesCursoView(QWidget *parent)
    : QWidget(parent), ui(new Ui::ActividadesCursoView)
{
    ui->setupUi(this);
    setWindowTitle("Actividades del curso");

    // Columnas de la tabla
    ui->tablaActividades->setColumnCount(5);
    ui->tablaActividades->setHorizontalHeaderLabels(
        {"Tipo", "Título", "Fecha de entrega", "Recursos", "Estado"});
    ui->tablaActividades->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tablaActividades->verticalHeader()->setVisible(false);
    ui->tablaActividades->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablaActividades->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->btnVolver, &QPushButton::clicked,
            this, &ActividadesCursoView::volverSolicitado);
}

ActividadesCursoView::~ActividadesCursoView()
{
    delete ui;
}

void ActividadesCursoView::mostrarActividades(const QString &curso,
                                              const QList<ActividadFila> &filas)
{
    ui->lblCurso->setText(QString("Actividades de %1").arg(curso));

    ui->tablaActividades->setRowCount(0);
    ui->tablaActividades->setRowCount(filas.size());
    for (int i = 0; i < filas.size(); ++i) {
        const ActividadFila &fila = filas.at(i);
        ui->tablaActividades->setItem(i, 0, new QTableWidgetItem(fila.tipo));
        ui->tablaActividades->setItem(i, 1, new QTableWidgetItem(fila.titulo));
        ui->tablaActividades->setItem(i, 2,
            new QTableWidgetItem(fila.fechaEntrega.toString("dd/MM/yyyy HH:mm")));
        ui->tablaActividades->setItem(i, 3,
            new QTableWidgetItem(QString::number(fila.recursos)));
        ui->tablaActividades->setItem(i, 4,
            new QTableWidgetItem(fila.vencida ? "Vencida" : "Vigente"));
    }
}

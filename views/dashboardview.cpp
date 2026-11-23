#include "dashboardview.h"
#include "ui_dashboard.h"
#include <QPushButton>
#include <QMessageBox>

DashboardView::DashboardView(QWidget *parent)
    : QWidget(parent), ui(new Ui::DashboardView)
{
    ui->setupUi(this);
    setWindowTitle("Mis cursos");

    connect(ui->btnCrearTarea, &QPushButton::clicked, this, [this]() {
        emit crearActividadSolicitada(TipoVista::Tarea);
    });
    connect(ui->btnCrearExamen, &QPushButton::clicked, this, [this]() {
        emit crearActividadSolicitada(TipoVista::Examen);
    });
    connect(ui->btnCrearProyecto, &QPushButton::clicked, this, [this]() {
        emit crearActividadSolicitada(TipoVista::Proyecto);
    });
}

DashboardView::~DashboardView()
{
    delete ui;
}

void DashboardView::mostrarCursos(const QStringList &cursos)
{
    // Quita lo que haya en la grilla (marcadores del diseño o cursos anteriores)
    while (QLayoutItem *item = ui->gridCursos->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }

    const int columnas = 2;
    for (int i = 0; i < cursos.size(); ++i) {
        auto *boton = new QPushButton(cursos.at(i), ui->scrollAreaWidgetContents);
        boton->setMinimumHeight(80);
        connect(boton, &QPushButton::clicked, this, [this, curso = cursos.at(i)]() {
            emit cursoSeleccionado(curso);
        });
        ui->gridCursos->addWidget(boton, i / columnas, i % columnas);
    }
}

void DashboardView::mostrarMensaje(const QString &mensaje)
{
    QMessageBox::information(this, "Seguimiento académico", mensaje);
}

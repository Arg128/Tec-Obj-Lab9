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

    ui->tablaActividades->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 2);

    connect(ui->tablaActividades, &QTableWidget::itemSelectionChanged,
            this, &ActividadesCursoView::mostrarDetalle);
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
    m_filas = filas;
    ui->textDetalle->clear();

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

    if (filas.isEmpty()) {
        ui->textDetalle->setMarkdown("*Este curso todavía no tiene actividades.*");
    } else {
        ui->tablaActividades->selectRow(0); // dispara mostrarDetalle()
    }
}

// Muestra en el panel inferior el detalle y el Markdown de la fila elegida
void ActividadesCursoView::mostrarDetalle()
{
    const int fila = ui->tablaActividades->currentRow();
    if (fila < 0 || fila >= m_filas.size()) {
        ui->textDetalle->clear();
        return;
    }
    const ActividadFila &a = m_filas.at(fila);

    QString md;
    md += QString("# %1\n\n").arg(a.titulo);
    md += QString("**%1** · Entrega: %2 · %3\n\n")
              .arg(a.tipo,
                   a.fechaEntrega.toString("dd/MM/yyyy HH:mm"),
                   a.vencida ? "Vencida" : "Vigente");
    if (!a.detalle.isEmpty()) {
        md += a.detalle + "\n\n";
    }
    md += "---\n\n";
    md += a.markdown.trimmed().isEmpty() ? QString("*El docente no agregó notas.*")
                                         : a.markdown;
    md += "\n\n";
    if (!a.listaRecursos.isEmpty()) {
        md += "### Recursos\n\n";
        for (const QString &r : a.listaRecursos) {
            md += QString("- <%1>\n").arg(r);
        }
    }
    ui->textDetalle->setMarkdown(md);
}

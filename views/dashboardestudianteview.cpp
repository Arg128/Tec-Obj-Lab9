#include "dashboardestudianteview.h"
#include "ui_dashboard_estudiante.h"
#include <QBrush>
#include <QFileDialog>
#include <QHeaderView>
#include <QTableWidgetItem>

DashboardEstudianteView::DashboardEstudianteView(QWidget *parent)
    : QWidget(parent), ui(new Ui::DashboardEstudianteView)
{
    ui->setupUi(this);
    setWindowTitle("Mis entregas");
    ui->lblEstado->clear();

    // Filtro: el índice coincide con el orden de estos textos
    ui->comboFiltro->addItems({"Todas", "Pendientes", "Entregadas", "Calificadas"});

    // Columnas de la tabla
    ui->tablaEntregas->setColumnCount(6);
    ui->tablaEntregas->setHorizontalHeaderLabels(
        {"Curso", "Tipo", "Título", "Fecha límite", "Estado", "Nota"});
    // Todas las columnas se ajustan a su contenido, salvo el título que ocupa el espacio sobrante
    for (int col = 0; col < 6; ++col) {
        ui->tablaEntregas->horizontalHeader()->setSectionResizeMode(
            col, col == 2 ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
    }
    ui->tablaEntregas->verticalHeader()->setVisible(false);
    ui->tablaEntregas->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablaEntregas->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tablaEntregas->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->comboFiltro, &QComboBox::currentIndexChanged,
            this, &DashboardEstudianteView::repintarTabla);

    connect(ui->btnEntregar, &QPushButton::clicked, this, [this]() {
        const int fila = ui->tablaEntregas->currentRow();
        if (fila < 0) {
            mostrarError("Selecciona una actividad de la tabla.");
            return;
        }
        const QString id = ui->tablaEntregas->item(fila, 0)->data(Qt::UserRole).toString();

        // La vista solo elige el archivo; el modelo decide si la entrega es válida
        const QString ruta = QFileDialog::getOpenFileName(this, "Archivo a entregar");
        if (ruta.isEmpty()) return;

        ui->lblEstado->clear();
        emit entregarSolicitado(id, ruta);
    });

    connect(ui->btnCerrarSesion, &QPushButton::clicked,
            this, &DashboardEstudianteView::cerrarSesionSolicitado);
}

DashboardEstudianteView::~DashboardEstudianteView()
{
    delete ui;
}

void DashboardEstudianteView::setEstudiante(const QString &cui)
{
    ui->lblTitulo->setText(QString("Mis entregas — estudiante %1").arg(cui));
    ui->lblEstado->clear();
    ui->comboFiltro->setCurrentIndex(0);
}

void DashboardEstudianteView::mostrarEntregas(const QList<EntregaFila> &filas)
{
    m_filas = filas;
    repintarTabla();
}

void DashboardEstudianteView::mostrarEstado(const QString &mensaje)
{
    ui->lblEstado->setStyleSheet("color: #1b5e20;");
    ui->lblEstado->setText(mensaje);
}

void DashboardEstudianteView::mostrarError(const QString &mensaje)
{
    ui->lblEstado->setStyleSheet("color: #b00020;");
    ui->lblEstado->setText(mensaje);
}

void DashboardEstudianteView::repintarTabla()
{
    // 0 = Todas, 1 = Pendientes, 2 = Entregadas, 3 = Calificadas
    const int filtro = ui->comboFiltro->currentIndex();
    auto visible = [filtro](const EntregaFila &f) {
        switch (filtro) {
        case 1:  return f.estado == EstadoEntrega::Pendiente;
        case 2:  return f.estado == EstadoEntrega::Entregada;
        case 3:  return f.estado == EstadoEntrega::Calificada;
        default: return true;
        }
    };

    ui->tablaEntregas->setRowCount(0);
    int fila = 0;
    for (const EntregaFila &f : std::as_const(m_filas)) {
        if (!visible(f)) continue;

        QString estado;
        switch (f.estado) {
        case EstadoEntrega::Pendiente:  estado = f.vencida ? "Pendiente (vencida)" : "Pendiente"; break;
        case EstadoEntrega::Entregada:  estado = "Entregada"; break;
        case EstadoEntrega::Calificada: estado = "Calificada"; break;
        }
        const QString nota = (f.estado == EstadoEntrega::Calificada)
                                 ? QString::number(f.nota, 'f', 1) : QStringLiteral("-");

        ui->tablaEntregas->insertRow(fila);
        const QStringList celdas = {f.curso, f.tipo, f.titulo,
                                    f.fechaLimite.toString("dd/MM/yyyy HH:mm"), estado, nota};
        for (int col = 0; col < celdas.size(); ++col) {
            auto *item = new QTableWidgetItem(celdas.at(col));
            if (col == 0) {
                item->setData(Qt::UserRole, f.submissionId); // para saber qué entrega es
            }
            if (f.estado == EstadoEntrega::Pendiente && f.vencida) {
                item->setForeground(QBrush(QColor("#b00020")));
            }
            ui->tablaEntregas->setItem(fila, col, item);
        }
        ++fila;
    }
}

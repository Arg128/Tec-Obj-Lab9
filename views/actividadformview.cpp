#include "actividadformview.h"
#include "ui_actividad_form.h"
#include <QFileDialog>

// Divide un texto por un separador, recorta espacios y descarta vacíos.
static QStringList dividirLimpio(const QString &texto, const QString &separador)
{
    QStringList resultado;
    for (const QString &parte : texto.split(separador, Qt::SkipEmptyParts)) {
        const QString limpio = parte.trimmed();
        if (!limpio.isEmpty()) {
            resultado << limpio;
        }
    }
    return resultado;
}

ActividadFormView::ActividadFormView(QWidget *parent)
    : QWidget(parent), ui(new Ui::ActividadFormView)
{
    ui->setupUi(this);
    setWindowTitle("Nueva actividad");

    ui->lblError->setStyleSheet("color: #b00020;");
    ui->lblError->clear();
    ui->dateEditEntrega->setCalendarPopup(true);
    ui->dateEditEntrega->setDate(QDate::currentDate().addDays(7));

    connect(ui->comboTipo, &QComboBox::currentIndexChanged,
            this, &ActividadFormView::actualizarCamposPorTipo);
    connect(ui->btnAdjuntar, &QPushButton::clicked,
            this, &ActividadFormView::adjuntarArchivos);
    connect(ui->btnGuardar, &QPushButton::clicked,
            this, &ActividadFormView::emitirGuardar);
    connect(ui->btnCancelar, &QPushButton::clicked,
            this, &ActividadFormView::cancelarSolicitado);

    actualizarCamposPorTipo();
}

ActividadFormView::~ActividadFormView()
{
    delete ui;
}

void ActividadFormView::setCursos(const QStringList &cursos)
{
    ui->comboCurso->clear();
    ui->comboCurso->addItems(cursos);
}

void ActividadFormView::setTipo(TipoVista tipo)
{
    ui->comboTipo->setCurrentIndex(static_cast<int>(tipo));
}

void ActividadFormView::mostrarError(const QString &mensaje)
{
    ui->lblError->setText(mensaje);
}

void ActividadFormView::limpiar()
{
    ui->lineEditTitulo->clear();
    ui->plainTextNotas->clear();
    ui->plainTextEnlaces->clear();
    ui->listArchivos->clear();
    ui->lineEditFormato->clear();
    ui->lineEditIntegrantes->clear();
    ui->checkEntregaTardia->setChecked(false);
    ui->spinDuracion->setValue(60);
    ui->dateEditEntrega->setDate(QDate::currentDate().addDays(7));
    ui->lblError->clear();
}

// Muestra solo los campos que corresponden al tipo elegido
void ActividadFormView::actualizarCamposPorTipo()
{
    const auto tipo = static_cast<TipoVista>(ui->comboTipo->currentIndex());
    ui->checkEntregaTardia->setVisible(tipo == TipoVista::Tarea);
    ui->spinDuracion->setVisible(tipo == TipoVista::Examen);
    ui->lineEditFormato->setVisible(tipo == TipoVista::Proyecto);
    ui->lineEditIntegrantes->setVisible(tipo == TipoVista::Proyecto);
}

void ActividadFormView::adjuntarArchivos()
{
    const QStringList rutas = QFileDialog::getOpenFileNames(this, "Adjuntar archivos");
    for (const QString &ruta : rutas) {
        if (ui->listArchivos->findItems(ruta, Qt::MatchExactly).isEmpty()) {
            ui->listArchivos->addItem(ruta);
        }
    }
}

void ActividadFormView::emitirGuardar()
{
    ActividadFormData datos;
    datos.tipo = static_cast<TipoVista>(ui->comboTipo->currentIndex());
    datos.curso = ui->comboCurso->currentText().trimmed();
    datos.titulo = ui->lineEditTitulo->text().trimmed();
    datos.notasMarkdown = ui->plainTextNotas->toPlainText();
    datos.enlaces = dividirLimpio(ui->plainTextEnlaces->toPlainText(), "\n");
    for (int i = 0; i < ui->listArchivos->count(); ++i) {
        datos.archivos << ui->listArchivos->item(i)->text();
    }
    datos.fechaEntrega = QDateTime(ui->dateEditEntrega->date(), QTime(23, 59));
    datos.entregaTardia = ui->checkEntregaTardia->isChecked();
    datos.duracionMinutos = ui->spinDuracion->value();
    datos.formatoEntrega = ui->lineEditFormato->text().trimmed();
    datos.integrantes = dividirLimpio(ui->lineEditIntegrantes->text(), ";");

    ui->lblError->clear();
    emit guardarSolicitado(datos);
}

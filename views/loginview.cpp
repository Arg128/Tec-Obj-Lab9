#include "loginview.h"
#include "ui_login.h"

LoginView::LoginView(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginView)
{
    ui->setupUi(this);
    setWindowTitle("Iniciar sesión");

    ui->lblError->setStyleSheet("color: #b00020;");
    ui->lblError->clear();

    // El botón (o Enter en la contraseña) solo emite la señal; no valida nada.
    connect(ui->btnIngresar, &QPushButton::clicked, this, [this]() {
        emit loginSolicitado(ui->lineEditUsuario->text().trimmed(),
                             ui->lineEditPassword->text());
    });
    connect(ui->lineEditPassword, &QLineEdit::returnPressed,
            ui->btnIngresar, &QPushButton::click);
}

LoginView::~LoginView()
{
    delete ui;
}

void LoginView::mostrarError(const QString &mensaje)
{
    ui->lblError->setText(mensaje);
}

void LoginView::limpiar()
{
    ui->lineEditUsuario->clear();
    ui->lineEditPassword->clear();
    ui->lblError->clear();
}

#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>

namespace Ui { class LoginView; }

// Vista de inicio de sesión: solo muestra datos y emite señales.
class LoginView : public QWidget {
    Q_OBJECT
public:
    explicit LoginView(QWidget *parent = nullptr);
    ~LoginView();

    void mostrarError(const QString &mensaje);
    void limpiar();

signals:
    void loginSolicitado(const QString &usuario, const QString &password);

private:
    Ui::LoginView *ui;
};

#endif // LOGINVIEW_H

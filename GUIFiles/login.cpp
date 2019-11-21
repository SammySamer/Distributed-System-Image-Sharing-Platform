#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QDebug>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login),
    peer(new Peer)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_loginButton_clicked()
{
    // CHECK CREDENTIALS
    int validCred = 100;

    QString username, password;

    username = ui->user->text();
    password = ui->pass->text();

    std::string username_string = username.toUtf8().constData();
    std::string password_string = password.toUtf8().constData();

    validCred = peer->login(username_string,password_string);

    qDebug("%i",validCred);

    if (validCred == 1)
    {
        // Profile Window goes here
        peer->username = username_string;
        peer->password = password_string;

        this->hide();
        Profile profile (this,peer);
        profile.setModal(true);
        profile.exec();
    }
    else
    {
        QMessageBox::critical(this, "Login Failed", "Wrong Credentials!");
    }
}

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


void Login::on_push_back_clicked() { Login::close(); }

void Login::on_loginButton_clicked()
{
    // CHECK LOGIN CREDENTIALS
    int validCred = 100;

    QString username, password;

    username = ui->user->text();
    password = ui->pass->text();

    std::string username_string = username.toUtf8().constData();
    std::string password_string = password.toUtf8().constData();

    validCred = peer->login(username_string,password_string);

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

    else if (validCred == 0)
        QMessageBox::critical(this, "Login Failed",
                              "User not registered!");
    else if (validCred == 2)
        QMessageBox::critical(this, "Login Failed",
                              "Timeout Error with DoS!");
    else if (validCred == 3)
        QMessageBox::critical(this, "Login Failed",
                              "Special Characters not allowed!");
    else if (validCred == 4)
        QMessageBox::critical(this, "Login Failed",
                              "User already logged in!");
    else if (validCred == 5)
        QMessageBox::critical(this, "Login Failed",
                              "Incorrect password entered!");

    //6 or 8
    else
        QMessageBox::critical(this, "Login Failed",
                              "Connection error: DoS or User OFFLINE!");

}


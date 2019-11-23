#include "signup.h"
#include "ui_signup.h"
#include "peer.h"
#include <QMessageBox>
#include <QDebug>
#include <string>

Signup::Signup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Signup),
    peer(new Peer)
{
    ui->setupUi(this);
}

Signup::~Signup()
{
    delete ui;
}

void Signup::on_registerButton_clicked()
{
    // CHECK IF EXISTING USER
    int newUser = 100;

    QString username, password;

    username = ui->user->text();
    password = ui->pass->text();

    std::string username_string = username.toUtf8().constData();
    std::string password_string = password.toUtf8().constData();

    newUser = peer->sign_up(username_string,password_string);

    if (newUser == 1)
        QMessageBox::information(this, "Registration Succeeded",
                               "You are now registered!");

    else if (newUser == 0)
        QMessageBox::critical(this, "Registration Failed",
                              "Specials character not allowed!");
    else if (newUser == 2)
        QMessageBox::critical(this, "Registration Failed",
                              "Timeout Error with DoS!");
    else if (newUser == 3)
        QMessageBox::critical(this, "Registration Failed",
                              "Special Characters not allowed!");
    else if (newUser == 5)
        QMessageBox::critical(this, "Registration Failed",
                              "User already registered!");

    //6 or 8
    else
        QMessageBox::critical(this, "Registration Failed",
                              "Connection error: DoS or User OFFLINE!");


}

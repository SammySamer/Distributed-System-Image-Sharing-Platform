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
    //const char* NU=newUser.c_str();
    qDebug("%i",newUser);

    if (newUser == 1)
    {
         QMessageBox::critical(this, "Registration Succeeded", "You are now registered!");
    }
    else//ADJUST LATER
    {
         QMessageBox::critical(this, "Registration Failed", "User with same username already exists!");
    }

}

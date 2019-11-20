#include "login.h"
#include "ui_login.h"
#include <QMessageBox>
#include <QDebug>

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
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
    bool validCred = true;
    if (validCred)
    {
        // Profile Window goes here
        this->hide();
        Profile profile;
        profile.setModal(true);
        profile.exec();
    }
    else
    {
        QMessageBox::critical(this, "Login Failed", "Wrong Credentials!");
    }
}

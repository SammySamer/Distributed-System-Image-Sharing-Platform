#include "signup.h"
#include "ui_signup.h"
#include <QMessageBox>
#include <QDebug>

Signup::Signup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Signup)
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
    bool newUser = true;
    if (newUser)
    {
         QMessageBox::critical(this, "Registration Succeeded", "You are now registered!");
    }
    else
    {
         QMessageBox::critical(this, "Registration Failed", "User with same username already exists!");
    }

}

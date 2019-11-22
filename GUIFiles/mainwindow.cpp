#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginButton_clicked()
{
    // Login Window goes here
    this->hide();
    Login login;
    login.setModal(true);
    login.exec();
}

void MainWindow::on_signupButton_clicked()
{
    // Signup Window goes here
    this->hide();
    Signup signup;
    signup.setModal(true);
    signup.exec();
}

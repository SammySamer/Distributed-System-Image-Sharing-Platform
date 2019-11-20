#include "profile.h"
#include "ui_profile.h"

Profile::Profile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Profile)
{
    ui->setupUi(this);

    // PUT USERNAME
    ui->label->setText("username");
}

Profile::~Profile()
{
    delete ui;
}

void Profile::on_allusersButton_clicked()
{
    // All Users Window goes here
    this->hide();
    AllUsers users;
    users.showUsers();
    users.setModal(true);
    users.exec();
}

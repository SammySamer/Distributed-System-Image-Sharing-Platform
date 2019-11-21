#include "profile.h"
#include "ui_profile.h"
#include <string>

Profile::Profile(QWidget *parent, Peer *peer) :
    QDialog(parent),
    ui(new Ui::Profile)
{
    ui->setupUi(this);

    // PUT USERNAME
    QString uname = QString::fromStdString(peer->username);
    ui->label->setText(uname);
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

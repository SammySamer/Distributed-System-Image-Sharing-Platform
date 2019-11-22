#include "profile.h"
#include "ui_profile.h"
#include <string>
#include "notifications.h"

Profile::Profile(QWidget *parent, Peer *peer) :
    QDialog(parent),
    ui(new Ui::Profile),
    peer(peer)
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
    AllUsers users (this,peer);
    users.setModal(true);
    users.exec();
}

void Profile::on_notifications_clicked()
{
    //VEIWING NOTIFOCATIONS
    this->hide();
    Notifications notifications (this,peer);
    notifications.setModal(true);
    notifications.exec();

}

#include "profile.h"
#include <thread>
#include <unistd.h>
#include <string>

Profile::Profile(QWidget *parent, Peer *peer) :
    QDialog(parent),
    ui(new Ui::Profile)
{
    ui->setupUi(this);

    //peer->readfile();
    //peer->read_my_images_file();
    cout << "This is user " << peer->username << endl;
    std::thread listenThread(&Peer::listenPeer, peer);
    listenThread.detach();

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
    AllUsers users(this, peer);
    users.showUsers();
    users.setModal(true);
    users.exec();

}

#include "profile.h"
#include "ui_profile.h"

#include <string>
#include <qfiledialog.h>
#include <thread>
#include <unistd.h>
#include <QMessageBox>

Profile::Profile(QWidget *parent, Peer *peer) :
    QDialog(parent),
    ui(new Ui::Profile),
    peer(peer)
{
    ui->setupUi(this);
    ui->lbl_upload_successful->setVisible(false);
    ui->lbl_time->setText(
        QString::fromStdString("Login Time: " + peer->getCurrentTime()));

    // PUT USERNAME
    QString uname = QString::fromStdString(peer->username);
    ui->label->setText(uname);

    //closes everything once logged out
    connect(this, SIGNAL(destroyed()), this->parent(),
            SLOT(close()));

    peer->readfile();
    peer->read_my_images_file();

    ui->lbl_upload_successful->setVisible(false);

    cout << "User's name: " << peer->username << endl;

    std::thread listenThread(&Peer::listenPeer, peer);
    cout << "User thread is listening..." << endl;
    listenThread.detach();
}

Profile::~Profile()
{
    delete ui;
}

void Profile::on_allusersButton_clicked()
{
    // All Users Window goes here
    AllUsers users (this,peer);
    users.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint |
                         Qt::CustomizeWindowHint);
    users.setModal(true);
    users.exec();
}
/*
void Profile::on_sharedImButton_clicked()
{
    // Shared Images Window goes here
    SharedImages sharedIm;
    sharedIm.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint |
                         Qt::CustomizeWindowHint);
    sharedIm.setModal(true);
    sharedIm.exec();
}
*/
void Profile::on_notifications_clicked()
{
    //VIEWING NOTIFICATIONS
    Notifications notifications (this,peer);
    notifications.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint |
                         Qt::CustomizeWindowHint);
    notifications.setModal(true);
    notifications.exec();

}

void Profile::on_push_myimages_clicked()
{
    //VEIWING MY IMAGES
    MyImages myimages (this,peer);
    myimages.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint |
                         Qt::CustomizeWindowHint);
    myimages.setModal(true);
    myimages.exec();
}

void Profile::on_upload_images_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(
        this, tr("Open Image"), "", tr("JPEG (*.jpg *.jpeg);;"));

    int upload_stat = peer->upload(imagePath.toStdString());

    if (upload_stat == 1) { // Successful
        peer->update_my_images_file();
        ui->lbl_upload_successful->setVisible(true);
        ui->lbl_upload_successful->setText(QString("Uploaded Successfully!"));
        ui->lbl_upload_successful->setStyleSheet("QLabel { color : green; }");
    }

    else if (upload_stat == 0) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Username not registered!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    }
    else if (upload_stat == 6) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Choose a file!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    }
    else if (upload_stat == 3) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("No special Chars allowed "
                                                 "in image name! Only 1 dot."));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    }
    else if (upload_stat == 9) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Image already on ImageSharer!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    }
    else if (upload_stat == 2) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Timeout error with DoS!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    }

    //30 included
    else {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Connection Error:"
                                                 "DoS or User OFFLINE!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    }
}

void Profile::on_push_logout_clicked() {
  int logged = peer->logout();
  if (logged == 1) {
      QMessageBox::critical(this, "Logout Successful!",
                            "Ciao!");
    usleep(2000);
    peer->updatefile();
    peer->update_my_images_file();
    this->close();
  }

  else if (logged == 2)
      QMessageBox::critical(this, "Logout Failed",
                            "Timeout Error with DoS!");
  else
      QMessageBox::critical(this, "Logout Failed",
                            "Connection Error: DoS or User OFFLINE!");

}

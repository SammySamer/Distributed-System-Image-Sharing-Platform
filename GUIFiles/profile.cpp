#include "profile.h"
#include "ui_profile.h"
#include <string>
#include "notifications.h"
#include "myimages.h"
#include <qfiledialog.h>
#include <thread>
#include <unistd.h>

Profile::Profile(QWidget *parent, Peer *peer) :
    QDialog(parent),
    ui(new Ui::Profile),
    peer(peer)
{
    ui->setupUi(this);
    ui->lbl_upload_successful->setVisible(false);

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

void Profile::on_push_myimages_clicked()
{
    //VEIWING MY IMAGES
    this->hide();
    MyImages myimages (this,peer);
    myimages.setModal(true);
    myimages.exec();
}

void Profile::on_upload_images_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(
        this, tr("Open Image"), "", tr("JPEG (*.jpg *.jpeg);;PNG (*.png)"));

    int upload_stat = peer->upload(imagePath.toStdString());

    if (upload_stat == 1) { // Successful
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Uploaded Successfully!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : green; }");
    } else if (upload_stat == 0) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Username not found!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    } else if (upload_stat == 6) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Please, choose a file!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    } else if (upload_stat == 3) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(
          QString("No special Chars allowed in image name! Only 1 dot."));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    } else if (upload_stat == 9) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Image uploaded before!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    } else if (upload_stat == 2) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("DoS Offline!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    } else if (upload_stat == 30) {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Check your internet connection!")); // Send to failed!
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    } else {
      ui->lbl_upload_successful->setVisible(true);
      ui->lbl_upload_successful->setText(QString("Check your internet connection!"));
      ui->lbl_upload_successful->setStyleSheet("QLabel { color : red; }");
    }
}

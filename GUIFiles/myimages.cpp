#include "myimages.h"
#include "ui_myimages.h"
#include "profile.h"
#include "editmyimages.h"

MyImages::MyImages(QWidget *parent, Peer *peer) :
    QDialog(parent),
    ui(new Ui::MyImages),
    peer(peer)
{
    ui->setupUi(this);
    ui->lbl_result->setVisible(false);
    ui->lbl_user->setText(QString::fromStdString(peer->username));
    ui->lbl_time->setText(QString::fromStdString
                          ("Time Refreshed: " + peer->getCurrentTime()));

    map<string, vector<pair<string, int>>> my_images_copy = peer->myimages;

    map<string, vector<pair<string, int>>>::iterator it;

    for (it = my_images_copy.begin(); it != my_images_copy.end(); it++) {
        ui->listWidget->addItem(QString::fromStdString(it->first));
    }
}


void MyImages::on_push_view_clicked()
{
    if (ui->listWidget->currentRow() >= 0) {
        EditMyImages secd(this, peer, ui->listWidget->currentItem()->text());
        secd.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint |
                            Qt::CustomizeWindowHint);
        secd.setModal(true);
        secd.exec();

        ui->lbl_result->setVisible(false);
    }

    else {
        ui->lbl_result->setVisible(true);
        ui->lbl_result->setStyleSheet("QLabel { color : red; }");
        ui->lbl_result->setText("Please, select an image!");
    }
}


MyImages::~MyImages(){ delete ui; }
void MyImages::on_push_back_clicked(){ MyImages::close(); }



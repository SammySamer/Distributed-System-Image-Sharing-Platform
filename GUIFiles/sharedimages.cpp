#include "sharedimages.h"
#include "ui_sharedimages.h"

SharedImages::SharedImages(QWidget *parent, Peer *peer) :
    QDialog(parent),
    ui(new Ui::SharedImages),
    peer(peer)
{
    ui->setupUi(this);
    ui->selectLabel->setVisible(false);
    ui->userLabel->setText(QString::fromStdString(peer->username));
    ui->timeLabel->setText(QString::fromStdString
                          ("Time Refreshed: " + peer->getCurrentTime()));
    map<string, int> sharedimgs_copy = peer->sharedimgs;

    map<string, int>::iterator it;

    for (it = sharedimgs_copy.begin(); it != sharedimgs_copy.end(); it++)
    {
        ui->listWidget->addItem(QString::fromStdString(it->first));
    }
}

SharedImages::~SharedImages()
{
    delete ui;
}

void SharedImages::on_showButton_clicked()
{
    if (ui->listWidget->currentRow() >= 0) {
        EditMyImages secd(this, peer, ui->listWidget->currentItem()->text());
        secd.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint |
                            Qt::CustomizeWindowHint);
        secd.setModal(true);
        secd.exec();

        ui->selectLabel->setVisible(false);
    }

    else {
        ui->selectLabel->setVisible(true);
        ui->selectLabel->setStyleSheet("QLabel { color : red; }");
        ui->selectLabel->setText("Please, select an image!");
    }
}

void SharedImages::on_backButton_clicked()
{
    SharedImages::close();
}



#include "allusers_images.h"
#include "ui_allusers_images.h"
#include <QIntValidator>
#include <QMessageBox>

allusers_images::allusers_images(QWidget *parent, Peer *peer, QString s)
    : QDialog(parent),
      ui(new Ui::allusers_images),
      peer(peer),
      selectedUser(s) {

    ui->setupUi(this);

    ui->lbl_time->setText(
        QString::fromStdString("Time Refreshed: " + peer->getCurrentTime()));

    ui->line_views->setPlaceholderText("No. of Views");
    ui->lbl_result->setVisible(false);

    ui->lbl_user->setText(selectedUser);

    vector<string> images;
    images = peer->users[selectedUser.toUtf8().constData()];

    for (int i = 0; i < images.size(); i++) {
      if (i > 2) {
        ui->listWidget->addItem(QString::fromStdString(images[i]));
      }
    }

    //makes sure numbers are inserted
    ui->line_views->setValidator(new QIntValidator);
  }


  void allusers_images::on_push_request_clicked() {
    if (ui->listWidget->currentRow() >= 0) {

        if(ui->line_views->text() != "") {
            cout << "Request button clicked" << endl;

            QString img = ui->listWidget->currentItem()->text();

            QString fullImageName = selectedUser + "_" + img;

            //check if already shared
            if(peer->sharedimgs.count(fullImageName.toUtf8().constData()) == 0){
                int result = peer->request_image(selectedUser.toUtf8().constData(),
                                                 img.toUtf8().constData(), ui->line_views->text().toInt());

                if (result == 1) {
                  ui->lbl_result->setStyleSheet("QLabel { color : green; }");
                  ui->lbl_result->setText(QString("Request sent!"));
                  ui->lbl_result->setVisible(true);
                }

                else if (result == 0)
                    QMessageBox::critical(this, "Request Previously Sent",
                                          "Please wait for your response!");
                else if (result == 2)
                    QMessageBox::critical(this, "Request Failed",
                                          "Timeout Error with DoS!");
                else
                    QMessageBox::critical(this, "Request Failed",
                                          "Connection Error: DoS or User OFFLINE!");
            }

            else
                QMessageBox::critical(this, "Image Already Shared",
                                      "Request More Views from 'Shared Images'!");
        }
        else
            QMessageBox::critical(this, "No Number Entered",
                                  "Enter a number!");
    }
    else
        QMessageBox::critical(this, "No Image Selected",
                               "Selent an image!");
  }

  allusers_images::~allusers_images() { delete ui; }
  void allusers_images::on_push_back_clicked() { allusers_images::close(); }



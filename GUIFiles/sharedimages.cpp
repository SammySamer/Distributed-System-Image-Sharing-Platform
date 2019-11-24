#include "sharedimages.h"
#include "ui_sharedimages.h"
#include "viewimage.h"
#include <QDebug>
#include <string>

SharedImages::SharedImages(QWidget *parent, Peer *peer)
    : QDialog(parent), ui(new Ui::SharedImages), peer(peer) {
  ui->setupUi(this);
  ui->line_views->setPlaceholderText("No. of Views");
  ui->lbl_time->setText(
      QString::fromStdString("Last Time Refreshed: " + peer->getCurrentTime()));
  //ui->lbl_time->setStyleSheet("QLabel { color : white; }");
  ui->lbl_result->setVisible(false);
  string imagename, ownername, fullname;
  //qDebug("%i", peer->sharedimgs.size());
  //printf("%i", peer->sharedimgs.size());
  //if (peer->sharedimgs.size() != 0)
  //{
      //printf("%i", peer->sharedimgs.size());
      //qDebug("%i", peer->sharedimgs.size());
      for (auto const &x : peer->sharedimgs) {
        fullname = x.first;
        for (int j = fullname.length() - 1; j > 0; j--) {
          if (fullname[j] == '_') {
            ownername = fullname.substr(0, j);
            imagename = fullname.substr(j + 1, fullname.length() - 1);
            break;
          }
        }
        ui->listWidget->addItem(
            QString::fromStdString("Owner: " + ownername + ". Img: " + imagename +
                                   ". Views Left: " + std::to_string(x.second)));
    }
  //}

  ui->line_views->setValidator(new QIntValidator); // only numbers
}

SharedImages::~SharedImages() { delete ui; }

void SharedImages::on_push_view_clicked() {
    // just in case it opens and takes time, or will be replaced
    ui->lbl_result->setVisible(true);
    ui->lbl_result->setStyleSheet("QLabel { color : green; }");
    ui->lbl_result->setText("Image Loading. Please, wait!");

    if (ui->listWidget->currentRow() >= 0) {
    int i = 0;
    int views = 0;
    string imagename, ownername, fullname, img, cover;
    for (auto const &x : peer->sharedimgs) {
      fullname = x.first;
      for (int j = fullname.length() - 1; j > 0; j--) {
        if (fullname[j] == '_') {
          ownername = fullname.substr(0, j);
          imagename = fullname.substr(j + 1, fullname.length() - 1);
          break;
        }
      }
      if (ui->listWidget->currentRow() == i) {
        img = imagename;
        cover = fullname;
        views = x.second;
        break;
      } else
        i++;
    }
    if (views > 0) {
      peer->sharedimgs[cover]--;
      cout << ownername << " " << imagename << " " << peer->sharedimgs[cover]
           << endl;


      int resultUsers = peer->notify_views_by_viewer(ownername, imagename,
                                   peer->sharedimgs[cover]);
      if(resultUsers == 1){
          ViewImage secd(this, peer, cover, img);
      secd.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint |
                          Qt::CustomizeWindowHint);
      secd.setModal(true);
      secd.exec();
      }
      else if(resultUsers == 12){
        ui->lbl_result->setVisible(true);
        ui->lbl_result->setStyleSheet("QLabel { color : red; }");
        ui->lbl_result->setText("DoS Offline!");
      }
      else if(resultUsers == 10){
        ui->lbl_result->setVisible(true);
        ui->lbl_result->setStyleSheet("QLabel { color : red; }");
        ui->lbl_result->setText("Check your internet connection!"); // Getusers send failed!
      }
      else if(resultUsers == 0){
        ui->lbl_result->setVisible(true);
        ui->lbl_result->setStyleSheet("QLabel { color : red; }");
        ui->lbl_result->setText("Check your internet connection!"); // notify_views_by_viewer at viewer failed!
      }
      else{
        ui->lbl_result->setVisible(true);
        ui->lbl_result->setStyleSheet("QLabel { color : red; }");
        ui->lbl_result->setText("Check your internet connection!");
      }
    } else {
      ui->lbl_result->setVisible(true);
      ui->lbl_result->setText(QString("You don't have enough views! Refresh or check notifications."));
      ui->lbl_result->setStyleSheet("QLabel { color : red; }");
    }
  } else {
    ui->lbl_result->setVisible(true);
    ui->lbl_result->setStyleSheet("QLabel { color : red; }");
    ui->lbl_result->setText("Please, select an image!");
  }
}

void SharedImages::on_push_refresh_clicked() {
  // get any updates
    ui->lbl_result->setVisible(false);
  ui->listWidget->clear();
  ui->lbl_time->setText(
      QString::fromStdString("Last Time Refreshed: " + peer->getCurrentTime()));
  //ui->lbl_time->setStyleSheet("QLabel { color : white; }");
  string fullname, imagename, ownername;
  for (auto const &x : peer->sharedimgs) {
    fullname = x.first;
    for (int j = fullname.length() - 1; j > 0; j--) {
      if (fullname[j] == '_') {
        ownername = fullname.substr(0, j);
        imagename = fullname.substr(j + 1, fullname.length() - 1);
        break;
      }
    }
    ui->listWidget->addItem(
        QString::fromStdString("Owner: " + ownername + ". Img: " + imagename +
                               ". Views Left: " + std::to_string(x.second)));
  }
}

void SharedImages::on_push_request_clicked()
{
    if (ui->listWidget->currentRow() >= 0) {
        if(ui->line_views->text() != ""){
        int i = 0;
        string imagename, ownername, fullname;
        int noViews = ui->line_views->text().toInt();
        for (auto const &x : peer->sharedimgs) {
            if(i == ui->listWidget->currentRow()){
                fullname = x.first;
                for (int j = fullname.length() - 1; j > 0; j--) {
                  if (fullname[j] == '_') {
                    ownername = fullname.substr(0, j);
                    imagename = fullname.substr(j + 1, fullname.length() - 1);
                    break;
                  }
                }

            }
          i++;
        }
        int result = peer->update_views_request_by_viewer(ownername, imagename, noViews);
        if (result == 1) {
          ui->lbl_result->setStyleSheet("QLabel { color : green; }");
          ui->lbl_result->setText(QString("Request sent!"));
          ui->lbl_result->setVisible(true);
        } else if (result == 3) {
          ui->lbl_result->setStyleSheet("QLabel { color : red; }");
          ui->lbl_result->setText(QString("Check your internet connection!")); // Request send failed!
          ui->lbl_result->setVisible(true);
        } else if (result == 0) {
          ui->lbl_result->setStyleSheet("QLabel { color : red; }");
          ui->lbl_result->setText(
              QString("Request sent before! Wait for owner response!"));
          ui->lbl_result->setVisible(true);
        } else if (result == 2) {
          ui->lbl_result->setStyleSheet("QLabel { color : red; }");
          ui->lbl_result->setText(
              QString("Check your internet connection!"));
          ui->lbl_result->setVisible(true);
        }
        else if(result == 12){
          ui->lbl_result->setVisible(true);
          ui->lbl_result->setStyleSheet("QLabel { color : red; }");
          ui->lbl_result->setText("DoS Offline!");
        }
        else if(result == 10){
          ui->lbl_result->setVisible(true);
          ui->lbl_result->setStyleSheet("QLabel { color : red; }");
          ui->lbl_result->setText("Check your internet connection!"); // Getusers send failed!
        }
        else {
          ui->lbl_result->setStyleSheet("QLabel { color : red; }");
          ui->lbl_result->setText(QString("Check your internet connection!"));
          ui->lbl_result->setVisible(true);
        }
        } else{
            ui->lbl_result->setVisible(true);
            ui->lbl_result->setStyleSheet("QLabel { color : red; }");
            ui->lbl_result->setText("Please, enter the amount of views you want!");
        }
    } else {
      ui->lbl_result->setVisible(true);
      ui->lbl_result->setStyleSheet("QLabel { color : red; }");
      ui->lbl_result->setText("Please, select an image!");
    }
}

void SharedImages::on_push_back_clicked()
{
    SharedImages::close();
}

#include "editmyimages.h"
#include "ui_editmyimages.h"
#include "profile.h"
#include <QString>
#include <string>

EditMyImages::EditMyImages(QWidget *parent, Peer *peer, QString imagename) :
    QDialog(parent),
    ui(new Ui::EditMyImages),
    peer(peer),
    imagename(imagename)
{
    ui->setupUi(this);
    ui->line_views->setPlaceholderText("No. of Views");
    ui->line_views->setValidator(new QIntValidator); // only numbers
    QPixmap pm(imagename);
    ui->lbl_image->setPixmap(
        pm.scaled(200, 200, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->lbl_time->setText(
        QString::fromStdString("Last Time Refreshed: " + peer->getCurrentTime()));
    //ui->lbl_time->setStyleSheet("QLabel { color : white; }");
    ui->lbl_result->setVisible(false);
    vector<pair<string, int>> selectedViewers =
        peer->myimages[imagename.toStdString()];
    for (int i = 0; i < selectedViewers.size(); i++) {
      if(selectedViewers[i].first != "")
        ui->listWidget->addItem(QString::fromStdString(
          "Viewer: " + selectedViewers[i].first +
          ". Views Left: " + std::to_string(selectedViewers[i].second)));
    }
}

void EditMyImages::on_push_refresh_clicked()
{
    ui->listWidget->clear();
    ui->lbl_time->setText(
        QString::fromStdString("Last Time Refreshed: " + peer->getCurrentTime()));
    ui->lbl_time->setStyleSheet("QLabel { color : white; }");
    ui->lbl_result->setVisible(false);
    vector<pair<string, int>> selectedViewers =
        peer->myimages[imagename.toStdString()];
    for (int i = 0; i < selectedViewers.size(); i++) {
      ui->listWidget->addItem(QString::fromStdString(
          "Viewer: " + selectedViewers[i].first +
          ". Views Left: " + std::to_string(selectedViewers[i].second)));
    }
}

void EditMyImages::on_push_update_views_clicked()
{
    if (ui->listWidget->currentRow() >= 0) {
        if(ui->line_views->text() != ""){

    int noViews = ui->line_views->text().toInt();
    if (noViews < 0) {
      ui->lbl_result->setVisible(true);
      ui->lbl_result->setText("Please, add the number of views >= 0");
      ui->lbl_result->setStyleSheet("QLabel { color : red; }");
    } else {
      vector<pair<string, int>> selectedViewers =
          peer->myimages[imagename.toStdString()];
      string viewer = selectedViewers[ui->listWidget->currentRow()].first;
      int resultUsers = peer->update_views_by_owner(viewer, imagename.toStdString(), noViews);
    if(resultUsers == 1){
      ui->lbl_result->setVisible(true);
      ui->lbl_result->setStyleSheet("QLabel { color : green; }");
      ui->lbl_result->setText("Views updated!");
    }
    else if(resultUsers == 12){
      ui->lbl_result->setVisible(true);
      ui->lbl_result->setStyleSheet("QLabel { color : red; }");
      ui->lbl_result->setText("DoS Offline!");
    }
    else if(resultUsers == 10){
      ui->lbl_result->setVisible(true);
      ui->lbl_result->setStyleSheet("QLabel { color : red; }");
      ui->lbl_result->setText("Check your internet connection!"); // send failed
    }
    else if(resultUsers == 0){
      ui->lbl_result->setVisible(true);
      ui->lbl_result->setStyleSheet("QLabel { color : red; }");
      ui->lbl_result->setText("Check your internet connection!"); // update_views_by_owner at viewer failed!
    }
    else{
      ui->lbl_result->setVisible(true);
      ui->lbl_result->setStyleSheet("QLabel { color : red; }");
      ui->lbl_result->setText("Check your internet connection!"); // something wrong
    }
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

EditMyImages::~EditMyImages() { delete ui; }
void EditMyImages::on_push_back_clicked() { EditMyImages::close(); }

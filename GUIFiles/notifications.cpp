#include "notifications.h"
#include "ui_notifications.h"
#include "profile.h"
#include <string>

Notifications::Notifications(QWidget *parent, Peer *peer) :
    QDialog(parent),
    ui(new Ui::Notifications),
    peer(peer)
{
    ui->setupUi(this);

    ui->line_views->setValidator(new QIntValidator); // only numbers
    ui->lbl_time->setText(
        QString::fromStdString("Last Time Refreshed: " + peer->getCurrentTime()));

    //ui->lbl_time->setStyleSheet("QLabel { color : white; }");
    ui->line_views->setPlaceholderText("No. of Views");
    ui->lbl_result->setVisible(false);
    int resultUsers = peer->getUsers(); // because approve needs IP & port
    if(resultUsers == 1){
    for (int i = 0; i < peer->requests_buffer.size(); i++) {
      string req;
      if (peer->requests_buffer[i].first == 2002)
        req = " wants to view ";
      else if (peer->requests_buffer[i].first == 2004)
        req = " approved you to view ";
      else if (peer->requests_buffer[i].first == 2007)
        req = " want to update his number of views to ";
      else
        req = " Check your internet connection ";
      ui->listWidget->addItem(
          QString::fromStdString(peer->requests_buffer[i].second.name + req +
                                 peer->requests_buffer[i].second.imagename + " "+to_string(peer->requests_buffer[i].second.views)+ " times"));
        }
    }


}


void Notifications::on_push_approve_clicked() {
  if (ui->listWidget->currentRow() >= 0) {
    string usname =
        peer->requests_buffer[ui->listWidget->currentRow()].second.name;
    string imname =
        peer->requests_buffer[ui->listWidget->currentRow()].second.imagename;
    int opcode = peer->requests_buffer[ui->listWidget->currentRow()].first;
    if(ui->line_views->text() == "" && (opcode == 2002 || opcode == 2007)){
        ui->lbl_result->setVisible(true);
        ui->lbl_result->setStyleSheet("QLabel { color : red; }");
        ui->lbl_result->setText("Please, enter number of views!");
    }
    else{
        int noViews = ui->line_views->text().toInt();

        if (opcode == 2002){ // req = " wants to view ";
            if (noViews <= 0) {
              ui->lbl_result->setVisible(true);
              ui->lbl_result->setText("Please, add the number of views > 0");
              ui->lbl_result->setStyleSheet("QLabel { color : red; }");
            } else {
              cout << "ApprovedUser " << usname << " imagename " << imname << endl;
              std::thread SendImageThread(&Peer::send_image, peer, usname, imname,
                                          noViews);
              SendImageThread.detach();
              ui->lbl_result->setVisible(true);
              ui->lbl_result->setText("Approved!");
              ui->lbl_result->setStyleSheet("QLabel { color : green; }");

            }
        }
        else if (opcode == 2004){ // req = " approved you to view ";
            ui->lbl_result->setVisible(true);
            ui->lbl_result->setText("You were approved! Nothing to approve!");
            ui->lbl_result->setStyleSheet("QLabel { color : red; }");
        }
        else if (opcode == 2007){ // req = " want to update his number of views to ";
            if (noViews < 0) {
              ui->lbl_result->setVisible(true);
              ui->lbl_result->setText("Please, add the number of views >= 0");
              ui->lbl_result->setStyleSheet("QLabel { color : red; }");
            } else {
              cout << "Updating views of " << usname << " imagename " << imname << endl;
              int resultUsers = peer->update_views_by_owner(usname, imname, noViews);
              if(resultUsers == 1){
              ui->lbl_result->setVisible(true);
              ui->lbl_result->setText("Updated!");
              ui->lbl_result->setStyleSheet("QLabel { color : green; }");
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
                ui->lbl_result->setText("Check your internet connection!"); // update_views_by_owner at viewer failed!
              }
              else{
                ui->lbl_result->setVisible(true);
                ui->lbl_result->setStyleSheet("QLabel { color : red; }");
                ui->lbl_result->setText("Check your internet connection!");
              }
            }
        }
        else{
            ui->lbl_result->setVisible(true);
            ui->lbl_result->setText("Check your internet connection!");
            ui->lbl_result->setStyleSheet("QLabel { color : red; }");
        }

        peer->requests_buffer.erase(peer->requests_buffer.begin() +
                                    ui->listWidget->currentRow());

        // refill list
        ui->listWidget->clear();
        for (int i = 0; i < peer->requests_buffer.size(); i++) {
          string req;
          if (peer->requests_buffer[i].first == 2002)
            req = " wants to view ";
          else if (peer->requests_buffer[i].first == 2004)
            req = " approved you to view ";
          else if (peer->requests_buffer[i].first == 2007)
            req = " want to update his number of views to ";
          else
            req = " Check your internet connection ";
          ui->listWidget->addItem(
              QString::fromStdString(peer->requests_buffer[i].second.name + req +
                                     peer->requests_buffer[i].second.imagename + " "+to_string(peer->requests_buffer[i].second.views)+ "times"));

        }

    }
  }
   else {
    ui->lbl_result->setVisible(true);
    ui->lbl_result->setStyleSheet("QLabel { color : red; }");
    ui->lbl_result->setText("Please, select a notification!");
  }
}

void Notifications::on_push_refresh_clicked() { // refill list
  ui->lbl_result->setVisible(false);
  ui->lbl_time->setText(
      QString::fromStdString("Last Time Refreshed: " + peer->getCurrentTime()));
  //ui->lbl_time->setStyleSheet("QLabel { color : white; }");
  ui->listWidget->clear();
  int resultUsers = peer->getUsers(); // because approve needs IP & port
  if(resultUsers == 1){
  for (int i = 0; i < peer->requests_buffer.size(); i++) {
    string req;
    if (peer->requests_buffer[i].first == 2002)
      req = " wants to view ";
    else if (peer->requests_buffer[i].first == 2003)
      req = " approved you to view ";
    else if (peer->requests_buffer[i].first == 2007)
      req = " want to update his number of views to ";
    else
      req = " Check your internet connection ";
    ui->listWidget->addItem(
        QString::fromStdString(peer->requests_buffer[i].second.name + req +
                               peer->requests_buffer[i].second.imagename + " "+to_string(peer->requests_buffer[i].second.views)+ "times"));

  }
  }
  else if(resultUsers == 2){
    ui->lbl_result->setVisible(true);
    ui->lbl_result->setStyleSheet("QLabel { color : red; }");
    ui->lbl_result->setText("DoS Offline!");
  }
  else if(resultUsers == 0){
    ui->lbl_result->setVisible(true);
    ui->lbl_result->setStyleSheet("QLabel { color : red; }");
    ui->lbl_result->setText("Getusers send failed!");
  }
  else{
    ui->lbl_result->setVisible(true);
    ui->lbl_result->setStyleSheet("QLabel { color : red; }");
    ui->lbl_result->setText("Check your internet connection!");
  }
}

void Notifications::on_push_delete_clicked() {
  if (ui->listWidget->currentRow() >= 0) {
    // Refaay: should reply with request disapproved

    peer->requests_buffer.erase(peer->requests_buffer.begin() +
                                ui->listWidget->currentRow());

    ui->listWidget->clear();
    for (int i = 0; i < peer->requests_buffer.size(); i++) {
      string req;
      if (peer->requests_buffer[i].first == 2002)
        req = " wants to view ";
      else if (peer->requests_buffer[i].first == 2003)
        req = " approved you to view ";
      else if (peer->requests_buffer[i].first == 2007)
        req = " want to update his number of views to ";
      else
        req = " Check your internet connection ";
      ui->listWidget->addItem(
          QString::fromStdString(peer->requests_buffer[i].second.name + req +
                                 peer->requests_buffer[i].second.imagename + " "+to_string(peer->requests_buffer[i].second.views)+ "times"));
    }
    ui->lbl_result->setVisible(true);
    ui->lbl_result->setStyleSheet("QLabel { color : green; }");
    ui->lbl_result->setText("Notification deleted!");
  } else {
    ui->lbl_result->setVisible(true);
    ui->lbl_result->setStyleSheet("QLabel { color : red; }");
    ui->lbl_result->setText("Please, select a notification!");
  }
}

Notifications::~Notifications() { delete ui; }
void Notifications::on_push_back_clicked() { Notifications::close();}

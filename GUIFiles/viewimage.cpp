#include "ui_viewimage.h"
#include "viewimage.h"
#include <qpicture.h>

ViewImage::ViewImage(QWidget *parent, Peer *peer, string cover,
                                 string img)
    : QDialog(parent), ui(new Ui::ViewImage), peer(peer) {
  ui->setupUi(this);
  ui->lbl_time->setText(
      QString::fromStdString("Last Time Refreshed: " + peer->getCurrentTime()));
  ui->lbl_time->setStyleSheet("QLabel { color : white; }");
  ui->lbl_result->setVisible(false);

  connect(this, SIGNAL(destroyed()), this->parent(),
          SLOT(on_push_refresh_clicked()));

  string extract_command;

  extract_command =
      "steghide extract -sf " + cover + " -p hk";

  QProcess::execute(QString::fromStdString(extract_command));
  QPixmap pm(QString::fromStdString(img));
  ui->lbl_image->setPixmap(
      pm.scaled(500, 500, Qt::IgnoreAspectRatio, Qt::FastTransformation));

  string delete_cmd =
      "rm " + img;
  QProcess::execute(QString::fromStdString(delete_cmd));
}

ViewImage::~ViewImage() { delete ui; }

void ViewImage::on_push_back_clicked()
{
    ViewImage::close();
}

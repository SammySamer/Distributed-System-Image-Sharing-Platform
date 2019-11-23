#ifndef ALLUSERS_IMAGES_H
#define ALLUSERS_IMAGES_H

#include <QDialog>
#include <peer.h>

namespace Ui {
class allusers_images;
}

class allusers_images : public QDialog
{
    Q_OBJECT

public:
  explicit allusers_images(QWidget *parent = nullptr, Peer *peer = nullptr,
                            QString s = "");
  ~allusers_images();

private slots:

  void on_push_request_clicked();
  void on_push_back_clicked();

private:
  Ui::allusers_images *ui;
  Peer *peer;
  QString selectedUser;

};
#endif

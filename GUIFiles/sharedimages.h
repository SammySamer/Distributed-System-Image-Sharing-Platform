#ifndef SHAREDIMAGES_H
#define SHAREDIMAGES_H

#include <QDialog>
#include "peer.h"
#include "profile.h"
#include "editmyimages.h"

namespace Ui {
class SharedImages;
}

class SharedImages : public QDialog
{
    Q_OBJECT

public:
  explicit SharedImages(QWidget *parent = nullptr, Peer *peer = nullptr);
  ~SharedImages();

private slots:
  void on_push_view_clicked();

  void on_push_refresh_clicked();

  void on_push_request_clicked();

  void on_push_back_clicked();

private:
  Ui::SharedImages *ui;
  Peer *peer;
};

#endif // SHAREDIMAGES_H

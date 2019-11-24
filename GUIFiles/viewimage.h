#ifndef VIEWIMAGE_H
#define VIEWIMAGE_H

#include <QDialog>
#include "peer.h"

namespace Ui {
class ViewImage;
}

class ViewImage : public QDialog
{
    Q_OBJECT

public:
    explicit ViewImage(QWidget *parent = nullptr, Peer *peer = nullptr,
                       string cover = "", string img = "");
    ~ViewImage();

private slots:
  void on_push_back_clicked();

private:
  Ui::ViewImage *ui;
  Peer *peer;
};

#endif // VIEWIMAGE_H

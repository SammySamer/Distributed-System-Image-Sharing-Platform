#ifndef MYIMAGES_H
#define MYIMAGES_H

#include <QDialog>
#include "peer.h"

namespace Ui {
class MyImages;
}

class MyImages : public QDialog
{
    Q_OBJECT

public:
    explicit MyImages(QWidget *parent = nullptr, Peer *peer = nullptr);
    ~MyImages();

private slots:
    void on_push_back_clicked();

    void on_push_view_clicked();

private:
    Ui::MyImages *ui;
    Peer *peer;
};

#endif // MYIMAGES_H

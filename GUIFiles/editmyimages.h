#ifndef EDITMYIMAGES_H
#define EDITMYIMAGES_H

#include <QDialog>
#include "peer.h"

namespace Ui {
class EditMyImages;
}

class EditMyImages : public QDialog
{
    Q_OBJECT

public:
    explicit EditMyImages(QWidget *parent = nullptr, Peer *peer = nullptr, QString imagename = "");
    ~EditMyImages();

private slots:
    void on_push_back_clicked();

    void on_push_refresh_clicked();

    void on_push_update_views_clicked();

private:
    Ui::EditMyImages *ui;
    Peer *peer;
    QString imagename;
};

#endif // EDITMYIMAGES_H

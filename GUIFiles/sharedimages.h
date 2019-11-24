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
    void on_showButton_clicked();

    void on_backButton_clicked();

private:
    Ui::SharedImages *ui;
    Peer *peer;
};

#endif // SHAREDIMAGES_H

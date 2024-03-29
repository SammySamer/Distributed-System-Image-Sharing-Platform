#ifndef PROFILE_H
#define PROFILE_H

#include <QDialog>

#include "peer.h"
#include "sharedimages.h"
#include "allusers.h"
#include "myimages.h"
#include "notifications.h"

namespace Ui {
class Profile;
}

class Profile : public QDialog
{
    Q_OBJECT

public:
    explicit Profile(QWidget *parent = nullptr, Peer *peer = nullptr);
    ~Profile();

private slots:
    void on_allusersButton_clicked();

    void on_sharedImButton_clicked();

    void on_notifications_clicked();

    void on_push_myimages_clicked();

    void on_upload_images_clicked();

    void on_push_logout_clicked();

private:
    Ui::Profile *ui;
    Peer *peer;
};

#endif // PROFILE_H

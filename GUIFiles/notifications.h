#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <QDialog>
#include "peer.h"

namespace Ui {
class Notifications;
}

class Notifications : public QDialog
{
    Q_OBJECT

public:
    explicit Notifications(QWidget *parent = nullptr, Peer *peer = nullptr);
    //void showNotifications();
    ~Notifications();

private slots:
  void on_push_approve_clicked();

  void on_push_refresh_clicked();

  void on_push_delete_clicked();

  void on_push_back_clicked();

private:
    Ui::Notifications *ui;
    Peer *peer;
    QString selectedRequest;
};

#endif // NOTIFICATIONS_H

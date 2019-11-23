#ifndef ALLUSERS_H
#define ALLUSERS_H

#include <QDialog>
#include <string>
#include <QtDebug>
#include <QList>
#include <QLabel>
#include <QSizePolicy>

#include "peer.h"
#include "allusers_images.h"


namespace Ui {
class AllUsers;
}

class AllUsers : public QDialog
{
    Q_OBJECT

public:
    explicit AllUsers(QWidget *parent = nullptr, Peer *peer = nullptr);
    ~AllUsers();
    void showUsers();

private slots:
    void on_push_back_clicked();

    void on_push_view_clicked();
    
    void on_push_refresh_clicked();
    
private:
    Ui::AllUsers *ui;
    allusers_images *images;
    Peer *peer;
};

#endif // ALLUSERS_H

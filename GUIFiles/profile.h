#ifndef PROFILE_H
#define PROFILE_H

#include <QDialog>
#include "allusers.h"

namespace Ui {
class Profile;
}

class Profile : public QDialog
{
    Q_OBJECT

public:
    explicit Profile(QWidget *parent = nullptr);
    ~Profile();

private slots:
    void on_allusersButton_clicked();

private:
    Ui::Profile *ui;
};

#endif // PROFILE_H

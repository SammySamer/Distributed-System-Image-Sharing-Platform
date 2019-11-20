#ifndef ALLUSERS_H
#define ALLUSERS_H

#include <QDialog>
#include <string>
#include <QtDebug>
#include <QList>
#include <QLabel>
#include <QSizePolicy>


namespace Ui {
class AllUsers;
}

class AllUsers : public QDialog
{
    Q_OBJECT

public:
    explicit AllUsers(QWidget *parent = nullptr);
    ~AllUsers();
    void showUsers();

private:
    Ui::AllUsers *ui;
};

#endif // ALLUSERS_H

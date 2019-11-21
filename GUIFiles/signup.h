#ifndef SIGNUP_H
#define SIGNUP_H

#include <QDialog>
#include "peer.h"

namespace Ui {
class Signup;
}

class Signup : public QDialog
{
    Q_OBJECT

public:
    explicit Signup(QWidget *parent = nullptr);
    ~Signup();

private slots:
    void on_registerButton_clicked();

private:
    Ui::Signup *ui;
    Peer *peer;
};

#endif // SIGNUP_H

#ifndef ALLUSERS_IMAGES_H
#define ALLUSERS_IMAGES_H

#include <QDialog>

namespace Ui {
class allusers_images;
}

class allusers_images : public QDialog
{
    Q_OBJECT

public:
    explicit allusers_images(QWidget *parent = nullptr);
    ~allusers_images();

private:
    Ui::allusers_images *ui;
};

#endif // ALLUSERS_IMAGES_H

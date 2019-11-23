#include "allusers_images.h"
#include "ui_allusers_images.h"

allusers_images::allusers_images(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::allusers_images)
{
    ui->setupUi(this);
}

allusers_images::~allusers_images()
{
    delete ui;
}

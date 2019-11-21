#include "allusers.h"
#include "ui_allusers.h"
#include "peer.h"
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <iostream>

AllUsers::AllUsers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AllUsers)
{
    ui->setupUi(this);
}

void AllUsers::showUsers()
{
    // GET NUMBER OF USERS
    const int noOfUsers = 10;

    QLabel *label[noOfUsers];
    //QLabel *online[noOfUsers];
    bool is_online = true;

    for (int i = 0; i < noOfUsers; i++)
    {
        char* username;

        // CHECK IF ONLINE
        if (!is_online)
            username = "username";
        else
            username = "username - ONLINE";

        label[i] = new QLabel (this);
        label[i]->setText(username);
        label[i]->setIndent(15*i);
        label[i]->setAlignment(Qt::AlignTop | Qt::AlignRight);

        /*online[i] = new QLabel (this);
        online[i]->setText(" - ONLINE");
        online[i]->setIndent(15*i);
        online[i]->setAlignment(Qt::AlignTop | Qt::AlignRight);*/

 }

    /*QList<QLabel *> labels;
    for (int i = 0; i < 10; ++i)
       labels << new QLabel(this);

    for (int i = 0; i < 10; ++i)
    {
       labels.at(i)->setText(QString::number(i));
       labels.at(i)->show();
    }
    */
    
    /*
    for (int i=1; i<=10; i++)       // 10 labels
    {
        char* nameLabel = strcat("nameLabel",(char*) i);
        char* username = "username";

        // CHECK IF ONLINE
        bool online = false;

        if(i<=noOfUsers)
        {
            // for each user
            ui->nameLabel1->setText(username);
            ui->nameLabel1->show();

            if (online)
            {
                ui->onLabel1->show();
            }
            else
            {
                ui->onLabel1->hide();
            }
        }

        else
        {
            // for each user
            ui->nameLabel1->hide();
            ui->onLabel1->hide();
        }
    }*/
    
}

AllUsers::~AllUsers()
{
    delete ui;
}

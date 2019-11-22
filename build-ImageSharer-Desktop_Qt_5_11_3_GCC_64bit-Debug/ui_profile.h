/********************************************************************************
** Form generated from reading UI file 'profile.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILE_H
#define UI_PROFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Profile
{
public:
    QLabel *label;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QPushButton *allusersButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;

    void setupUi(QDialog *Profile)
    {
        if (Profile->objectName().isEmpty())
            Profile->setObjectName(QStringLiteral("Profile"));
        Profile->resize(554, 503);
        label = new QLabel(Profile);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 30, 71, 16));
        widget = new QWidget(Profile);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(200, 90, 151, 361));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        allusersButton = new QPushButton(widget);
        allusersButton->setObjectName(QStringLiteral("allusersButton"));

        verticalLayout->addWidget(allusersButton);

        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(widget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        verticalLayout->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(widget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        verticalLayout->addWidget(pushButton_4);

        pushButton_5 = new QPushButton(widget);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));

        verticalLayout->addWidget(pushButton_5);

        pushButton_6 = new QPushButton(Profile);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));
        pushButton_6->setGeometry(QRect(410, 20, 113, 32));

        retranslateUi(Profile);

        QMetaObject::connectSlotsByName(Profile);
    } // setupUi

    void retranslateUi(QDialog *Profile)
    {
        Profile->setWindowTitle(QApplication::translate("Profile", "Profile", nullptr));
        label->setText(QApplication::translate("Profile", "username", nullptr));
        allusersButton->setText(QApplication::translate("Profile", "All Users", nullptr));
        pushButton_2->setText(QApplication::translate("Profile", "Shared Images", nullptr));
        pushButton_3->setText(QApplication::translate("Profile", "My Images", nullptr));
        pushButton_4->setText(QApplication::translate("Profile", "Notifications", nullptr));
        pushButton_5->setText(QApplication::translate("Profile", "Upload", nullptr));
        pushButton_6->setText(QApplication::translate("Profile", "Logout", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Profile: public Ui_Profile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILE_H

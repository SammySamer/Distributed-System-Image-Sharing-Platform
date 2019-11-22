/********************************************************************************
** Form generated from reading UI file 'allusers.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALLUSERS_H
#define UI_ALLUSERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_AllUsers
{
public:
    QPushButton *push_back;
    QListWidget *listWidget;
    QPushButton *push_refresh;
    QPushButton *push_view;
    QLabel *lbl_time;
    QLabel *label;
    QLabel *lbl_result;

    void setupUi(QDialog *AllUsers)
    {
        if (AllUsers->objectName().isEmpty())
            AllUsers->setObjectName(QStringLiteral("AllUsers"));
        AllUsers->resize(484, 460);
        push_back = new QPushButton(AllUsers);
        push_back->setObjectName(QStringLiteral("push_back"));
        push_back->setGeometry(QRect(380, 10, 89, 25));
        listWidget = new QListWidget(AllUsers);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(10, 80, 461, 321));
        push_refresh = new QPushButton(AllUsers);
        push_refresh->setObjectName(QStringLiteral("push_refresh"));
        push_refresh->setGeometry(QRect(10, 10, 89, 25));
        push_view = new QPushButton(AllUsers);
        push_view->setObjectName(QStringLiteral("push_view"));
        push_view->setGeometry(QRect(10, 410, 201, 25));
        lbl_time = new QLabel(AllUsers);
        lbl_time->setObjectName(QStringLiteral("lbl_time"));
        lbl_time->setGeometry(QRect(10, 440, 461, 17));
        label = new QLabel(AllUsers);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 50, 67, 17));
        lbl_result = new QLabel(AllUsers);
        lbl_result->setObjectName(QStringLiteral("lbl_result"));
        lbl_result->setGeometry(QRect(220, 410, 241, 17));

        retranslateUi(AllUsers);

        QMetaObject::connectSlotsByName(AllUsers);
    } // setupUi

    void retranslateUi(QDialog *AllUsers)
    {
        AllUsers->setWindowTitle(QApplication::translate("AllUsers", "All Users", nullptr));
        push_back->setText(QApplication::translate("AllUsers", "Back", nullptr));
        push_refresh->setText(QApplication::translate("AllUsers", "Refresh", nullptr));
        push_view->setText(QApplication::translate("AllUsers", "View Selected User Images", nullptr));
        lbl_time->setText(QApplication::translate("AllUsers", "Time", nullptr));
        label->setText(QApplication::translate("AllUsers", "All Users", nullptr));
        lbl_result->setText(QApplication::translate("AllUsers", "Selected User is Offline", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AllUsers: public Ui_AllUsers {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALLUSERS_H

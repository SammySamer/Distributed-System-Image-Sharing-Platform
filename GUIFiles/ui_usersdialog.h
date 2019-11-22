/********************************************************************************
** Form generated from reading UI file 'usersdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERSDIALOG_H
#define UI_USERSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_UsersDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *push_refresh;
    QSpacerItem *horizontalSpacer;
    QPushButton *push_back;
    QLabel *label;
    QListWidget *listWidget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *push_view;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lbl_result;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_7;
    QLabel *lbl_time;
    QSpacerItem *horizontalSpacer_8;

    void setupUi(QDialog *UsersDialog)
    {
        if (UsersDialog->objectName().isEmpty())
            UsersDialog->setObjectName(QStringLiteral("UsersDialog"));
        UsersDialog->resize(500, 500);
        UsersDialog->setStyleSheet(QLatin1String("background-color: rgb(46, 52, 54)\n"
""));
        verticalLayout = new QVBoxLayout(UsersDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        push_refresh = new QPushButton(UsersDialog);
        push_refresh->setObjectName(QStringLiteral("push_refresh"));
        push_refresh->setStyleSheet(QStringLiteral("color: rgb(238, 238, 236)"));

        horizontalLayout_2->addWidget(push_refresh);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        push_back = new QPushButton(UsersDialog);
        push_back->setObjectName(QStringLiteral("push_back"));
        push_back->setStyleSheet(QStringLiteral("color: rgb(238, 238, 236)"));

        horizontalLayout_2->addWidget(push_back);


        verticalLayout->addLayout(horizontalLayout_2);

        label = new QLabel(UsersDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setStyleSheet(QStringLiteral("color: rgb(238, 238, 236)"));

        verticalLayout->addWidget(label);

        listWidget = new QListWidget(UsersDialog);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setStyleSheet(QStringLiteral("color: rgb(238, 238, 236)"));

        verticalLayout->addWidget(listWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        push_view = new QPushButton(UsersDialog);
        push_view->setObjectName(QStringLiteral("push_view"));
        push_view->setStyleSheet(QStringLiteral("color: rgb(238, 238, 236)"));

        horizontalLayout->addWidget(push_view);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        lbl_result = new QLabel(UsersDialog);
        lbl_result->setObjectName(QStringLiteral("lbl_result"));

        horizontalLayout_3->addWidget(lbl_result);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        lbl_time = new QLabel(UsersDialog);
        lbl_time->setObjectName(QStringLiteral("lbl_time"));
        lbl_time->setStyleSheet(QStringLiteral("color: rgb(238, 238, 236)"));

        horizontalLayout_7->addWidget(lbl_time);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_8);


        verticalLayout->addLayout(horizontalLayout_7);

        QWidget::setTabOrder(listWidget, push_view);
        QWidget::setTabOrder(push_view, push_refresh);
        QWidget::setTabOrder(push_refresh, push_back);

        retranslateUi(UsersDialog);
        QObject::connect(push_back, SIGNAL(clicked()), UsersDialog, SLOT(close()));

        QMetaObject::connectSlotsByName(UsersDialog);
    } // setupUi

    void retranslateUi(QDialog *UsersDialog)
    {
        UsersDialog->setWindowTitle(QApplication::translate("UsersDialog", "DistImage", nullptr));
        push_refresh->setText(QApplication::translate("UsersDialog", "Refresh", nullptr));
        push_back->setText(QApplication::translate("UsersDialog", "Back", nullptr));
        label->setText(QApplication::translate("UsersDialog", "All Users:", nullptr));
        push_view->setText(QApplication::translate("UsersDialog", "View Selected User Images", nullptr));
        lbl_result->setText(QApplication::translate("UsersDialog", "Selected User is Offline!", nullptr));
        lbl_time->setText(QApplication::translate("UsersDialog", "Time", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UsersDialog: public Ui_UsersDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERSDIALOG_H

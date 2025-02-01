/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.15
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QLabel *label;
    QLineEdit *libraryPathEdit;
    QPushButton *btnBrowse;
    QPushButton *btnSave;
    QPushButton *btnCancel;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QString::fromUtf8("SettingsDialog"));
        SettingsDialog->resize(400, 300);
        label = new QLabel(SettingsDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 131, 18));
        libraryPathEdit = new QLineEdit(SettingsDialog);
        libraryPathEdit->setObjectName(QString::fromUtf8("libraryPathEdit"));
        libraryPathEdit->setGeometry(QRect(160, 10, 201, 26));
        btnBrowse = new QPushButton(SettingsDialog);
        btnBrowse->setObjectName(QString::fromUtf8("btnBrowse"));
        btnBrowse->setGeometry(QRect(60, 50, 88, 26));
        btnSave = new QPushButton(SettingsDialog);
        btnSave->setObjectName(QString::fromUtf8("btnSave"));
        btnSave->setGeometry(QRect(70, 200, 88, 26));
        btnCancel = new QPushButton(SettingsDialog);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
        btnCancel->setGeometry(QRect(200, 200, 88, 26));

        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Music Library Path:", nullptr));
        btnBrowse->setText(QCoreApplication::translate("SettingsDialog", "Browse...", nullptr));
        btnSave->setText(QCoreApplication::translate("SettingsDialog", "Save", nullptr));
        btnCancel->setText(QCoreApplication::translate("SettingsDialog", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H

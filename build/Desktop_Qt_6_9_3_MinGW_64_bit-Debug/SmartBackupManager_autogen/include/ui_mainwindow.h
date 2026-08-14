/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QGroupBox *backupConfigurationGroup;
    QGridLayout *gridLayout;
    QLabel *sourceLabel;
    QLineEdit *sourcePathEdit;
    QPushButton *browseSourceButton;
    QLabel *destinationLabel;
    QLineEdit *destinationPathEdit;
    QPushButton *browseDestinationButton;
    QPushButton *startBackupButton;
    QLabel *progressLabel;
    QProgressBar *backupProgressBar;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(900, 600);
        MainWindow->setMinimumSize(QSize(900, 600));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        titleLabel = new QLabel(centralwidget);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setGeometry(QRect(80, 40, 181, 16));
        subtitleLabel = new QLabel(centralwidget);
        subtitleLabel->setObjectName("subtitleLabel");
        subtitleLabel->setGeometry(QRect(80, 60, 241, 16));
        backupConfigurationGroup = new QGroupBox(centralwidget);
        backupConfigurationGroup->setObjectName("backupConfigurationGroup");
        backupConfigurationGroup->setGeometry(QRect(80, 110, 601, 191));
        gridLayout = new QGridLayout(backupConfigurationGroup);
        gridLayout->setObjectName("gridLayout");
        sourceLabel = new QLabel(backupConfigurationGroup);
        sourceLabel->setObjectName("sourceLabel");

        gridLayout->addWidget(sourceLabel, 0, 0, 1, 1);

        sourcePathEdit = new QLineEdit(backupConfigurationGroup);
        sourcePathEdit->setObjectName("sourcePathEdit");
        sourcePathEdit->setReadOnly(true);

        gridLayout->addWidget(sourcePathEdit, 1, 0, 1, 1);

        browseSourceButton = new QPushButton(backupConfigurationGroup);
        browseSourceButton->setObjectName("browseSourceButton");

        gridLayout->addWidget(browseSourceButton, 1, 1, 1, 1);

        destinationLabel = new QLabel(backupConfigurationGroup);
        destinationLabel->setObjectName("destinationLabel");

        gridLayout->addWidget(destinationLabel, 2, 0, 1, 1);

        destinationPathEdit = new QLineEdit(backupConfigurationGroup);
        destinationPathEdit->setObjectName("destinationPathEdit");
        destinationPathEdit->setReadOnly(true);

        gridLayout->addWidget(destinationPathEdit, 3, 0, 1, 1);

        browseDestinationButton = new QPushButton(backupConfigurationGroup);
        browseDestinationButton->setObjectName("browseDestinationButton");

        gridLayout->addWidget(browseDestinationButton, 3, 1, 1, 1);

        startBackupButton = new QPushButton(backupConfigurationGroup);
        startBackupButton->setObjectName("startBackupButton");
        startBackupButton->setEnabled(false);

        gridLayout->addWidget(startBackupButton, 4, 1, 1, 1);

        progressLabel = new QLabel(centralwidget);
        progressLabel->setObjectName("progressLabel");
        progressLabel->setGeometry(QRect(90, 330, 37, 12));
        backupProgressBar = new QProgressBar(centralwidget);
        backupProgressBar->setObjectName("backupProgressBar");
        backupProgressBar->setGeometry(QRect(90, 350, 521, 23));
        backupProgressBar->setMaximum(0);
        backupProgressBar->setValue(0);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 900, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Smart Backup Manager", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "Smart Backup Manager", nullptr));
        subtitleLabel->setText(QCoreApplication::translate("MainWindow", "Protect your files and verfiy their intergrity", nullptr));
        backupConfigurationGroup->setTitle(QCoreApplication::translate("MainWindow", "Backup Configuration", nullptr));
        sourceLabel->setText(QCoreApplication::translate("MainWindow", "Source Folder", nullptr));
        sourcePathEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Select the folder you want to protect", nullptr));
        browseSourceButton->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        destinationLabel->setText(QCoreApplication::translate("MainWindow", "Destination Folder", nullptr));
        destinationPathEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "select where backups should be stored", nullptr));
        browseDestinationButton->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        startBackupButton->setText(QCoreApplication::translate("MainWindow", "Start Backup", nullptr));
        progressLabel->setText(QCoreApplication::translate("MainWindow", "Ready", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

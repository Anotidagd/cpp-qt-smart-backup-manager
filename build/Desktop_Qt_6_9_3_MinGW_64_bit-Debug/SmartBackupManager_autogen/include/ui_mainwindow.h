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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *titleLabel;
    QGroupBox *backupConfigurationGroup;
    QLabel *sourceLabel;
    QLineEdit *sourcePathEdit;
    QPushButton *browseSourceButton;
    QLabel *destinationLabel;
    QPushButton *startBackupButton;
    QPushButton *browseDestinationButton;
    QLineEdit *destinationPathEdit;
    QLabel *progressLabel;
    QProgressBar *backupProgressBar;
    QGroupBox *backupHistoryGroup;
    QPushButton *refreshHistoryButton;
    QTableWidget *historyTable;
    QPushButton *openBackupButton;
    QPushButton *restoreBackupButton;
    QPushButton *checkIntegrityButton;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1100, 720);
        MainWindow->setMinimumSize(QSize(1100, 720));
        MainWindow->setStyleSheet(QString::fromUtf8("background-color:rgb(28, 38, 51);\n"
"color: rgb(244, 247, 251);"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        titleLabel = new QLabel(centralwidget);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setGeometry(QRect(390, 10, 441, 41));
        backupConfigurationGroup = new QGroupBox(centralwidget);
        backupConfigurationGroup->setObjectName("backupConfigurationGroup");
        backupConfigurationGroup->setGeometry(QRect(210, 80, 671, 251));
        backupConfigurationGroup->setStyleSheet(QString::fromUtf8("background:#171D26;\n"
"border-color: rgb(41, 52, 69);\n"
"font: 700 9pt \"Caviar Dreams\";\n"
""));
        sourceLabel = new QLabel(backupConfigurationGroup);
        sourceLabel->setObjectName("sourceLabel");
        sourceLabel->setGeometry(QRect(10, 40, 81, 16));
        sourcePathEdit = new QLineEdit(backupConfigurationGroup);
        sourcePathEdit->setObjectName("sourcePathEdit");
        sourcePathEdit->setGeometry(QRect(9, 65, 501, 23));
        sourcePathEdit->setReadOnly(true);
        browseSourceButton = new QPushButton(backupConfigurationGroup);
        browseSourceButton->setObjectName("browseSourceButton");
        browseSourceButton->setGeometry(QRect(543, 65, 80, 24));
        destinationLabel = new QLabel(backupConfigurationGroup);
        destinationLabel->setObjectName("destinationLabel");
        destinationLabel->setGeometry(QRect(9, 110, 105, 16));
        startBackupButton = new QPushButton(backupConfigurationGroup);
        startBackupButton->setObjectName("startBackupButton");
        startBackupButton->setEnabled(false);
        startBackupButton->setGeometry(QRect(448, 200, 181, 24));
        browseDestinationButton = new QPushButton(backupConfigurationGroup);
        browseDestinationButton->setObjectName("browseDestinationButton");
        browseDestinationButton->setGeometry(QRect(540, 140, 80, 24));
        destinationPathEdit = new QLineEdit(backupConfigurationGroup);
        destinationPathEdit->setObjectName("destinationPathEdit");
        destinationPathEdit->setGeometry(QRect(10, 140, 501, 23));
        destinationPathEdit->setReadOnly(true);
        progressLabel = new QLabel(centralwidget);
        progressLabel->setObjectName("progressLabel");
        progressLabel->setGeometry(QRect(210, 340, 281, 16));
        backupProgressBar = new QProgressBar(centralwidget);
        backupProgressBar->setObjectName("backupProgressBar");
        backupProgressBar->setGeometry(QRect(210, 360, 561, 23));
        backupProgressBar->setMaximum(0);
        backupProgressBar->setValue(0);
        backupHistoryGroup = new QGroupBox(centralwidget);
        backupHistoryGroup->setObjectName("backupHistoryGroup");
        backupHistoryGroup->setGeometry(QRect(210, 390, 671, 261));
        backupHistoryGroup->setStyleSheet(QString::fromUtf8("background:#171D26;\n"
"border-color: rgb(41, 52, 69);\n"
"font: 700 9pt \"Caviar Dreams\";"));
        refreshHistoryButton = new QPushButton(backupHistoryGroup);
        refreshHistoryButton->setObjectName("refreshHistoryButton");
        refreshHistoryButton->setGeometry(QRect(490, 20, 121, 21));
        historyTable = new QTableWidget(backupHistoryGroup);
        if (historyTable->columnCount() < 9)
            historyTable->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        historyTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        historyTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        historyTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        historyTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        historyTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        historyTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        historyTable->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        historyTable->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        historyTable->setObjectName("historyTable");
        historyTable->setGeometry(QRect(10, 70, 601, 131));
        historyTable->setStyleSheet(QString::fromUtf8("background:#171D26;\n"
"background:#171D26;\n"
"border-color: rgb(41, 52, 69);\n"
"font: 700 9pt \"Caviar Dreams\";"));
        historyTable->setColumnCount(9);
        openBackupButton = new QPushButton(backupHistoryGroup);
        openBackupButton->setObjectName("openBackupButton");
        openBackupButton->setEnabled(false);
        openBackupButton->setGeometry(QRect(110, 220, 161, 21));
        restoreBackupButton = new QPushButton(backupHistoryGroup);
        restoreBackupButton->setObjectName("restoreBackupButton");
        restoreBackupButton->setEnabled(false);
        restoreBackupButton->setGeometry(QRect(280, 220, 181, 21));
        checkIntegrityButton = new QPushButton(backupHistoryGroup);
        checkIntegrityButton->setObjectName("checkIntegrityButton");
        checkIntegrityButton->setGeometry(QRect(480, 220, 131, 21));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(470, 50, 221, 16));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1100, 21));
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
        backupConfigurationGroup->setTitle(QCoreApplication::translate("MainWindow", "Backup Configuration", nullptr));
        sourceLabel->setText(QCoreApplication::translate("MainWindow", "Source Folder", nullptr));
        sourcePathEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "Select the folder you want to protect", nullptr));
        browseSourceButton->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        destinationLabel->setText(QCoreApplication::translate("MainWindow", "Destination Folder", nullptr));
        startBackupButton->setText(QCoreApplication::translate("MainWindow", "Start Backup", nullptr));
        browseDestinationButton->setText(QCoreApplication::translate("MainWindow", "Browse", nullptr));
        destinationPathEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "select where backups should be stored", nullptr));
        progressLabel->setText(QCoreApplication::translate("MainWindow", "Ready", nullptr));
        backupHistoryGroup->setTitle(QCoreApplication::translate("MainWindow", "Backup History", nullptr));
        refreshHistoryButton->setText(QCoreApplication::translate("MainWindow", "Refresh History", nullptr));
        QTableWidgetItem *___qtablewidgetitem = historyTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = historyTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Status", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = historyTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Source", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = historyTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Destination", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = historyTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "Copied", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = historyTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Verified", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = historyTable->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Failed", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = historyTable->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "Duration", nullptr));
        openBackupButton->setText(QCoreApplication::translate("MainWindow", "Open Backup folder", nullptr));
        restoreBackupButton->setText(QCoreApplication::translate("MainWindow", "Restore Selected Backup", nullptr));
        checkIntegrityButton->setText(QCoreApplication::translate("MainWindow", "Check Integrity", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Backup \342\200\242 Restore \342\200\242 Integrity", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

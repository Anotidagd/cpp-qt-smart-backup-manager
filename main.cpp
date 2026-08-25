#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QCoreApplication::setOrganizationName(
        "AGD Developer"
        );

    QCoreApplication::setApplicationName(
        "SmartBackupManager"
        );

    a.setWindowIcon(
        QIcon(":icons/app.ico")
        );

    QFile styleFile(
        ":/styles/main.qss"
        );

    if (styleFile.open(
            QFile::ReadOnly |
            QFile::Text
            )) {

        a.setStyleSheet(
            QString::fromUtf8(
                styleFile.readAll()
                )
            );

    } else {

        qWarning()
        << "Could not load application stylesheet.";
    }

    MainWindow w;
    w.show();

    return a.exec();
}

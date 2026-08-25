#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName(
        "AGD Developer"
        );

    QCoreApplication::setApplicationName(
        "SmartBackupManager"
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

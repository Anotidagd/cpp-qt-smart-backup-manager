#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "core/backup/BackupService.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QThread;
class BackupWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectSourceFolder();
    void selectDestinationFolder();
    void updateStartButtonState();
    void handleStartBackup();

    void handleBackupProgress(
        int processedFiles,
        int totalFiles,
        const QString &currentFile
        );

    void handleBackupFinished(
        const BackupResult &result
        );

private:
    bool validatePaths(
        QString *errorMessage = nullptr
        ) const;

    Ui::MainWindow *ui;

    QThread *m_backupThread = nullptr;
    BackupWorker *m_backupWorker = nullptr;

    bool m_backupRunning = false;
};

#endif // MAINWINDOW_H

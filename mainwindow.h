#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "core/backup/BackupService.h"
#include "database/DatabaseManager.h"
#include <QDateTime>
#include "core/restore/RestoreService.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QThread;
class BackupWorker;
class RestoreWorker;

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
    void refreshBackupHistory();
    void handleHistorySelection();
    void openSelectedBackupFolder();
    void restoreSelectedBackup();
    void handleRestoreProgress(
        int processedFiles,
        int totalFiles,
        const QString &currentFile
        );

    void handleRestoreFinished(
        const RestoreResult &result
        );

private:
    bool validatePaths(
        QString *errorMessage = nullptr
        ) const;

    Ui::MainWindow *ui;

    QThread *m_backupThread = nullptr;
    BackupWorker *m_backupWorker = nullptr;

    bool m_backupRunning = false;
    DatabaseManager m_databaseManager;

    bool m_databaseAvailable = false;

    QDateTime m_backupStartedAt;
    QString selectedBackupPath() const;
    QThread *m_restoreThread = nullptr;
    RestoreWorker *m_restoreWorker = nullptr;

    bool m_restoreRunning = false;






};

#endif // MAINWINDOW_H

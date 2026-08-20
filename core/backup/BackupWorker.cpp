#include "BackupWorker.h"
#include <QDesktopServices>
#include <QUrl>

BackupWorker::BackupWorker(
    const QString &sourcePath,
    const QString &destinationPath,
    QObject *parent
    )
    : QObject(parent),
    m_sourcePath(sourcePath),
    m_destinationPath(destinationPath)
{
}

void BackupWorker::process()
{
    BackupService backupService;

    const BackupResult result =
        backupService.createBackup(
            m_sourcePath,
            m_destinationPath,

            [this](
                int processedFiles,
                int totalFiles,
                const QString &currentFile
                )
            {
                emit progressChanged(
                    processedFiles,
                    totalFiles,
                    currentFile
                    );
            }
            );

    emit finished(result);
}


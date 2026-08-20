#include "RestoreWorker.h"

RestoreWorker::RestoreWorker(
    const QString &backupPath,
    const QString &destinationPath,
    QObject *parent
    )
    : QObject(parent),
    m_backupPath(backupPath),
    m_destinationPath(destinationPath)
{
}

void RestoreWorker::process()
{
    RestoreService service;

    const RestoreResult result =
        service.restoreBackup(
            m_backupPath,
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

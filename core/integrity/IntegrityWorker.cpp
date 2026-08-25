#include "IntegrityWorker.h"

IntegrityWorker::IntegrityWorker(
    const QString &sourcePath,
    const QString &backupPath,
    QObject *parent
    )
    : QObject(parent),
    m_sourcePath(sourcePath),
    m_backupPath(backupPath)
{
}

void IntegrityWorker::process()
{
    IntegrityService service;

    const IntegrityResult result =
        service.checkIntegrity(
            m_sourcePath,
            m_backupPath,

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

#ifndef BACKUPRECORD_H
#define BACKUPRECORD_H

#include <QDateTime>
#include <QString>

struct BackupRecord
{
    int id = 0;
    QString sourcePath;
    QString destinationPath;
    QString backupPath;

    QString status;

    int copiedFiles = 0;
    int verifiedFiles = 0;

    int failedFiles = 0;
    int integrityFailures = 0;

    int skippedFiles = 0;

    QDateTime startedAt;
    QDateTime completedAt;

    qint64 durationMs = 0;
};

#endif // BACKUPRECORD_H

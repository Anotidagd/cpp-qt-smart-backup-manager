#ifndef BACKUPSERVICE_H
#define BACKUPSERVICE_H

#include <QString>
#include <QStringList>

struct BackupResult
{
    bool success = false;

    QString backupPath;

    int copiedFiles = 0;
    int failedFiles = 0;
    int skippedFiles = 0;

    QStringList errors;
};

class BackupService
{
public:
    BackupService() = default;

    BackupResult createBackup(
        const QString &sourcePath,
        const QString &destinationPath
        );

private:
    bool copyFile(
        const QString &sourceFile,
        const QString &destinationFile,
        QString &errorMessage
        );
};

#endif // BACKUPSERVICE_H

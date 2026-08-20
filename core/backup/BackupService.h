#ifndef BACKUPSERVICE_H
#define BACKUPSERVICE_H

#include <QString>
#include <QStringList>
#include <QMetaType>

#include <functional>

struct BackupResult
{
    bool success = false;

    QString backupPath;

    int copiedFiles = 0;
    int verifiedFiles = 0;

    int failedFiles = 0;
    int integrityFailures = 0;

    int skippedFiles = 0;

    QStringList errors;
};

Q_DECLARE_METATYPE(BackupResult)

class BackupService
{
public:
    using ProgressCallback =
        std::function<void(
            int processedFiles,
            int totalFiles,
            const QString &currentFile
            )>;

    BackupService() = default;

    BackupResult createBackup(
        const QString &sourcePath,
        const QString &destinationPath,
        const ProgressCallback &progressCallback = {}
        );

private:
    bool copyFile(
        const QString &sourceFile,
        const QString &destinationFile,
        QString &errorMessage
        );

    int countFiles(
        const QString &sourcePath
        ) const;
};


#endif // BACKUPSERVICE_H

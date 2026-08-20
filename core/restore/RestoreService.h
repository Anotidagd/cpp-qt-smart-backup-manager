#ifndef RESTORESERVICE_H
#define RESTORESERVICE_H

#include <QString>
#include <QStringList>
#include <QMetaType>

#include <functional>

struct RestoreResult
{
    bool success = false;

    QString restorePath;

    int restoredFiles = 0;
    int verifiedFiles = 0;

    int skippedFiles = 0;
    int failedFiles = 0;

    int integrityFailures = 0;

    QStringList errors;
};

Q_DECLARE_METATYPE(RestoreResult)

class RestoreService
{
public:
    using ProgressCallback =
        std::function<void(
            int processedFiles,
            int totalFiles,
            const QString &currentFile
            )>;

    RestoreResult restoreBackup(
        const QString &backupPath,
        const QString &destinationPath,
        const ProgressCallback &progressCallback = {}
        );

private:
    int countFiles(
        const QString &backupPath
        ) const;
};

#endif // RESTORESERVICE_H

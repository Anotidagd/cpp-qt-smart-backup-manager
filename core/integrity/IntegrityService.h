#ifndef INTEGRITYSERVICE_H
#define INTEGRITYSERVICE_H

#include <QMetaType>
#include <QString>
#include <QStringList>

#include <functional>

struct IntegrityResult
{
    bool success = false;

    int unchangedFiles = 0;
    int modifiedFiles = 0;
    int missingFiles = 0;
    int newFiles = 0;

    int failedFiles = 0;

    QStringList modifiedPaths;
    QStringList missingPaths;
    QStringList newPaths;

    QStringList errors;
};

Q_DECLARE_METATYPE(IntegrityResult)

class IntegrityService
{
public:
    using ProgressCallback =
        std::function<void(
            int processedFiles,
            int totalFiles,
            const QString &currentFile
            )>;

    bool createManifest(
        const QString &backupPath,
        QString *errorMessage = nullptr
        );

    IntegrityResult checkIntegrity(
        const QString &sourcePath,
        const QString &backupPath,
        const ProgressCallback &progressCallback = {}
        );

private:
    int countFiles(
        const QString &directoryPath
        ) const;
};

#endif // INTEGRITYSERVICE_H

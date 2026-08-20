#include "RestoreService.h"

#include "../hashing/HashService.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

RestoreResult RestoreService::restoreBackup(
    const QString &backupPath,
    const QString &destinationPath,
    const ProgressCallback &progressCallback
    )
{
    RestoreResult result;

    QDir backupDirectory(
        backupPath
        );

    if (!backupDirectory.exists()) {

        result.errors.append(
            "Backup directory does not exist."
            );

        return result;
    }

    QDir destinationDirectory(
        destinationPath
        );

    if (!destinationDirectory.exists()) {

        result.errors.append(
            "Restore destination does not exist."
            );

        return result;
    }

    const QString timestamp =
        QDateTime::currentDateTime().toString(
            "yyyy-MM-dd_HH-mm-ss"
            );

    const QString restoreRoot =
        destinationDirectory.filePath(
            "Restored_" + timestamp
            );

    if (!QDir().mkpath(restoreRoot)) {

        result.errors.append(
            "Could not create restore directory."
            );

        return result;
    }

    result.restorePath =
        restoreRoot;

    const int totalFiles =
        countFiles(backupPath);

    int processedFiles = 0;

    if (progressCallback) {

        progressCallback(
            0,
            totalFiles,
            QString()
            );
    }

    HashService hashService;

    QDirIterator iterator(
        backupPath,
        QDir::Files |
            QDir::Dirs |
            QDir::NoDotAndDotDot |
            QDir::Hidden |
            QDir::System,
        QDirIterator::Subdirectories
        );

    while (iterator.hasNext()) {

        iterator.next();

        const QFileInfo fileInfo =
            iterator.fileInfo();

        const QString sourceFile =
            fileInfo.absoluteFilePath();

        const QString relativePath =
            backupDirectory.relativeFilePath(
                sourceFile
                );

        const QString destinationFile =
            QDir(restoreRoot).filePath(
                relativePath
                );

        if (fileInfo.isSymLink()) {

            result.skippedFiles++;

            processedFiles++;

            if (progressCallback) {
                progressCallback(
                    processedFiles,
                    totalFiles,
                    relativePath
                    );
            }

            continue;
        }

        if (fileInfo.isDir()) {

            if (!QDir().mkpath(
                    destinationFile
                    )) {

                result.errors.append(
                    "Could not create directory: "
                    + destinationFile
                    );
            }

            continue;
        }

        if (!fileInfo.isFile()) {
            continue;
        }

        const QFileInfo destinationInfo(
            destinationFile
            );

        if (!QDir().mkpath(
                destinationInfo.dir()
                    .absolutePath()
                )) {

            result.failedFiles++;

            result.errors.append(
                "Could not create restore directory for: "
                + relativePath
                );

            processedFiles++;

            if (progressCallback) {
                progressCallback(
                    processedFiles,
                    totalFiles,
                    relativePath
                    );
            }

            continue;
        }

        // Safe default: do not overwrite.
        if (QFile::exists(
                destinationFile
                )) {

            result.skippedFiles++;

            processedFiles++;

            if (progressCallback) {
                progressCallback(
                    processedFiles,
                    totalFiles,
                    relativePath
                    );
            }

            continue;
        }

        if (!QFile::copy(
                sourceFile,
                destinationFile
                )) {

            result.failedFiles++;

            result.errors.append(
                "Could not restore file: "
                + relativePath
                );

            processedFiles++;

            if (progressCallback) {
                progressCallback(
                    processedFiles,
                    totalFiles,
                    relativePath
                    );
            }

            continue;
        }

        result.restoredFiles++;

        QString verificationError;

        if (hashService.verifyFiles(
                sourceFile,
                destinationFile,
                verificationError
                )) {

            result.verifiedFiles++;

        } else {

            result.integrityFailures++;

            result.errors.append(
                verificationError
                );
        }

        processedFiles++;

        if (progressCallback) {

            progressCallback(
                processedFiles,
                totalFiles,
                relativePath
                );
        }
        if (fileInfo.fileName() ==
            ".integrity_manifest.json") {

            continue;
        }
    }

    result.success =
        result.failedFiles == 0 &&
        result.integrityFailures == 0;

    return result;
}


int RestoreService::countFiles(
    const QString &backupPath
    ) const
{
    int totalFiles = 0;

    QDirIterator iterator(
        backupPath,
        QDir::Files |
            QDir::Hidden |
            QDir::System,
        QDirIterator::Subdirectories
        );

    while (iterator.hasNext()) {

        iterator.next();

        totalFiles++;
    }

    return totalFiles;
}

#include "BackupService.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include "../hashing/HashService.h"

BackupResult BackupService::createBackup(
    const QString &sourcePath,
    const QString &destinationPath,
    const ProgressCallback &progressCallback
    )
{
    BackupResult result;

    QDir sourceDirectory(sourcePath);

    if (!sourceDirectory.exists()) {
        result.errors.append(
            "Source directory does not exist."
            );

        return result;
    }

    QDir destinationDirectory(destinationPath);

    if (!destinationDirectory.exists()) {
        result.errors.append(
            "Destination directory does not exist."
            );

        return result;
    }

    // Example:
    // Backup_2026-08-08_09-30-42
    const QString timestamp =
        QDateTime::currentDateTime().toString(
            "yyyy-MM-dd_HH-mm-ss"
            );

    const QString backupFolderName =
        "Backup_" + timestamp;

    const QString backupRoot =
        destinationDirectory.filePath(
            backupFolderName
            );

    if (!QDir().mkpath(backupRoot)) {
        result.errors.append(
            "Failed to create backup directory."
            );

        return result;
    }

    result.backupPath = backupRoot;

    QDirIterator iterator(
        sourcePath,
        QDir::Files |
            QDir::Dirs |
            QDir::NoDotAndDotDot |
            QDir::Hidden |
            QDir::System,
        QDirIterator::Subdirectories
        );

    const int totalFiles =
        countFiles(sourcePath);

    int processedFiles = 0;

    if (progressCallback) {

        progressCallback(
            0,
            totalFiles,
            QString()
            );
    }

    while (iterator.hasNext()) {

        iterator.next();

        const QFileInfo fileInfo =
            iterator.fileInfo();

        const QString absoluteSourcePath =
            fileInfo.absoluteFilePath();

        const QString relativePath =
            sourceDirectory.relativeFilePath(
                absoluteSourcePath
                );

        const QString targetPath =
            QDir(backupRoot).filePath(
                relativePath
                );

        // Directory
        if (fileInfo.isDir()) {

            if (!QDir().mkpath(targetPath)) {

                result.errors.append(
                    "Failed to create directory: "
                    + targetPath
                    );
            }

            continue;
        }

        // Skip symbolic links for now
        if (fileInfo.isSymLink()) {
            result.skippedFiles++;
            continue;
        }

        // File
        if (fileInfo.isFile()) {

            const QFileInfo targetInfo(
                targetPath
                );

            QDir parentDirectory =
                targetInfo.dir();

            if (!parentDirectory.exists()) {

                if (!QDir().mkpath(
                        parentDirectory.absolutePath()
                        )) {

                    result.failedFiles++;

                    result.errors.append(
                        "Failed to create directory: "
                        + parentDirectory.absolutePath()
                        );

                    continue;
                }
            }

            QString errorMessage;

            if (!copyFile(
                    absoluteSourcePath,
                    targetPath,
                    errorMessage
                    )) {

                result.failedFiles++;

                result.errors.append(
                    errorMessage
                    );

                continue;
            }

            result.copiedFiles++;

            HashService hashService;

            QString verificationError;

            if (hashService.verifyFiles(
                    absoluteSourcePath,
                    targetPath,
                    verificationError
                    )) {

                result.verifiedFiles++;

            } else {

                result.integrityFailures++;

                result.errors.append(
                    verificationError
                    );
            }
        }
    }

    result.success =
        result.failedFiles == 0 &&
        result.integrityFailures == 0;

    return result;
}

bool BackupService::copyFile(
    const QString &sourceFile,
    const QString &destinationFile,
    QString &errorMessage
    )
{
    if (QFile::exists(destinationFile)) {

        if (!QFile::remove(destinationFile)) {

            errorMessage =
                "Could not replace existing file: "
                + destinationFile;

            return false;
        }
    }

    if (!QFile::copy(
            sourceFile,
            destinationFile
            )) {

        errorMessage =
            "Failed to copy: "
            + sourceFile;

        return false;
    }

    return true;
}

int BackupService::countFiles(
    const QString &sourcePath
    ) const
{
    int totalFiles = 0;

    QDirIterator iterator(
        sourcePath,
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

        if (fileInfo.isFile() ||
            fileInfo.isSymLink()) {

            totalFiles++;
        }
    }

    return totalFiles;
}

#include "IntegrityService.h"

#include "../hashing/HashService.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

namespace
{

const QString manifestFileName =
    ".integrity_manifest.json";

QString normalizeRelativePath(
    const QString &path
    )
{
    return QDir::fromNativeSeparators(
        path
        );
}

} // namespace


bool IntegrityService::createManifest(
    const QString &backupPath,
    QString *errorMessage
    )
{
    QDir backupDirectory(backupPath);

    if (!backupDirectory.exists()) {

        if (errorMessage != nullptr) {
            *errorMessage =
                "Backup directory does not exist.";
        }

        return false;
    }

    HashService hashService;

    QJsonArray filesArray;

    QDirIterator iterator(
        backupPath,
        QDir::Files |
            QDir::Hidden |
            QDir::System,
        QDirIterator::Subdirectories
        );

    while (iterator.hasNext()) {

        iterator.next();

        const QFileInfo fileInfo =
            iterator.fileInfo();

        if (fileInfo.fileName() ==
            manifestFileName) {

            continue;
        }

        if (fileInfo.isSymLink()) {
            continue;
        }

        const HashResult hashResult =
            hashService.calculateSha256(
                fileInfo.absoluteFilePath()
                );

        if (!hashResult.success) {

            if (errorMessage != nullptr) {
                *errorMessage =
                    hashResult.error;
            }

            return false;
        }

        const QString relativePath =
            normalizeRelativePath(
                backupDirectory.relativeFilePath(
                    fileInfo.absoluteFilePath()
                    )
                );

        QJsonObject fileObject;

        fileObject["path"] =
            relativePath;

        fileObject["sha256"] =
            QString::fromLatin1(
                hashResult.hash.toHex()
                );

        filesArray.append(
            fileObject
            );
    }

    QJsonObject rootObject;

    rootObject["version"] = 1;

    rootObject["createdAt"] =
        QDateTime::currentDateTimeUtc()
            .toString(Qt::ISODateWithMs);

    rootObject["files"] =
        filesArray;

    const QString manifestPath =
        backupDirectory.filePath(
            manifestFileName
            );

    QSaveFile manifestFile(
        manifestPath
        );

    if (!manifestFile.open(
            QIODevice::WriteOnly
            )) {

        if (errorMessage != nullptr) {
            *errorMessage =
                "Could not create integrity manifest.";
        }

        return false;
    }

    const QJsonDocument document(
        rootObject
        );

    if (manifestFile.write(
            document.toJson(
                QJsonDocument::Indented
                )
            ) == -1) {

        if (errorMessage != nullptr) {
            *errorMessage =
                "Could not write integrity manifest.";
        }

        return false;
    }

    if (!manifestFile.commit()) {

        if (errorMessage != nullptr) {
            *errorMessage =
                "Could not save integrity manifest.";
        }

        return false;
    }

    return true;
}


IntegrityResult IntegrityService::checkIntegrity(
    const QString &sourcePath,
    const QString &backupPath,
    const ProgressCallback &progressCallback
    )
{
    IntegrityResult result;

    QDir sourceDirectory(
        sourcePath
        );

    QDir backupDirectory(
        backupPath
        );

    if (!sourceDirectory.exists()) {

        result.errors.append(
            "Original source directory no longer exists."
            );

        return result;
    }

    const QString manifestPath =
        backupDirectory.filePath(
            manifestFileName
            );

    QFile manifestFile(
        manifestPath
        );

    if (!manifestFile.open(
            QIODevice::ReadOnly
            )) {

        result.errors.append(
            "Integrity manifest could not be opened."
            );

        return result;
    }

    const QJsonDocument document =
        QJsonDocument::fromJson(
            manifestFile.readAll()
            );

    if (!document.isObject()) {

        result.errors.append(
            "Integrity manifest is invalid."
            );

        return result;
    }

    const QJsonArray filesArray =
        document.object()
            .value("files")
            .toArray();

    QHash<QString, QByteArray>
        baselineHashes;

    for (const QJsonValue &value :
         filesArray) {

        const QJsonObject object =
            value.toObject();

        const QString path =
            object.value("path")
                .toString();

        const QByteArray hash =
            QByteArray::fromHex(
                object.value("sha256")
                    .toString()
                    .toLatin1()
                );

        baselineHashes.insert(
            path,
            hash
            );
    }

    QHash<QString, QByteArray>
        currentHashes;

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

    HashService hashService;

    QDirIterator iterator(
        sourcePath,
        QDir::Files |
            QDir::Hidden |
            QDir::System,
        QDirIterator::Subdirectories
        );

    while (iterator.hasNext()) {

        iterator.next();

        const QFileInfo fileInfo =
            iterator.fileInfo();

        if (fileInfo.isSymLink()) {
            continue;
        }

        const QString relativePath =
            normalizeRelativePath(
                sourceDirectory.relativeFilePath(
                    fileInfo.absoluteFilePath()
                    )
                );

        const HashResult hashResult =
            hashService.calculateSha256(
                fileInfo.absoluteFilePath()
                );

        if (!hashResult.success) {

            result.failedFiles++;

            result.errors.append(
                hashResult.error
                );

        } else {

            currentHashes.insert(
                relativePath,
                hashResult.hash
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
    }

    // Check files that existed
    // when the backup was created.
    for (auto iterator =
         baselineHashes.constBegin();
         iterator != baselineHashes.constEnd();
         ++iterator) {

        const QString &path =
            iterator.key();

        if (!currentHashes.contains(path)) {

            result.missingFiles++;

            result.missingPaths.append(
                path
                );

            continue;
        }

        if (currentHashes.value(path) ==
            iterator.value()) {

            result.unchangedFiles++;

        } else {

            result.modifiedFiles++;

            result.modifiedPaths.append(
                path
                );
        }
    }

    // Detect files that did not
    // exist in the baseline.
    for (auto iterator =
         currentHashes.constBegin();
         iterator != currentHashes.constEnd();
         ++iterator) {

        if (!baselineHashes.contains(
                iterator.key()
                )) {

            result.newFiles++;

            result.newPaths.append(
                iterator.key()
                );
        }
    }

    // Changed files do NOT mean
    // the scan itself failed.
    result.success =
        result.failedFiles == 0;

    return result;
}


int IntegrityService::countFiles(
    const QString &directoryPath
    ) const
{
    int totalFiles = 0;

    QDirIterator iterator(
        directoryPath,
        QDir::Files |
            QDir::Hidden |
            QDir::System,
        QDirIterator::Subdirectories
        );

    while (iterator.hasNext()) {

        iterator.next();

        if (!iterator.fileInfo().isSymLink()) {
            totalFiles++;
        }
    }

    return totalFiles;
}

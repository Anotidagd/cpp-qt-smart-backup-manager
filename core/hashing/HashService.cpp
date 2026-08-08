#include "HashService.h"

#include <QCryptographicHash>
#include <QFile>

HashResult HashService::calculateSha256(
    const QString &filePath
    ) const
{
    HashResult result;

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {

        result.error =
            "Unable to open file for hashing: "
            + filePath;

        return result;
    }

    QCryptographicHash hash(
        QCryptographicHash::Sha256
        );

    constexpr qint64 bufferSize =
        1024 * 1024; // 1 MB

    while (!file.atEnd()) {

        const QByteArray data =
            file.read(bufferSize);

        if (data.isEmpty() &&
            file.error() != QFile::NoError) {

            result.error =
                "Error while reading file: "
                + filePath;

            return result;
        }

        hash.addData(data);
    }

    result.hash = hash.result();

    result.success = true;

    return result;
}

bool HashService::verifyFiles(
    const QString &sourceFile,
    const QString &backupFile,
    QString &errorMessage
    ) const
{
    const HashResult sourceHash =
        calculateSha256(sourceFile);

    if (!sourceHash.success) {

        errorMessage =
            sourceHash.error;

        return false;
    }

    const HashResult backupHash =
        calculateSha256(backupFile);

    if (!backupHash.success) {

        errorMessage =
            backupHash.error;

        return false;
    }

    if (sourceHash.hash != backupHash.hash) {

        errorMessage =
            "Integrity verification failed: "
            + sourceFile;

        return false;
    }

    return true;
}

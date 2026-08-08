#ifndef HASHSERVICE_H
#define HASHSERVICE_H

#include <QByteArray>
#include <QString>

struct HashResult
{
    bool success = false;
    QByteArray hash;
    QString error;
};

class HashService
{
public:
    HashService() = default;

    HashResult calculateSha256(
        const QString &filePath
        ) const;

    bool verifyFiles(
        const QString &sourceFile,
        const QString &backupFile,
        QString &errorMessage
        ) const;
};

#endif // HASHSERVICE_H

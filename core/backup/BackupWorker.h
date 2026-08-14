#ifndef BACKUPWORKER_H
#define BACKUPWORKER_H

#include "BackupService.h"

#include <QObject>
#include <QString>

class BackupWorker : public QObject
{
    Q_OBJECT

public:
    explicit BackupWorker(
        const QString &sourcePath,
        const QString &destinationPath,
        QObject *parent = nullptr
        );

public slots:
    void process();

signals:
    void progressChanged(
        int processedFiles,
        int totalFiles,
        const QString &currentFile
        );

    void finished(
        const BackupResult &result
        );

private:
    QString m_sourcePath;
    QString m_destinationPath;
};

#endif // BACKUPWORKER_H

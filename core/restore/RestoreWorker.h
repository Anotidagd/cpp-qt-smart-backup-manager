#ifndef RESTOREWORKER_H
#define RESTOREWORKER_H

#include "RestoreService.h"

#include <QObject>
#include <QString>

class RestoreWorker : public QObject
{
    Q_OBJECT

public:
    explicit RestoreWorker(
        const QString &backupPath,
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
        const RestoreResult &result
        );

private:
    QString m_backupPath;
    QString m_destinationPath;
};

#endif

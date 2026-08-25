#ifndef INTEGRITYWORKER_H
#define INTEGRITYWORKER_H

#include "IntegrityService.h"

#include <QObject>
#include <QString>

class IntegrityWorker : public QObject
{
    Q_OBJECT

public:
    explicit IntegrityWorker(
        const QString &sourcePath,
        const QString &backupPath,
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
        const IntegrityResult &result
        );

private:
    QString m_sourcePath;
    QString m_backupPath;
};

#endif // INTEGRITYWORKER_H

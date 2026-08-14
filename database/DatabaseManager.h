#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "../models/BackupRecord.h"

#include <QSqlDatabase>
#include <QString>
#include <QList>

class DatabaseManager
{
public:
    DatabaseManager();

    bool initialize(
        QString *errorMessage = nullptr
        );

    bool insertBackupRecord(
        const BackupRecord &record,
        QString *errorMessage = nullptr
        );

    QString databasePath() const;
    QList<BackupRecord> getBackupHistory(
        QString *errorMessage = nullptr
        );

private:
    bool createTables(
        QString *errorMessage
        );

    QSqlDatabase m_database;

    QString m_databasePath;
};

#endif // DATABASEMANAGER_H

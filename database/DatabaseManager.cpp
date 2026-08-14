#include "DatabaseManager.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>

DatabaseManager::DatabaseManager()
{
}

bool DatabaseManager::initialize(
    QString *errorMessage
    )
{
    const QString appDataPath =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
            );

    if (appDataPath.isEmpty()) {

        if (errorMessage != nullptr) {
            *errorMessage =
                "Could not determine the application data directory.";
        }

        return false;
    }

    QDir directory;

    if (!directory.mkpath(appDataPath)) {

        if (errorMessage != nullptr) {
            *errorMessage =
                "Could not create the application data directory.";
        }

        return false;
    }

    m_databasePath =
        QDir(appDataPath).filePath(
            "smart_backup_manager.db"
            );

    m_database =
        QSqlDatabase::addDatabase(
            "QSQLITE",
            "smartbackup_main_connection"
            );

    m_database.setDatabaseName(
        m_databasePath
        );

    if (!m_database.open()) {

        if (errorMessage != nullptr) {

            *errorMessage =
                "Could not open SQLite database: "
                + m_database.lastError().text();
        }

        return false;
    }

    return createTables(
        errorMessage
        );
}


bool DatabaseManager::createTables(
    QString *errorMessage
    )
{
    QSqlQuery query(m_database);

    const QString createBackupHistoryTable =
        R"(
            CREATE TABLE IF NOT EXISTS backup_history
            (
                id INTEGER PRIMARY KEY AUTOINCREMENT,

                source_path TEXT NOT NULL,
                destination_path TEXT NOT NULL,
                backup_path TEXT,

                status TEXT NOT NULL,

                copied_files INTEGER NOT NULL DEFAULT 0,
                verified_files INTEGER NOT NULL DEFAULT 0,

                failed_files INTEGER NOT NULL DEFAULT 0,
                integrity_failures INTEGER NOT NULL DEFAULT 0,

                skipped_files INTEGER NOT NULL DEFAULT 0,

                started_at TEXT NOT NULL,
                completed_at TEXT NOT NULL,

                duration_ms INTEGER NOT NULL DEFAULT 0
            )
        )";

    if (!query.exec(
            createBackupHistoryTable
            )) {

        if (errorMessage != nullptr) {

            *errorMessage =
                "Could not create backup history table: "
                + query.lastError().text();
        }

        return false;
    }

    return true;
}


bool DatabaseManager::insertBackupRecord(
    const BackupRecord &record,
    QString *errorMessage
    )
{
    if (!m_database.isOpen()) {

        if (errorMessage != nullptr) {
            *errorMessage =
                "The backup history database is not open.";
        }

        return false;
    }

    QSqlQuery query(m_database);

    query.prepare(
        R"(
            INSERT INTO backup_history
            (
                source_path,
                destination_path,
                backup_path,
                status,

                copied_files,
                verified_files,
                failed_files,
                integrity_failures,
                skipped_files,

                started_at,
                completed_at,
                duration_ms
            )
            VALUES
            (
                :source_path,
                :destination_path,
                :backup_path,
                :status,

                :copied_files,
                :verified_files,
                :failed_files,
                :integrity_failures,
                :skipped_files,

                :started_at,
                :completed_at,
                :duration_ms
            )
        )"
        );

    query.bindValue(
        ":source_path",
        record.sourcePath
        );

    query.bindValue(
        ":destination_path",
        record.destinationPath
        );

    query.bindValue(
        ":backup_path",
        record.backupPath
        );

    query.bindValue(
        ":status",
        record.status
        );

    query.bindValue(
        ":copied_files",
        record.copiedFiles
        );

    query.bindValue(
        ":verified_files",
        record.verifiedFiles
        );

    query.bindValue(
        ":failed_files",
        record.failedFiles
        );

    query.bindValue(
        ":integrity_failures",
        record.integrityFailures
        );

    query.bindValue(
        ":skipped_files",
        record.skippedFiles
        );

    query.bindValue(
        ":started_at",
        record.startedAt.toString(
            Qt::ISODateWithMs
            )
        );

    query.bindValue(
        ":completed_at",
        record.completedAt.toString(
            Qt::ISODateWithMs
            )
        );

    query.bindValue(
        ":duration_ms",
        record.durationMs
        );

    if (!query.exec()) {

        if (errorMessage != nullptr) {

            *errorMessage =
                "Could not save backup history: "
                + query.lastError().text();
        }

        return false;
    }

    return true;
}


QString DatabaseManager::databasePath() const
{
    return m_databasePath;
}

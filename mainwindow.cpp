#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core/backup/BackupService.h"
#include "core/backup/BackupWorker.h"
#include "core/restore/RestoreWorker.h"
#include "core/integrity/IntegrityWorker.h"
#include <QDesktopServices>
#include <QUrl>


#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QThread>
#include <QDebug>
#include <QHeaderView>
#include <QTableWidget>
#include <QTableWidgetItem>

namespace
{

#ifdef Q_OS_WIN
constexpr Qt::CaseSensitivity pathCaseSensitivity = Qt::CaseInsensitive;
#else
constexpr Qt::CaseSensitivity pathCaseSensitivity = Qt::CaseSensitive;
#endif

QString normalizePath(const QString &path)
{
    if (path.trimmed().isEmpty()) {
        return {};
    }

    return QDir::cleanPath(
        QDir(path).absolutePath()
        );
}

bool pathsAreEqual(
    const QString &firstPath,
    const QString &secondPath
    )
{
    return normalizePath(firstPath).compare(
               normalizePath(secondPath),
               pathCaseSensitivity
               ) == 0;
}

bool isSameOrChildDirectory(
    const QString &childPath,
    const QString &parentPath
    )
{
    const QString child =
        normalizePath(childPath);

    QString parent =
        normalizePath(parentPath);

    if (child.isEmpty() ||
        parent.isEmpty()) {

        return false;
    }

    if (child.compare(
            parent,
            pathCaseSensitivity
            ) == 0) {

        return true;
    }

    parent += QDir::separator();

    return child.startsWith(
        parent,
        pathCaseSensitivity
        );
}


} // namespace

//the constructor is here , remebmer that..
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QString databaseError;

    m_databaseAvailable =
        m_databaseManager.initialize(
            &databaseError
            );

    if (!m_databaseAvailable) {

        statusBar()->showMessage(
            "Backup history database unavailable."
            );

        qWarning()
            << databaseError;
    }
    qRegisterMetaType<BackupResult>(
        "BackupResult"
        );

    qRegisterMetaType<RestoreResult>(
        "RestoreResult"
        );

    qRegisterMetaType<IntegrityResult>(
        "IntegrityResult"
        );

    ///////
    ui->openBackupButton->setEnabled(false);
    ui->restoreBackupButton->setEnabled(false);
    ui->checkIntegrityButton->setEnabled(false);
    ui->historyTable->setColumnCount(8);

    ui->historyTable->setHorizontalHeaderLabels(
        {
            "Date",
            "Status",
            "Source",
            "Destination",
            "Copied",
            "Verified",
            "Failed",
            "Duration"
        }
        );

    ui->historyTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    ui->historyTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    ui->historyTable->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    ui->historyTable->setAlternatingRowColors(
        true
        );

    ui->historyTable->verticalHeader()->setVisible(
        false
        );

    QHeaderView *header =
        ui->historyTable->horizontalHeader();

    header->setSectionResizeMode(
        QHeaderView::ResizeToContents
        );

    header->setSectionResizeMode(
        2,
        QHeaderView::Stretch
        );

    header->setSectionResizeMode(
        3,
        QHeaderView::Stretch
        );

    connect(
        ui->refreshHistoryButton,
        &QPushButton::clicked,
        this,
        &MainWindow::refreshBackupHistory
        );

    connect(
        ui->historyTable,
        &QTableWidget::itemSelectionChanged,
        this,
        &MainWindow::handleHistorySelection
        );

    connect(
        ui->openBackupButton,
        &QPushButton::clicked,
        this,
        &MainWindow::openSelectedBackupFolder
        );

    connect(
        ui->restoreBackupButton,
        &QPushButton::clicked,
        this,
        &MainWindow::restoreSelectedBackup
        );
    ///////////////


    // Required because BackupResult travels
    // between worker and GUI threads.


    ui->sourcePathEdit->setReadOnly(true);
    ui->destinationPathEdit->setReadOnly(true);

    ui->startBackupButton->setEnabled(false);

    ui->backupProgressBar->setRange(0, 100);
    ui->backupProgressBar->setValue(0);

    ui->progressLabel->setText(
        "Ready"
        );
    connect(
        ui->checkIntegrityButton,
        &QPushButton::clicked,
        this,
        &MainWindow::checkSelectedBackupIntegrity
        );
    connect(
        ui->browseSourceButton,
        &QPushButton::clicked,
        this,
        &MainWindow::selectSourceFolder
        );

    connect(
        ui->browseDestinationButton,
        &QPushButton::clicked,
        this,
        &MainWindow::selectDestinationFolder
        );

    connect(
        ui->sourcePathEdit,
        &QLineEdit::textChanged,
        this,
        &MainWindow::updateStartButtonState
        );

    connect(
        ui->destinationPathEdit,
        &QLineEdit::textChanged,
        this,
        &MainWindow::updateStartButtonState
        );

    connect(
        ui->startBackupButton,
        &QPushButton::clicked,
        this,
        &MainWindow::handleStartBackup
        );

    statusBar()->showMessage(
        "Select a source folder and backup destination."
        );
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::selectSourceFolder()
{
    const QString initialDirectory =
        ui->sourcePathEdit->text().isEmpty()
            ? QDir::homePath()
            : ui->sourcePathEdit->text();

    const QString selectedDirectory =
        QFileDialog::getExistingDirectory(
            this,
            tr("Select Source Folder"),
            initialDirectory,
            QFileDialog::ShowDirsOnly |
                QFileDialog::DontResolveSymlinks
            );

    if (selectedDirectory.isEmpty()) {
        return;
    }

    ui->sourcePathEdit->setText(
        normalizePath(selectedDirectory)
        );
}


void MainWindow::selectDestinationFolder()
{
    QString initialDirectory =
        ui->destinationPathEdit->text();

    if (initialDirectory.isEmpty()) {
        initialDirectory = QDir::homePath();
    }

    const QString selectedDirectory =
        QFileDialog::getExistingDirectory(
            this,
            tr("Select Backup Destination"),
            initialDirectory,
            QFileDialog::ShowDirsOnly |
                QFileDialog::DontResolveSymlinks
            );

    if (selectedDirectory.isEmpty()) {
        return;
    }

    ui->destinationPathEdit->setText(
        normalizePath(selectedDirectory)
        );
}


void MainWindow::updateStartButtonState()
{
    QString errorMessage;

    const bool pathsAreValid =
        validatePaths(&errorMessage);

    // Important:
    // Never enable Start while backup is running.
    const bool operationRunning =
        m_backupRunning ||
        m_restoreRunning ||
        m_integrityRunning;

    ui->startBackupButton->setEnabled(
        pathsAreValid &&
        !operationRunning
        );

    if (pathsAreValid) {

        if (!m_backupRunning) {
            statusBar()->showMessage(
                "Backup configuration is valid."
                );
        }

    } else if (
        !ui->sourcePathEdit->text().isEmpty() &&
        !ui->destinationPathEdit->text().isEmpty()
        ) {

        statusBar()->showMessage(
            errorMessage
            );

    } else {

        statusBar()->showMessage(
            "Select a source folder and backup destination."
            );
    }
}


bool MainWindow::validatePaths(
    QString *errorMessage
    ) const
{
    const QString sourcePath =
        normalizePath(
            ui->sourcePathEdit->text()
            );

    const QString destinationPath =
        normalizePath(
            ui->destinationPathEdit->text()
            );

    auto setError =
        [errorMessage](
            const QString &message
            )
    {
        if (errorMessage != nullptr) {
            *errorMessage = message;
        }
    };

    if (sourcePath.isEmpty()) {

        setError(
            "Please select a source folder."
            );

        return false;
    }

    if (destinationPath.isEmpty()) {

        setError(
            "Please select a backup destination."
            );

        return false;
    }

    if (!QFileInfo(sourcePath).isDir()) {

        setError(
            "The selected source folder does not exist."
            );

        return false;
    }

    if (!QFileInfo(destinationPath).isDir()) {

        setError(
            "The backup destination does not exist."
            );

        return false;
    }

    if (pathsAreEqual(
            sourcePath,
            destinationPath
            )) {

        setError(
            "The source and destination cannot be the same folder."
            );

        return false;
    }

    if (isSameOrChildDirectory(
            destinationPath,
            sourcePath
            )) {

        setError(
            "The backup destination cannot be inside the source folder."
            );

        return false;
    }

    return true;
}


void MainWindow::handleStartBackup()
{
    QString errorMessage;

    if (!validatePaths(&errorMessage)) {

        QMessageBox::warning(
            this,
            tr("Invalid Backup Configuration"),
            errorMessage
            );

        return;
    }

    // Prevent double-starting.
    if (m_backupRunning ||
        m_restoreRunning ||
        m_integrityRunning) {

        return;
    }

    const QString sourcePath =
        ui->sourcePathEdit->text();

    const QString destinationPath =
        ui->destinationPathEdit->text();

    m_backupRunning = true;

    m_backupStartedAt =
        QDateTime::currentDateTimeUtc();

    // Lock configuration controls.
    ui->startBackupButton->setEnabled(false);

    ui->browseSourceButton->setEnabled(false);

    ui->browseDestinationButton->setEnabled(false);

    // Indeterminate progress while source
    // is initially being scanned.
    ui->backupProgressBar->setRange(
        0,
        0
        );

    ui->progressLabel->setText(
        "Scanning source files..."
        );

    statusBar()->showMessage(
        "Preparing backup..."
        );

    // Create thread.
    m_backupThread =
        new QThread(this);

    // Create worker.
    m_backupWorker =
        new BackupWorker(
            sourcePath,
            destinationPath
            );

    // Worker now belongs to background thread.
    m_backupWorker->moveToThread(
        m_backupThread
        );

    // When thread starts,
    // begin the backup.
    connect(
        m_backupThread,
        &QThread::started,
        m_backupWorker,
        &BackupWorker::process
        );

    // Worker reports progress to GUI.
    connect(
        m_backupWorker,
        &BackupWorker::progressChanged,
        this,
        &MainWindow::handleBackupProgress
        );

    // Worker reports final result.
    connect(
        m_backupWorker,
        &BackupWorker::finished,
        this,
        &MainWindow::handleBackupFinished
        );

    // Stop thread when backup ends.
    connect(
        m_backupWorker,
        &BackupWorker::finished,
        m_backupThread,
        &QThread::quit
        );

    // Clean worker.
    connect(
        m_backupWorker,
        &BackupWorker::finished,
        m_backupWorker,
        &QObject::deleteLater
        );

    // Clean thread.
    connect(
        m_backupThread,
        &QThread::finished,
        m_backupThread,
        &QObject::deleteLater
        );

    // Reset our pointers after thread stops.
    connect(
        m_backupThread,
        &QThread::finished,
        this,
        [this]()
        {
            m_backupThread = nullptr;
            m_backupWorker = nullptr;
        }
        );

    // Start background thread.
    m_backupThread->start();
}


void MainWindow::handleBackupProgress(
    int processedFiles,
    int totalFiles,
    const QString &currentFile
    )
{
    if (totalFiles <= 0) {

        ui->backupProgressBar->setRange(
            0,
            0
            );

        ui->progressLabel->setText(
            "Preparing backup..."
            );

        return;
    }

    // Switch from indeterminate
    // to actual progress.
    ui->backupProgressBar->setRange(
        0,
        totalFiles
        );

    ui->backupProgressBar->setValue(
        processedFiles
        );

    if (currentFile.isEmpty()) {

        ui->progressLabel->setText(
            QString("%1 / %2 files")
                .arg(processedFiles)
                .arg(totalFiles)
            );

        return;
    }

    ui->progressLabel->setText(
        QString(
            "%1 / %2 files — %3"
            )
            .arg(processedFiles)
            .arg(totalFiles)
            .arg(currentFile)
        );

    statusBar()->showMessage(
        QString(
            "Backing up %1"
            )
            .arg(currentFile)
        );
}


void MainWindow::handleBackupFinished(
    const BackupResult &result
    )
{
    m_backupRunning = false;
    const QDateTime completedAt =
        QDateTime::currentDateTimeUtc();

    if (m_databaseAvailable) {

        BackupRecord record;

        record.sourcePath =
            ui->sourcePathEdit->text();

        record.destinationPath =
            ui->destinationPathEdit->text();

        record.backupPath =
            result.backupPath;

        record.status =
            result.success
                ? "success"
                : "failed";

        record.copiedFiles =
            result.copiedFiles;

        record.verifiedFiles =
            result.verifiedFiles;

        record.failedFiles =
            result.failedFiles;

        record.integrityFailures =
            result.integrityFailures;

        record.skippedFiles =
            result.skippedFiles;

        record.startedAt =
            m_backupStartedAt;

        record.completedAt =
            completedAt;

        record.durationMs =
            m_backupStartedAt.msecsTo(
                completedAt
                );

        QString databaseError;

        if (!m_databaseManager.insertBackupRecord(
                record,
                &databaseError
                )) {

            qWarning()
            << databaseError;

        } else {

            refreshBackupHistory();
        }
    }

    // Unlock controls.
    ui->browseSourceButton->setEnabled(true);

    ui->browseDestinationButton->setEnabled(true);

    // Start button gets enabled only
    // if paths remain valid.
    updateStartButtonState();

    if (ui->backupProgressBar->maximum() > 0) {

        ui->backupProgressBar->setValue(
            ui->backupProgressBar->maximum()
            );
    }

    if (result.success) {

        ui->progressLabel->setText(
            "Backup completed successfully."
            );

        statusBar()->showMessage(
            "Backup completed successfully."
            );

        QMessageBox::information(
            this,
            tr("Backup Complete"),
            tr(
                "Backup completed successfully.\n\n"
                "Files copied: %1\n"
                "Files verified: %2\n"
                "Files skipped: %3\n\n"
                "Backup location:\n%4"
                )
                .arg(result.copiedFiles)
                .arg(result.verifiedFiles)
                .arg(result.skippedFiles)
                .arg(result.backupPath)
            );

        return;
    }

    ui->progressLabel->setText(
        "Backup completed with errors."
        );

    statusBar()->showMessage(
        "Backup completed with errors."
        );

    QString details;

    if (!result.errors.isEmpty()) {

        details =
            "\n\nErrors:\n"
            + result.errors.join("\n");
    }

    QMessageBox::warning(
        this,
        tr("Backup Completed With Errors"),
        tr(
            "The backup completed with errors.\n\n"
            "Copied: %1\n"
            "Verified: %2\n"
            "Copy failures: %3\n"
            "Integrity failures: %4\n"
            "Skipped: %5"
            )
                .arg(result.copiedFiles)
                .arg(result.verifiedFiles)
                .arg(result.failedFiles)
                .arg(result.integrityFailures)
                .arg(result.skippedFiles)
            + details
        );
    qDebug()
        << "Database:"
        << m_databaseManager.databasePath();
    if (m_databaseAvailable) {
        refreshBackupHistory();
    }
}
void MainWindow::refreshBackupHistory()
{
    if (!m_databaseAvailable) {

        statusBar()->showMessage(
            "Backup history database is unavailable."
            );

        return;
    }

    QString errorMessage;

    const QList<BackupRecord> records =
        m_databaseManager.getBackupHistory(
            &errorMessage
            );

    if (!errorMessage.isEmpty()) {

        statusBar()->showMessage(
            "Unable to load backup history."
            );

        qWarning()
            << errorMessage;

        return;
    }

    ui->historyTable->setRowCount(0);

    for (const BackupRecord &record : records) {

        const int row =
            ui->historyTable->rowCount();

        ui->historyTable->insertRow(row);

        // Convert stored UTC timestamp
        // back to local time for the user.
        const QString dateText =
            record.startedAt
                .toLocalTime()
                .toString(
                    "dd MMM yyyy HH:mm:ss"
                    );

        QString durationText;

        if (record.durationMs < 1000) {

            durationText =
                QString("%1 ms")
                    .arg(record.durationMs);

        } else {

            const double seconds =
                record.durationMs / 1000.0;

            durationText =
                QString("%1 s")
                    .arg(
                        seconds,
                        0,
                        'f',
                        2
                        );
        }

        auto *dateItem =
            new QTableWidgetItem(
                dateText
                );

        auto *statusItem =
            new QTableWidgetItem(
                record.status
                );

        auto *sourceItem =
            new QTableWidgetItem(
                record.sourcePath
                );

        auto *destinationItem =
            new QTableWidgetItem(
                record.destinationPath
                );

        auto *copiedItem =
            new QTableWidgetItem(
                QString::number(
                    record.copiedFiles
                    )
                );

        auto *verifiedItem =
            new QTableWidgetItem(
                QString::number(
                    record.verifiedFiles
                    )
                );

        auto *failedItem =
            new QTableWidgetItem(
                QString::number(
                    record.failedFiles +
                    record.integrityFailures
                    )
                );

        auto *durationItem =
            new QTableWidgetItem(
                durationText
                );

        // Useful when paths are truncated.
        sourceItem->setToolTip(
            record.sourcePath
            );

        destinationItem->setToolTip(
            record.destinationPath
            );

        // Store actual backup path
        // inside the row for later use.
        sourceItem->setData(
            Qt::UserRole,
            record.backupPath
            );
        sourceItem->setData(
            Qt::UserRole + 1,
            record.id
            );

        sourceItem->setData(
            Qt::UserRole + 2,
            record.sourcePath
            );

        ui->historyTable->setItem(
            row,
            0,
            dateItem
            );

        ui->historyTable->setItem(
            row,
            1,
            statusItem
            );

        ui->historyTable->setItem(
            row,
            2,
            sourceItem
            );

        ui->historyTable->setItem(
            row,
            3,
            destinationItem
            );

        ui->historyTable->setItem(
            row,
            4,
            copiedItem
            );

        ui->historyTable->setItem(
            row,
            5,
            verifiedItem
            );

        ui->historyTable->setItem(
            row,
            6,
            failedItem
            );

        ui->historyTable->setItem(
            row,
            7,
            durationItem
            );
    }

    statusBar()->showMessage(
        QString(
            "%1 backup record(s) loaded."
            )
            .arg(records.size())
        );

}
void MainWindow::handleHistorySelection()
{
    const bool hasSelection =
        !ui->historyTable->selectedItems().isEmpty();

    const bool operationRunning =
        m_backupRunning ||
        m_restoreRunning ||
        m_integrityRunning;

    ui->openBackupButton->setEnabled(
        hasSelection
        );

    ui->restoreBackupButton->setEnabled(
        hasSelection &&
        !operationRunning
        );

    ui->checkIntegrityButton->setEnabled(
        hasSelection &&
        !operationRunning
        );
}
QString MainWindow::selectedBackupPath() const
{
    const int row =
        ui->historyTable->currentRow();

    if (row < 0) {
        return {};
    }

    QTableWidgetItem *sourceItem =
        ui->historyTable->item(
            row,
            2
            );

    if (sourceItem == nullptr) {
        return {};
    }

    return sourceItem
        ->data(Qt::UserRole)
        .toString();
}

void MainWindow::restoreSelectedBackup()
{
    if (m_backupRunning ||
        m_restoreRunning ||
        m_integrityRunning) {

        return;
    }

    const QString backupPath =
        selectedBackupPath();

    if (backupPath.isEmpty()) {
        return;
    }

    if (!QDir(backupPath).exists()) {

        QMessageBox::warning(
            this,
            tr("Backup Not Found"),
            tr(
                "The selected backup directory no longer exists."
                )
            );

        return;
    }

    const QString destinationPath =
        QFileDialog::getExistingDirectory(
            this,
            tr("Select Restore Destination"),
            QDir::homePath(),
            QFileDialog::ShowDirsOnly |
                QFileDialog::DontResolveSymlinks
            );

    if (destinationPath.isEmpty()) {
        return;
    }

    m_restoreRunning = true;

    ui->restoreBackupButton->setEnabled(
        false
        );

    ui->startBackupButton->setEnabled(
        false
        );

    ui->backupProgressBar->setRange(
        0,
        0
        );

    ui->progressLabel->setText(
        "Preparing restore..."
        );

    statusBar()->showMessage(
        "Preparing restore..."
        );

    m_restoreThread =
        new QThread(this);

    m_restoreWorker =
        new RestoreWorker(
            backupPath,
            destinationPath
            );

    m_restoreWorker->moveToThread(
        m_restoreThread
        );

    connect(
        m_restoreThread,
        &QThread::started,
        m_restoreWorker,
        &RestoreWorker::process
        );

    connect(
        m_restoreWorker,
        &RestoreWorker::progressChanged,
        this,
        &MainWindow::handleRestoreProgress
        );

    connect(
        m_restoreWorker,
        &RestoreWorker::finished,
        this,
        &MainWindow::handleRestoreFinished
        );

    connect(
        m_restoreWorker,
        &RestoreWorker::finished,
        m_restoreThread,
        &QThread::quit
        );

    connect(
        m_restoreWorker,
        &RestoreWorker::finished,
        m_restoreWorker,
        &QObject::deleteLater
        );

    connect(
        m_restoreThread,
        &QThread::finished,
        m_restoreThread,
        &QObject::deleteLater
        );

    connect(
        m_restoreThread,
        &QThread::finished,
        this,
        [this]()
        {
            m_restoreThread = nullptr;
            m_restoreWorker = nullptr;
        }
        );

    m_restoreThread->start();
}

void MainWindow::handleRestoreProgress(
    int processedFiles,
    int totalFiles,
    const QString &currentFile
    )
{
    if (totalFiles <= 0) {

        ui->backupProgressBar->setRange(
            0,
            0
            );

        return;
    }

    ui->backupProgressBar->setRange(
        0,
        totalFiles
        );

    ui->backupProgressBar->setValue(
        processedFiles
        );

    ui->progressLabel->setText(
        QString(
            "Restoring %1 / %2 — %3"
            )
            .arg(processedFiles)
            .arg(totalFiles)
            .arg(currentFile)
        );
}

void MainWindow::handleRestoreFinished(
    const RestoreResult &result
    )
{
    m_restoreRunning = false;

    updateStartButtonState();
    handleHistorySelection();

    if (ui->backupProgressBar->maximum() > 0) {

        ui->backupProgressBar->setValue(
            ui->backupProgressBar->maximum()
            );
    }

    if (result.success) {

        ui->progressLabel->setText(
            "Restore completed successfully."
            );

        statusBar()->showMessage(
            "Restore completed successfully."
            );

        QMessageBox::information(
            this,
            tr("Restore Complete"),
            tr(
                "Restore completed successfully.\n\n"
                "Files restored: %1\n"
                "Files verified: %2\n"
                "Files skipped: %3\n\n"
                "Restore location:\n%4"
                )
                .arg(result.restoredFiles)
                .arg(result.verifiedFiles)
                .arg(result.skippedFiles)
                .arg(result.restorePath)
            );

        return;
    }

    QString details;

    if (!result.errors.isEmpty()) {

        details =
            "\n\nErrors:\n"
            + result.errors.join("\n");
    }

    QMessageBox::warning(
        this,
        tr("Restore Completed With Errors"),
        tr(
            "Restore completed with errors.\n\n"
            "Restored: %1\n"
            "Verified: %2\n"
            "Failed: %3\n"
            "Integrity failures: %4\n"
            "Skipped: %5"
            )
                .arg(result.restoredFiles)
                .arg(result.verifiedFiles)
                .arg(result.failedFiles)
                .arg(result.integrityFailures)
                .arg(result.skippedFiles)
            + details
        );
}
void MainWindow::openSelectedBackupFolder()
{
    const QString backupPath =
        selectedBackupPath();

    if (backupPath.isEmpty()) {
        return;
    }

    if (!QDir(backupPath).exists()) {

        QMessageBox::warning(
            this,
            tr("Backup Not Found"),
            tr(
                "The backup directory no longer exists:\n\n%1"
                ).arg(backupPath)
            );

        return;
    }

    QDesktopServices::openUrl(
        QUrl::fromLocalFile(
            backupPath
            )
        );
}
QString MainWindow::selectedSourcePath() const
{
    const int row =
        ui->historyTable->currentRow();

    if (row < 0) {
        return {};
    }

    QTableWidgetItem *sourceItem =
        ui->historyTable->item(
            row,
            2
            );

    if (sourceItem == nullptr) {
        return {};
    }

    return sourceItem
        ->data(Qt::UserRole + 2)
        .toString();
}
///////////////////////////////////////////////////
void MainWindow::checkSelectedBackupIntegrity()
{
    if (m_backupRunning ||
        m_restoreRunning ||
        m_integrityRunning) {

        return;
    }

    const QString backupPath =
        selectedBackupPath();

    const QString sourcePath =
        selectedSourcePath();

    if (backupPath.isEmpty() ||
        sourcePath.isEmpty()) {

        QMessageBox::warning(
            this,
            tr("Integrity Check"),
            tr(
                "The selected backup does not contain enough information "
                "to perform an integrity scan."
                )
            );

        return;
    }

    if (!QDir(backupPath).exists()) {

        QMessageBox::warning(
            this,
            tr("Backup Not Found"),
            tr(
                "The selected backup directory no longer exists:\n\n%1"
                ).arg(backupPath)
            );

        return;
    }

    if (!QDir(sourcePath).exists()) {

        QMessageBox::warning(
            this,
            tr("Source Not Found"),
            tr(
                "The original source directory no longer exists:\n\n%1"
                ).arg(sourcePath)
            );

        return;
    }

    const QString manifestPath =
        QDir(backupPath).filePath(
            ".integrity_manifest.json"
            );

    if (!QFileInfo::exists(
            manifestPath
            )) {

        QMessageBox::warning(
            this,
            tr("Manifest Not Found"),
            tr(
                "This backup does not contain an integrity manifest.\n\n"
                "Older backups created before integrity tracking was added "
                "cannot be scanned."
                )
            );

        return;
    }

    m_integrityRunning = true;

    ui->checkIntegrityButton->setEnabled(
        false
        );

    ui->restoreBackupButton->setEnabled(
        false
        );

    ui->startBackupButton->setEnabled(
        false
        );

    ui->backupProgressBar->setRange(
        0,
        0
        );

    ui->progressLabel->setText(
        "Preparing integrity scan..."
        );

    statusBar()->showMessage(
        "Preparing integrity scan..."
        );

    m_integrityThread =
        new QThread(this);

    m_integrityWorker =
        new IntegrityWorker(
            sourcePath,
            backupPath
            );

    m_integrityWorker->moveToThread(
        m_integrityThread
        );

    connect(
        m_integrityThread,
        &QThread::started,
        m_integrityWorker,
        &IntegrityWorker::process
        );

    connect(
        m_integrityWorker,
        &IntegrityWorker::progressChanged,
        this,
        &MainWindow::handleIntegrityProgress
        );

    connect(
        m_integrityWorker,
        &IntegrityWorker::finished,
        this,
        &MainWindow::handleIntegrityFinished
        );

    connect(
        m_integrityWorker,
        &IntegrityWorker::finished,
        m_integrityThread,
        &QThread::quit
        );

    connect(
        m_integrityWorker,
        &IntegrityWorker::finished,
        m_integrityWorker,
        &QObject::deleteLater
        );

    connect(
        m_integrityThread,
        &QThread::finished,
        m_integrityThread,
        &QObject::deleteLater
        );

    connect(
        m_integrityThread,
        &QThread::finished,
        this,
        [this]()
        {
            m_integrityThread = nullptr;
            m_integrityWorker = nullptr;
        }
        );

    m_integrityThread->start();
}
//////////////////////////////////////////////////////////////////////
void MainWindow::handleIntegrityProgress(
    int processedFiles,
    int totalFiles,
    const QString &currentFile
    )
{
    if (totalFiles <= 0) {

        ui->backupProgressBar->setRange(
            0,
            0
            );

        ui->progressLabel->setText(
            "Preparing integrity scan..."
            );

        return;
    }

    ui->backupProgressBar->setRange(
        0,
        totalFiles
        );

    ui->backupProgressBar->setValue(
        processedFiles
        );

    if (currentFile.isEmpty()) {

        ui->progressLabel->setText(
            QString(
                "Integrity scan: %1 / %2 files"
                )
                .arg(processedFiles)
                .arg(totalFiles)
            );

        return;
    }

    ui->progressLabel->setText(
        QString(
            "Scanning %1 / %2 — %3"
            )
            .arg(processedFiles)
            .arg(totalFiles)
            .arg(currentFile)
        );

    statusBar()->showMessage(
        QString(
            "Checking %1"
            ).arg(currentFile)
        );
}
/////////////////////////////////////////////////////////////////////////////////////
void MainWindow::handleIntegrityFinished(
    const IntegrityResult &result
    )
{
    m_integrityRunning = false;

    updateStartButtonState();
    handleHistorySelection();

    if (ui->backupProgressBar->maximum() > 0) {

        ui->backupProgressBar->setValue(
            ui->backupProgressBar->maximum()
            );
    }

    if (!result.success) {

        ui->progressLabel->setText(
            "Integrity scan completed with errors."
            );

        statusBar()->showMessage(
            "Integrity scan completed with errors."
            );

        QString details;

        if (!result.errors.isEmpty()) {
            details =
                result.errors.join("\n");
        }

        QMessageBox messageBox(this);

        messageBox.setIcon(
            QMessageBox::Warning
            );

        messageBox.setWindowTitle(
            tr("Integrity Scan Error")
            );

        messageBox.setText(
            tr(
                "The integrity scan could not complete cleanly.\n\n"
                "Failed files: %1"
                )
                .arg(result.failedFiles)
            );

        if (!details.isEmpty()) {
            messageBox.setDetailedText(
                details
                );
        }

        messageBox.exec();

        return;
    }

    const bool changesDetected =
        result.modifiedFiles > 0 ||
        result.missingFiles > 0 ||
        result.newFiles > 0;

    if (changesDetected) {

        ui->progressLabel->setText(
            "Integrity changes detected."
            );

        statusBar()->showMessage(
            "Integrity scan detected file changes."
            );

    } else {

        ui->progressLabel->setText(
            "Integrity verified — no changes detected."
            );

        statusBar()->showMessage(
            "Integrity verified."
            );
    }

    QStringList details;

    if (!result.modifiedPaths.isEmpty()) {

        details.append(
            "MODIFIED FILES"
            );

        for (const QString &path :
             result.modifiedPaths) {

            details.append(
                "  " + path
                );
        }

        details.append("");
    }

    if (!result.missingPaths.isEmpty()) {

        details.append(
            "MISSING FILES"
            );

        for (const QString &path :
             result.missingPaths) {

            details.append(
                "  " + path
                );
        }

        details.append("");
    }

    if (!result.newPaths.isEmpty()) {

        details.append(
            "NEW FILES"
            );

        for (const QString &path :
             result.newPaths) {

            details.append(
                "  " + path
                );
        }
    }

    QMessageBox messageBox(this);

    messageBox.setWindowTitle(
        tr("Integrity Scan Complete")
        );

    messageBox.setIcon(
        changesDetected
            ? QMessageBox::Warning
            : QMessageBox::Information
        );

    messageBox.setText(
        tr(
            "Integrity scan completed.\n\n"
            "Unchanged: %1\n"
            "Modified: %2\n"
            "Missing: %3\n"
            "New: %4"
            )
            .arg(result.unchangedFiles)
            .arg(result.modifiedFiles)
            .arg(result.missingFiles)
            .arg(result.newFiles)
        );

    if (!details.isEmpty()) {

        messageBox.setDetailedText(
            details.join("\n")
            );
    }

    messageBox.exec();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core/backup/BackupService.h"
#include "core/backup/BackupWorker.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QThread>
#include <QDebug>

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

    // Required because BackupResult travels
    // between worker and GUI threads.
    qRegisterMetaType<BackupResult>(
        "BackupResult"
        );

    ui->sourcePathEdit->setReadOnly(true);
    ui->destinationPathEdit->setReadOnly(true);

    ui->startBackupButton->setEnabled(false);

    ui->backupProgressBar->setRange(0, 100);
    ui->backupProgressBar->setValue(0);

    ui->progressLabel->setText(
        "Ready"
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
    ui->startBackupButton->setEnabled(
        pathsAreValid &&
        !m_backupRunning
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
    if (m_backupRunning) {
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
}

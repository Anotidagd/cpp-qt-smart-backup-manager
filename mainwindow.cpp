#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>
#include "core/backup/BackupService.h"

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

    return QDir::cleanPath(QDir(path).absolutePath());
}

bool pathsAreEqual(const QString &firstPath, const QString &secondPath)
{
    return normalizePath(firstPath).compare(
               normalizePath(secondPath),
               pathCaseSensitivity
               ) == 0;
}

bool isSameOrChildDirectory(const QString &childPath,
                            const QString &parentPath)
{
    const QString child = normalizePath(childPath);
    QString parent = normalizePath(parentPath);

    if (child.isEmpty() || parent.isEmpty()) {
        return false;
    }

    if (child.compare(parent, pathCaseSensitivity) == 0) {
        return true;
    }

    parent += QDir::separator();

    return child.startsWith(parent, pathCaseSensitivity);
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->sourcePathEdit->setReadOnly(true);
    ui->destinationPathEdit->setReadOnly(true);
    ui->startBackupButton->setEnabled(false);

    connect(ui->browseSourceButton,
            &QPushButton::clicked,
            this,
            &MainWindow::selectSourceFolder);

    connect(ui->browseDestinationButton,
            &QPushButton::clicked,
            this,
            &MainWindow::selectDestinationFolder);

    connect(ui->sourcePathEdit,
            &QLineEdit::textChanged,
            this,
            &MainWindow::updateStartButtonState);

    connect(ui->destinationPathEdit,
            &QLineEdit::textChanged,
            this,
            &MainWindow::updateStartButtonState);

    connect(ui->startBackupButton,
            &QPushButton::clicked,
            this,
            &MainWindow::handleStartBackup);

    statusBar()->showMessage("Select a source folder and backup destination.");
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

    ui->sourcePathEdit->setText(normalizePath(selectedDirectory));
}

void MainWindow::selectDestinationFolder()
{
    QString initialDirectory = ui->destinationPathEdit->text();

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
    const bool pathsAreValid = validatePaths(&errorMessage);

    ui->startBackupButton->setEnabled(pathsAreValid);

    if (pathsAreValid) {
        statusBar()->showMessage(
            "Backup configuration is valid."
            );
    } else if (!ui->sourcePathEdit->text().isEmpty() &&
               !ui->destinationPathEdit->text().isEmpty()) {
        statusBar()->showMessage(errorMessage);
    } else {
        statusBar()->showMessage(
            "Select a source folder and backup destination."
            );
    }
}

bool MainWindow::validatePaths(QString *errorMessage) const
{
    const QString sourcePath =
        normalizePath(ui->sourcePathEdit->text());

    const QString destinationPath =
        normalizePath(ui->destinationPathEdit->text());

    auto setError = [errorMessage](const QString &message) {
        if (errorMessage != nullptr) {
            *errorMessage = message;
        }
    };

    if (sourcePath.isEmpty()) {
        setError("Please select a source folder.");
        return false;
    }

    if (destinationPath.isEmpty()) {
        setError("Please select a backup destination.");
        return false;
    }

    if (!QFileInfo(sourcePath).isDir()) {
        setError("The selected source folder does not exist.");
        return false;
    }

    if (!QFileInfo(destinationPath).isDir()) {
        setError("The backup destination does not exist.");
        return false;
    }

    if (pathsAreEqual(sourcePath, destinationPath)) {
        setError(
            "The source and destination cannot be the same folder."
            );
        return false;
    }

    if (isSameOrChildDirectory(destinationPath, sourcePath)) {
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

    const QString sourcePath =
        ui->sourcePathEdit->text();

    const QString destinationPath =
        ui->destinationPathEdit->text();

    ui->startBackupButton->setEnabled(false);

    statusBar()->showMessage(
        "Backup in progress..."
        );

    BackupService backupService;

    const BackupResult result =
        backupService.createBackup(
            sourcePath,
            destinationPath
            );

    ui->startBackupButton->setEnabled(true);

    if (result.success) {

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
}

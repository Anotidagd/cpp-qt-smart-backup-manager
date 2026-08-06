#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>

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

    QMessageBox::information(
        this,
        tr("Configuration Ready"),
        tr(
            "The source and destination folders are valid.\n\n"
            "The backup engine will be connected in the next stage."
            )
        );
}

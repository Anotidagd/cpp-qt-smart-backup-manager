#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectSourceFolder();
    void selectDestinationFolder();
    void updateStartButtonState();
    void handleStartBackup();

private:
    bool validatePaths(QString *errorMessage = nullptr) const;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

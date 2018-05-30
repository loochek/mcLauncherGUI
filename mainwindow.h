#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "versionpreparator.h"
#include "versionmanifestparser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void on_pushButton_2_clicked();
    void onVParseCompleted();
    void versionJsonDownloaded();
    void unlock();

private:
    Ui::MainWindow *ui;
    VersionPreparator prep;
    VersionManifestParser par;
};

#endif // MAINWINDOW_H

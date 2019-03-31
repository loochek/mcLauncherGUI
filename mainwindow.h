#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "launcher.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void progressChanged(QString status, int current, int all);

private:
    Ui::MainWindow *ui;
    Launcher *launcher;
};

#endif // MAINWINDOW_H

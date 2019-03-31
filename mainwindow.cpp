#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent) :QWidget(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    launcher = new Launcher(this);
    connect(&Singleton<Downloader>::instance(), &Downloader::progressChanged, this, &MainWindow::progressChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete launcher;
}

void MainWindow::on_pushButton_clicked()
{
    launcher->setGamePath("D:/Minecraft/");
    launcher->launch("1.13.2");
}

void MainWindow::progressChanged(QString status, int current, int all)
{
    ui->progressBar->setMaximum(all);
    ui->progressBar->setValue(current);
    ui->progressBar->setFormat(status);
}

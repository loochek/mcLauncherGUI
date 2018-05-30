#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&prep, &VersionPreparator::statusChanged, ui->progressBar, &QProgressBar::setFormat);
    connect(&prep, &VersionPreparator::progressChanged, ui->progressBar, &QProgressBar::setValue);
    connect(&prep, &VersionPreparator::finished, this, unlock);
    connect(&par, &VersionManifestParser::parseCompleted, this, onVParseCompleted);
    par.update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_2_clicked()
{
    connect(&dlMgr, &Downloader::simpleDownloadFinished, this, versionJsonDownloaded);
    ui->progressBar->setValue(0);
    ui->comboBox->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    ui->lineEdit_3->setEnabled(false);
    ui->lineEdit_4->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    dlMgr.simpleDownload({ui->comboBox->currentText() + ".json", par.getUrlById(ui->comboBox->currentText()), ui->lineEdit_2->text() + "versions/" + ui->comboBox->currentText() + "/" + ui->comboBox->currentText() + ".json", "-"});
}

void MainWindow::onVParseCompleted()
{
    ui->comboBox->addItems(par.getIdsList());
}

void MainWindow::versionJsonDownloaded()
{
    disconnect(&dlMgr, &Downloader::simpleDownloadFinished, this, versionJsonDownloaded);
    prep.prepare(ui->lineEdit_2->text() + "versions/" + ui->comboBox->currentText() + "/" + ui->comboBox->currentText() + ".json", ui->lineEdit_2->text(), ui->lineEdit_3->text(), ui->lineEdit_4->text());
}

void MainWindow::unlock()
{
    ui->comboBox->setEnabled(true);
    ui->lineEdit_2->setEnabled(true);
    ui->lineEdit_3->setEnabled(true);
    ui->lineEdit_4->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->pushButton_3->setText("press3");
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->pushButton_2->setText("press2");
    ui->pushButton_3->setText("press2");
}


void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setText("press1");
    ui->pushButton_2->setText("press1");
    ui->pushButton_3->setText("press1");
}


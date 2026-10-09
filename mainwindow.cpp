#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>

#include <QImage>
#include <QPixmap>
#include <QGraphicsView>   // <-- это критически важно для fitInView


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , item(nullptr)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->imgview->setScene(scene);

    QImage image("/home/yuriy/Pictures/test.png");
    if (!image.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(image);
        item = scene->addPixmap(pixmap);

        // 2. Отложенный fitInView, чтобы виджет уже имел финальный размер
        QTimer::singleShot(0, [this]() {
            ui->imgview->fitInView(item, Qt::KeepAspectRatio);
        });
        //ui->imgview->fitInView(item, Qt::KeepAspectRatio); // KeepAspectRatioByExpanding KeepAspectRatio
    } else {
        // На случай, если файл не найден — чтобы не было пустой сцены
        qWarning() << "Не удалось загрузить изображение: /home/yuriy/Pictures/test.png";
    }


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

// resize event
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // 3. При каждом изменении размера окна — снова вписываем картинку
    if (item) {
        QTimer::singleShot(0, [this]() {
            ui->imgview->fitInView(item, Qt::KeepAspectRatio);
        });
    }
}

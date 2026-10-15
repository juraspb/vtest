#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QTimer>

#include <QImage>
#include <QPixmap>
#include <QGraphicsView>   // <-- это критически важно для fitInView
#include <opencv2/opencv.hpp>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , item(nullptr)
{
    ui->setupUi(this);

    isRecording = false;
    outputFilename = "output_video.avi";

    // RTSP-адрес камеры
    QString rtspUrl = "rtsp://192.168.1.102:554/Streaming/Channels/101";
    cap.open(rtspUrl.toStdString());

    if (!cap.isOpened()) {
        qDebug() << "Ошибка: не удалось открыть камеру";
        return;
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::captureFrame);
    timer->start(33); // ~30 FPS

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
    if (cap.isOpened()) cap.release();
    if (videoWriter.isOpened()) videoWriter.release();
}

QPixmap MainWindow::matToQPixmap(const cv::Mat &mat)
{
    cv::Mat rgbMat;
    if (mat.channels() == 3) {
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
    } else {
        rgbMat = mat.clone();
    }

    QImage image(rgbMat.data, rgbMat.cols, rgbMat.rows,
                 rgbMat.step, QImage::Format_RGB888);
    return QPixmap::fromImage(image);
}

void MainWindow::captureFrame()
{
    cap >> cvFrame;
    if (cvFrame.empty()) return;

    ui->labelVideo->setPixmap(matToQPixmap(cvFrame).scaled(
        ui->labelVideo->size(), Qt::KeepAspectRatio));

    if (isRecording && videoWriter.isOpened()) {
        videoWriter.write(cvFrame);
    }
}

void MainWindow::on_btnStartRecording_clicked()
{
    if (isRecording) return;

    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);

    // Если FPS не определён, устанавливаем 30
    if (fps <= 0) fps = 30.0;

    int fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');

    videoWriter.open(outputFilename.toStdString(), fourcc, fps, cv::Size(frameWidth, frameHeight));

    if (videoWriter.isOpened()) {
        isRecording = true;
        ui->btnStartRecording->setEnabled(false);
        ui->btnStopRecording->setEnabled(true);
        qDebug() << "Запись начата: " << outputFilename;
    } else {
        qDebug() << "Ошибка: не удалось открыть VideoWriter";
    }
}

void MainWindow::on_btnStopRecording_clicked()
{
    if (!isRecording) return;

    isRecording = false;
    videoWriter.release();

    ui->btnStartRecording->setEnabled(true);
    ui->btnStopRecording->setEnabled(false);
    qDebug() << "Запись остановлена";
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

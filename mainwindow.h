#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <opencv2/opencv.hpp>

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
    void on_btnStartRecording_clicked();
    void on_btnStopRecording_clicked();
    void resizeEvent(QResizeEvent *event);
    void captureFrame();


private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *item;

    QTimer *timer;

    cv::VideoCapture cap;
    cv::VideoWriter videoWriter;
    bool isRecording;
    QString outputFilename;

    cv::Mat cvFrame;
    QPixmap matToQPixmap(const cv::Mat &mat);

};
#endif // MAINWINDOW_H

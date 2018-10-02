#include "mainwindow.hpp"
#include "ui_mainwindow.h"

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cv::Mat img3(240, 320, CV_8UC3, cv::Scalar(200, 100, 0));
    cv::imshow("IMG3", img3);

}

MainWindow::~MainWindow()
{
    delete ui;
}

QChartView *drawhistogram(int histogramseries[]){

    QBarSet *set = new QBarSet("Histogram");
    for(int i = 0 ; i < 256 ; i++){
        *set << histogramseries[i];
    }

    QBarSeries  *series = new QBarSeries();
    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}

void MainWindow::on_Load_clicked()
{
    //------* Read Image from File Qialog *------//
    //    QString multi_file_path = QFileDialog::getOpenFileNames(this, tr("OpenFile"), "D:/", tr("JPG(*.jpg)"));
    //    qDebug() << multi_file_path;
    QString img_path = QFileDialog::getOpenFileName(this, tr("open file"), "", tr("Image(*.png *.jpg)"));

    src = cv::imread(img_path.toStdString());
    cv::imshow("Source Image", src);


    //------* Mat information *------//
    qDebug() << "src.type:" << src.type();
    qDebug() << "src.rows:" << src.rows;
    qDebug() << "src.cols:" << src.cols;
    qDebug() << "src.channels:" << src.channels();


    //------* Create a one channel mat *------//
    cv::Mat img2(240, 320, CV_8U, cv::Scalar(100));
    cv::imshow("img2", img2);


    //------* Basic drawing *------//
    cv::Mat img3(240, 320, CV_8UC3, cv::Scalar(100, 100, 100));
    cv::line(img3, cv::Point(2, 2), cv::Point(20, 20), cv::Scalar(255, 0, 0));
    cv::rectangle(img3, cv::Point(100, 40), cv::Point(200, 140), cv::Scalar(0, 0, 255), -1); // -1 means full of color
    cv::circle(img3, cv::Point(150, 200), 20, cv::Scalar(0, 255, 0), 4);
    cv::imshow("Basic drawing", img3);


    //------* Color space transform *------//
    cv::Mat HSV, Lab;
    cv::cvtColor(src, HSV, CV_BGR2HSV);
    cv::cvtColor(src, Lab, CV_BGR2Lab);
    cv::imshow("gray", HSV);


    //------* Draw Histogram *------//
    cv::Mat gray;
    cv::cvtColor(src, gray, CV_BGR2GRAY);   // Histogram should do on only one channel
    int histogram[256] = {0};
    for(int i = 0 ; i < gray.rows ; i++){
        for(int j = 0 ; j < gray.cols ; j++){
            for(int h = 0 ; h < 256 ; h++){
                if(gray.at<uchar>(i, j) == h){   histogram[h]++;    break;}
            }
        }
    }

    int max = histogram[0];
    for(int i = 1 ; i < 256 ; i++){
        if(histogram[i] > max)  max = histogram[i];
    }

    cv::Mat histo(250, 256, CV_8U, cv::Scalar(100));    // Create Histogram mat, rows = 250, defined by myself
    double total_pixel = (double)gray.cols * (double)gray.rows;
    for(int i = 0 ; i < 256 ; i++){
        // histogram[i] / total_pixel : normalize histogram to 0~1
        // 15000 is for showing better
        cv::line(histo, cv::Point(i, 250 - (15000 * histogram[i]) / total_pixel), cv::Point(i, 250), cv::Scalar(255));
    }
    cv::imshow("histogram", histo);
    cv::imwrite("./histogram.png", histo);  //   ./ means relative path, relative to your execution directory


    //------* Draw histogram by chartviews *------//
    QChartView *original_histo;
    original_histo = drawhistogram(histogram);  // drawhistogray is self-defined function below
    ui->ori_histo->addWidget(original_histo);


    //------* Histogram equalization *------//
    cv::Mat dst;
    cv::equalizeHist(gray, dst);    // opencv histogram equalization function
    cv::imshow("Histogram Equalization", dst);

}


void MainWindow::on_save_clicked()
{
//    QString save_path = QFileDialog::getSaveFileName(this, tr("save image"));
    QString save_path = QFileDialog::getSaveFileName(this, tr("save image"), NULL, tr("jpg (*.jpg);; png (*.png)"));
    qDebug() << save_path;
    cv::Mat dst = src.clone();
    if(dst.empty()){
        qDebug() << "src is empty";
    }
    else{
        cv::imwrite(save_path.toStdString(), dst);
    }
}


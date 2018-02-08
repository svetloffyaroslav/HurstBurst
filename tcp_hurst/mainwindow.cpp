// ======================================================================
// mainwindow.cpp
// ======================================================================
//           This file is a part of programm Hurst Burst | TCP client v0.1
// ======================================================================
//  Author: Svetlov Yaroslav
//  vk.com : https://vk.com/id52304190
//
//  Contact
//  --------------
//  Email  : hurst_burst@gmail.com
//
//
//  Description:
//  This is the main part of programm.There has been described all algorithms
//  of interface, distributions, and tcp-socket. This programm works like TCP
//  client.
//
//  ---------------
//  Codec:  UTF-8
//  Data: 07.02.2018
// ======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"


// [1]
// Set up interface, tcp-socket, and all signal-slots connection
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /*!
      [1.1] - Set up interface and prelimary settings, like: mask in lineEdit,
      title in head of window, values of variables, which will be use for translation

    */

    //[1.1]
    ui->setupUi(this);
    this->setWindowTitle("HURST BURST | TCP client");
    ui->listWidget->setCurrentRow(0);
    ui->stackedWidget_ParametersOfDistribution->setCurrentIndex(0);
    ui->lineEdit_IPAdres->setInputMask("000.000.000.000;");
    i_numberPacket = 0;

    TcpSocket_One= new QTcpSocket(this);
    Timer_BetweenPacket = new QTimer(this);

    connect(TcpSocket_One,  SIGNAL (connected()),    SLOT(slot_TcpSocket_OneConnected()));
    connect(TcpSocket_One,  SIGNAL (readyRead()),    SLOT(slot_TcpSocket_OneReadyRead()));
    connect(TcpSocket_One, SIGNAL(disconnected()),  SLOT(slot_TcpSocket_OneDisconnected()));
    connect(TcpSocket_One, SIGNAL(error(QAbstractSocket::SocketError)),
             this,                                  SLOT(slot_TcpSocket_OneError(QAbstractSocket::SocketError))
            );
    connect(Timer_BetweenPacket,SIGNAL(timeout()),SLOT(slot_timeoutTimer_BetweenPacket()));

    // set dark background gradient:
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(90, 90, 90));
    gradient.setColorAt(0.38, QColor(105, 105, 105));
    gradient.setColorAt(1, QColor(70, 70, 70));
    ui->widget_customplot->setBackground(QBrush(gradient));

    ui->widget_customplotNoise->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

}

void MainWindow::slot_TcpSocket_OneConnected()
{
    /* slot */
    ui->statusBar->showMessage(tr("Подключился"),3000);
    ui->groupBox_TimeRow->setEnabled(true);
    ui->groupBox_Transmission->setEnabled(true);
}
void MainWindow::slot_TcpSocket_OneReadyRead()
{
    qDebug()<<"Готовность к чтению";
}

void MainWindow::slot_TcpSocket_OneDisconnected()
{
    ui->statusBar->showMessage("Отключился",3000);
    ui->groupBox_TimeRow->setEnabled(false);
    ui->groupBox_Transmission->setEnabled(false);
    Timer_BetweenPacket->stop();
}

void MainWindow::slot_TcpSocket_OneError(QAbstractSocket::SocketError error_One)
{
    QString strSystemError =
        "Error: " + (error_One == QAbstractSocket::HostNotFoundError ?
                     "System Socket: The host was not found." :
                     error_One == QAbstractSocket::RemoteHostClosedError ?
                     "System Socket: The remote host is closed." :
                     error_One == QAbstractSocket::ConnectionRefusedError ?
                     "System Socket: The connection was refused." :
                     QString(TcpSocket_One->errorString())
                    );
    ui->statusBar->showMessage(strSystemError,3000);
}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_ConnectOrDisconnect_toggled(bool checked)
{
     if(checked)
    {
        QString string_IPAdress = ui->lineEdit_IPAdres->text();
        quint16 string_Port = ui->lineEdit_Port->text().toUShort();
        QHostAddress address(string_IPAdress);
        TcpSocket_One -> connectToHost (address,string_Port);
        ui->pushButton_ConnectOrDisconnect->setText(tr("Отключить"));
    }
    else
    {
        TcpSocket_One  -> disconnectFromHost();
        ui->pushButton_ConnectOrDisconnect->setText(tr("Подключить"));
    }
}



void MainWindow::on_pushButton_Transmit_clicked()
{
    ui->progressBar->setValue(0);
    ByteArray_Send.clear();

    ByteArray_Send.resize(ui->spinBox_SizeOfPacket->value());
    ByteArray_Send.fill('x');

    sendToSocket();

    i_numberPacket = 0;
    StartTimerDelta(i_numberPacket);
    ui->progressBar->setMaximum(ui->spinBox_parametr_N->value());
}

void MainWindow::StartTimerDelta(int i)
{
    if(i>ui->spinBox_parametr_N->value())
    {
        Timer_BetweenPacket->stop();
    }
    else
    {
        Timer_BetweenPacket->start((int)(vector_DeltaTime[i]));
    }
}


void MainWindow::slot_timeoutTimer_BetweenPacket()
{
    i_numberPacket++;
    ui->progressBar->setValue(i_numberPacket);
    if(i_numberPacket<ui->spinBox_parametr_N->value())
    {
        sendToSocket();
        StartTimerDelta(i_numberPacket);
    }
    else
    {
       Timer_BetweenPacket->stop();
    }
}



void MainWindow::sendToSocket()
{

    TcpSocket_One->write(ByteArray_Send);
    TcpSocket_One->flush();
}


void MainWindow::on_action_SaveTime_triggered()
{



}

void MainWindow::GenerateTime(int i_WhatGen)
{

    ui-> tableWidget ->setRowCount(0);
    vector_DeltaTime.clear();

    float f_a = ui->lineEdit_parametr_a->text().toFloat();
    float f_k = ui->lineEdit_parametr_k->text().toFloat();

    float f_L = ui->lineEdit_parametr_L->text().toFloat();

    int N = ui->spinBox_parametr_N->value();

    vector_DeltaTime.resize(N);

    char* outname;

    switch (i_WhatGen)
    {
        case 1: outname = "HurstBurst_pareto.txt"; break;
        case 2: outname = "HurstBurst_exp.txt"; break;
        case 3: outname = "HurstBurst_weibull.txt"; break;
        case 4: outname = "HurstBurst_lognorm.txt"; break;
        default:                                 break;
    }

    FILE* OUT_F;
    OUT_F = fopen(outname, "wt");

    const int range_from = 0;// N(0,1)
    const int range_to = 1;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    //std::
    // std::random_device                      rand_dev;
    std::mt19937                            generator(seed);
    std::uniform_real_distribution<float>   distr(range_from, range_to);


    float f_tens = 0;
    if(ui->radioButton_1X->isChecked())
    {
        f_tens = 1.0;
    }
    else if(ui->radioButton_X10->isChecked())
    {
        f_tens = 10.0;
    }
    else if(ui->radioButton_X100->isChecked())
    {
        f_tens = 100.0;
    }

 if(i_WhatGen==1)
 {
    for(int s= 0;s<N;s++)
    {
        float rand = distr(generator);
        float y = f_k/pow(1-rand,1.0/f_a);
        vector_DeltaTime[s] = y*f_tens;
        fprintf(OUT_F, "%f\n",y);
    }
 }
 else if(i_WhatGen==2)
 {
     for(int s= 0;s<N;s++)
     {
         float r = distr(generator);
         float y =  - log(r)/f_L;
         vector_DeltaTime[s] = y*f_tens;
         fprintf(OUT_F, "%f\n",y);
     }
 }
 else if(i_WhatGen==3)
 {
     std::weibull_distribution<float> weibull_distr(ui->lineEdit_weibull_a->text().toDouble(),ui->lineEdit_weibull_b->text().toDouble());
     for(int s= 0;s<N;s++)
     {
         float r = weibull_distr(generator);
         vector_DeltaTime[s]=r*f_tens;
         fprintf(OUT_F,"%f\n",r);
     }
 }
 else if(i_WhatGen==4)
 {
     std::lognormal_distribution<float> lognorm_distr(ui->lineEdit_lognorm_m->text().toDouble(),ui->lineEdit_lognorm_s->text().toDouble());
     for(int s= 0;s<N;s++)
     {
          float r = lognorm_distr(generator);
          vector_DeltaTime[s]=r*f_tens;
          fprintf(OUT_F,"%f\n",r);
     }
 }

 fclose(OUT_F);
 ui-> tableWidget ->setRowCount(N);
 QTableWidgetItem *newItem = new QTableWidgetItem();
 newItem->setText(QString::number(0));
 newItem->setTextAlignment(Qt::AlignHCenter);
 ui->tableWidget->setItem(0,0,newItem);

 int i_currentRow = 0;
 for(int i=0;i<N;i++)
   {
     i_currentRow++;
     QTableWidgetItem *newItem = new QTableWidgetItem();
     newItem->setText(QString::number(vector_DeltaTime[i]));
     newItem->setTextAlignment(Qt::AlignHCenter);
     ui->tableWidget->setItem(i_currentRow,0,newItem);
     ui->tableWidget->setCurrentCell(i,0);
   }

     // drawGraphic();
     drawGraphicNoise();
}

void MainWindow::on_pushButton_DownloadFromTXT_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                         "/home",
                                                         tr("Текстовые файлы (*txt)"));
    vector_DeltaTime.clear();
    ui-> tableWidget ->setRowCount(0);
    QFile inputFile(fileName);

    i_line = -1;

     if (inputFile.open(QIODevice::ReadOnly))
      {
         QTextStream stream(&inputFile);
         QString line;
         do
         {
             i_line++;
             line = stream.readLine();

         } while (!line.isNull());

         vector_DeltaTime.resize(i_line);
         ui->spinBox_parametr_N->setValue(i_line);
     }
     inputFile.close();

  if (inputFile.open(QIODevice::ReadOnly))
  {
     i_line = 0;
     QTextStream stream(&inputFile);
     QString line;
      do
      {

        line = stream.readLine();
        if(line.toFloat()!=0)
        {
           vector_DeltaTime[i_line] = line.toFloat();
           i_line++;
        }

      } while (!line.isNull());
  }

     ui-> tableWidget ->setRowCount(i_line);
     QTableWidgetItem *newItem = new QTableWidgetItem();
     newItem->setText(QString::number(0));
     newItem->setTextAlignment(Qt::AlignHCenter);
     ui->tableWidget->setItem(0,0,newItem);

     int i_currentRow = 0;
     for(int i=0;i<vector_DeltaTime.size();i++)
       {
         i_currentRow++;
         QTableWidgetItem *newItem = new QTableWidgetItem();
         newItem->setText(QString::number(vector_DeltaTime[i]));
         newItem->setTextAlignment(Qt::AlignHCenter);
         ui->tableWidget->setItem(i_currentRow,0,newItem);
         ui->tableWidget->setCurrentCell(i,0);
       }
}

void MainWindow::drawGraphic()
{

//    QCPBars *bars_distribution = new QCPBars(ui->widget_customplot->xAxis, ui->widget_customplot->yAxis);
//    bars_distribution->setAntialiased(false); // gives more crisp, pixel aligned bar borders

//    bars_distribution->setPen(QPen(QColor(111, 9, 176).lighter(170)));


    QCPBars *fossil = new QCPBars(ui->widget_customplot->xAxis, ui->widget_customplot->yAxis);

    fossil->setAntialiased(false);
    // set names and colors:
    fossil->setName("Fossil fuels");
    fossil->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    fossil->setBrush(QColor(111, 9, 176));

    // prepare x axis with country labels:
    ui->widget_customplot->xAxis->setTickLabelRotation(150);
    // ui->widget_customplot->xAxis->setSubTicks(false);
    ui->widget_customplot->xAxis->setTickLength(0, 8);
    ui->widget_customplot->xAxis->setRange(0, 8);
    ui->widget_customplot->xAxis->setBasePen(QPen(Qt::white));
    ui->widget_customplot->xAxis->setTickPen(QPen(Qt::white));
    ui->widget_customplot->xAxis->grid()->setVisible(true);
    ui->widget_customplot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    ui->widget_customplot->xAxis->setTickLabelColor(Qt::white);
    ui->widget_customplot->xAxis->setLabelColor(Qt::white);

    // prepare y axis:
    ui->widget_customplot->yAxis->setRange(0, 5.1);
    ui->widget_customplot->yAxis->setPadding(5); // a bit more space to the left border
    ui->widget_customplot->yAxis->setLabel("Power Consumption in\nKilowatts per Capita (2007)");
    ui->widget_customplot->yAxis->setBasePen(QPen(Qt::white));
    ui->widget_customplot->yAxis->setTickPen(QPen(Qt::white));
    ui->widget_customplot->yAxis->setSubTickPen(QPen(Qt::white));
    ui->widget_customplot->yAxis->grid()->setSubGridVisible(true);
    ui->widget_customplot->yAxis->setTickLabelColor(Qt::white);
    ui->widget_customplot->yAxis->setLabelColor(Qt::white);
    ui->widget_customplot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    ui->widget_customplot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));

    QVector<double> fossilData;
    QVector<double> ticks;

      ticks <<1 <<2  << 3 << 4 << 5 << 6 << 7;
      fossilData << 0.86*10.5 << 0.83*5.5 << 0.84*5.5 << 0.52*5.8 << 0.89*5.2 << 0.90*4.2 << 0.67*11;


    // например


    // отсортировали от меньшего к старшему
    // qSort(vector_DeltaTime.begin(), vector_DeltaTime.end());

     // Преобразования к double


    fossil->setData(ticks,fossilData);

    // setup legend:
    ui->widget_customplot->legend->setVisible(true);
    ui->widget_customplot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
    ui->widget_customplot->legend->setBrush(QColor(255, 255, 255, 100));
    ui->widget_customplot->legend->setBorderPen(Qt::NoPen);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    ui->widget_customplot->legend->setFont(legendFont);
    ui->widget_customplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->widget_customplot->replot();
}

void MainWindow::drawGraphicNoise()
{
    QVector<double> d_x(vector_DeltaTime.size()),d_y(vector_DeltaTime.size());                                     // инициализиурется два вектора типа double для отправки в класс QCustomPlot и построения графиков



    for(int i=0;i<vector_DeltaTime.size();i++)                                                                // цикл от 0 до количества всех пришедших значений
     {
        d_x[i] =(double)i;                                                                               // в x вектор попадают значения i
        d_y[i] =(double)vector_DeltaTime[i];                                                                          // в y вектор заходят пришедшие значение импульсной характеристики
     }

    double minY = d_y[0],maxY = d_y[0];                                                                 // расчет максимального и минимального значения для вектора y
    for(int i = 0;i<vector_DeltaTime.size();i++)                                                             // Снова прогоняем цикл сравнивая значения
     {
        if (d_y[i]<minY) minY = d_y[i];                                                                  // устанавливая минимальное
        if (d_y[i]>maxY) maxY = d_y[i];                                                                  //  и устанавливая максимальное
     }


        ui->widget_customplotNoise->clearGraphs();                                                          // очищаем график
        ui->widget_customplotNoise->addGraph();                                                             // добавляем новый график
        ui->widget_customplotNoise->xAxis->setRange(d_x[0],d_x[vector_DeltaTime.size()-1]);                      // Устанавливаем диапазон - от Xo до Xмах
        ui->widget_customplotNoise->yAxis->setRange(minY,maxY);                                             // Устанавливает диапазон  от Ymin до Ymax
        ui->widget_customplotNoise->graph(0)->setData(d_x,d_y);                                             // Устанавливаем данные
//                                          ui->widget_GraphImpulse->xAxis->setLabel("x");                                                   // легенда
//                                          ui->widget_GraphImpulse->yAxis->setLabel("y");
        ui->widget_customplotNoise->replot();
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    ui->stackedWidget_ParametersOfDistribution->setCurrentIndex(index.row());
}

void MainWindow::on_pushButton_GenerateTime_clicked()
{
    GenerateTime(ui->stackedWidget_ParametersOfDistribution->currentIndex()+1);
}

void MainWindow::on_pushButton_clicked()
{
    drawGraphic();
}

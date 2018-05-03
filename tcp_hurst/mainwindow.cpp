// ======================================================================
// mainwindow.cpp
// ======================================================================
//      This file is a part of programm Hurst Burst | TCP client v0.1
// ======================================================================
//  Author: Svetlov Yaroslav
//  vk.com : https://vk.com/id52304190
//
//  Contact
//  --------------
//  Email  : hurst_burst@gmail.com
//
//  Description:
//  This is the main part of programm.There has been described all algorithms
//  of interface, distributions, and tcp-socket. This programm works like TCP
//  client.
//
//  ---------------
//  Codec:  UTF-8
//  Data: 14.02.2018
// ======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"

#define ER 2.72


Q_LOGGING_CATEGORY(logDebug,    "Debug ")
Q_LOGGING_CATEGORY(logWarning,  "Warn  ")
Q_LOGGING_CATEGORY(logCritical, "Crit  ")
Q_LOGGING_CATEGORY(logPutInfo,  "Info  ")

// [1]
// Set up interface, tcp-socket, and all signal-slots connection
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /*!
      [1.1] - Set up interface and prelimary settings, like: mask in lineEdit,
      title in head of window, values of variables, which will be use for translation;
      [1.2] - Create new objects of timer and tcp-sockets;
      [1.3] - Organize connection signals with slots: such as
    */

    //[1.1]
    ui->setupUi(this);
    this->setWindowTitle("HURST BURST | TCP client");
    ui->listWidget->setCurrentRow(0);
    ui->stackedWidget_ParametersOfDistribution->setCurrentIndex(0);
    ui->lineEdit_IPAdres->setInputMask("000.000.000.000;");
    i_numberPacket = 0;

    // [1.2]
    TcpSocket_One= new QTcpSocket(this);
    Timer_BetweenPacket = new QTimer(this);
    colorpanel_noise = new setcolorandsettingsgraph();
    colorpanel_histogram = new setcolorandsettingsgraph();
    bars_packets = new QCPBars(ui->widget_customplot->xAxis, ui->widget_customplot->yAxis);

    // [1.3]
    connect(TcpSocket_One, SIGNAL (connected()),    SLOT(slot_TcpSocket_OneConnected()));
    connect(TcpSocket_One, SIGNAL (readyRead()),    SLOT(slot_TcpSocket_OneReadyRead()));
    connect(TcpSocket_One, SIGNAL(disconnected()),  SLOT(slot_TcpSocket_OneDisconnected()));
    connect(TcpSocket_One, SIGNAL(error(QAbstractSocket::SocketError)),
             this,                                  SLOT(slot_TcpSocket_OneError(QAbstractSocket::SocketError))
            );
    connect(Timer_BetweenPacket,SIGNAL(timeout()),SLOT(slot_timeoutTimer_BetweenPacket()));
   //  connect(ui->widget_customplotNoise,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(slot_clickedPointNoise(QMouseEvent*)));
    connect(colorpanel_noise,SIGNAL(signal_setColorsGraph(QString,QString,QString)),this,SLOT(slot_setColorGraph(QString,QString,QString)));
    connect(colorpanel_histogram,SIGNAL(signal_setColorsGraph(QString,QString,QString)),this,SLOT(slot_setColorHist(QString,QString,QString)));

    // set dark background gradient for histgram graphic:
//    QLinearGradient gradient(0, 0, 0, 400);
//    gradient.setColorAt(0, QColor(90, 90, 90));
//    gradient.setColorAt(0.38, QColor(105, 105, 105));
//    gradient.setColorAt(1, QColor(70, 70, 70));
//    ui->widget_customplot->setBackground(QBrush(gradient));
    ui->widget_customplotNoise->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ui->widget_customplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    on_toolButton_grab_handNoise_toggled(true);

}

void MainWindow::slot_setColorHist(QString qstr_BackgroundColor,QString qstr_LineColor,QString qstr_PourColor)
{
    ui->widget_customplot->setBackground(QBrush(QColor(qstr_BackgroundColor),Qt::SolidPattern));
    bars_packets->setPen(QPen(QColor(qstr_LineColor)));
    bars_packets->setBrush(QColor(qstr_PourColor));
    ui->widget_customplot->replot();

    // ui->widget_customplot->graph(0)->setPen(QColor(qstr_LineColor));
}

void MainWindow::slot_setColorGraph(QString qstr_BackgroundColor,QString qstr_LineColor, QString qstr_PourColor)
{

   qDebug()<<"qstr_BackgroundColor = " <<qstr_BackgroundColor<<"   qstr_LineColor = "<<qstr_LineColor<<"  qstr_LineColor"<<qstr_LineColor;

   ui->widget_customplotNoise->setBackground(QBrush(QColor(qstr_BackgroundColor),Qt::SolidPattern));
   ui->widget_customplotNoise->graph(0)->setPen(QColor(qstr_LineColor));
   ui->widget_customplotNoise->graph(0)->setBrush(QColor(qstr_LineColor));

   ui->widget_customplotNoise->xAxis->setTickPen(QColor(qstr_PourColor));
   ui->widget_customplotNoise->xAxis->setBasePen(QColor(qstr_PourColor));
   ui->widget_customplotNoise->xAxis->setSubTickPen(QColor(qstr_PourColor));
   ui->widget_customplotNoise->yAxis->setTickPen(QColor(qstr_PourColor));
   ui->widget_customplotNoise->yAxis->setBasePen(QColor(qstr_PourColor));
   ui->widget_customplotNoise->yAxis->setSubTickPen(QColor(qstr_PourColor));

   ui->widget_customplotNoise->yAxis->setTickLabelColor(QColor(qstr_PourColor));
   ui->widget_customplotNoise->yAxis->setLabelColor(QColor(qstr_PourColor));
   ui->widget_customplotNoise->xAxis->setTickLabelColor(QColor(qstr_PourColor));
   ui->widget_customplotNoise->xAxis->setLabelColor(QColor(qstr_PourColor));
   ui->widget_customplotNoise->replot();

}

void MainWindow::slot_clickedPointNoise(QMouseEvent* event)
{
    ui->widget_customplotNoise->setCursor(Qt::CrossCursor);
    QCustomPlot *customPlot = ui->widget_customplotNoise;
    static QCPItemLine *hCursor, *vCursor;
    static QCPItemText *phaseTracerText=new QCPItemText(customPlot);
    QString textXYnimber;

    double x=customPlot->xAxis->pixelToCoord(event->pos().x());
    double y=customPlot->yAxis->pixelToCoord(event->pos().y());
    if(hCursor) customPlot->removeItem(hCursor);
    hCursor = new QCPItemLine(customPlot);
    customPlot->addItem(hCursor);
    hCursor->start->setCoords(QCPRange::minRange, y);
    hCursor->end->setCoords(QCPRange::maxRange, y);
    if(vCursor) customPlot->removeItem(vCursor);
    vCursor = new QCPItemLine(customPlot);
    customPlot->addItem(vCursor);
    vCursor->start->setCoords(x, QCPRange::minRange);
    vCursor->end->setCoords(x, QCPRange::maxRange);

    textXYnimber.clear();
    textXYnimber.append("x= "+QString::number(x)+";"+"\n"+"y= "+QString::number(y)+";");

    phaseTracerText->position->setType(QCPItemPosition::ptPlotCoords);
    phaseTracerText->setPositionAlignment(Qt::AlignLeft|Qt::AlignBottom);
    phaseTracerText->position->setCoords(x, y);
    phaseTracerText->setText(textXYnimber);
    phaseTracerText->setTextAlignment(Qt::AlignLeft);
    phaseTracerText->setFont(QFont(font().family(), 9));
    phaseTracerText->setPadding(QMargins(15, 15, 2, 15));
    phaseTracerText->setVisible(true);
    customPlot->replot();

    if(!ui->toolButton_grab_handNoise->isChecked())
    {
        hCursor->setVisible(false);
        vCursor->setVisible(false);
        phaseTracerText->setVisible(false);
    }
}

void MainWindow::slot_TcpSocket_OneConnected()
{
    ui->statusBar->showMessage(tr("Подключился"),3000);
    ui->groupBox_Transmission->setEnabled(true);
}
void MainWindow::slot_TcpSocket_OneReadyRead()
{
    qDebug()<<"Готовность к чтению";
}

void MainWindow::slot_TcpSocket_OneDisconnected()
{
    ui->statusBar->showMessage("Отключился",3000);
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
    outname = "HurstBurst_TEXT.txt";



    FILE* OUT_F;
    OUT_F = fopen(outname, "wt");
    if(OUT_F==NULL)
    {
        qDebug()<<"Failed";
    }

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
 else if(i_WhatGen==5)
 {

     /*
       RMD метод
     */

       // double d_StandartDeviation = sqrt(ui->lineEdit_DispersionRMD->text().toDouble());
       double d_HurstParametr = ui->lineEdit_HurstRMD->text().toDouble();
       int i_Steps = ui->spinBox_CountOfSteps->value(); // кол-во шагов
       N = ui->lineEdit_NumbersRMD->text().toInt(); // 2^n - кол-во сделанных отсчетов

       vector_DeltaTime.resize(N);  // общий массив теперь становится длиной N
       vector_DeltaTime.fill(0);    // заполняем его нулями

       // расчитывем X(0) X(1)
      vector_DeltaTime[0] = 0;  // X(0)
      fprintf(OUT_F,"%f\n",(float)0);

      std::normal_distribution <float> norm_distr(0,1);
      float X_1=0;
      for(;;)
      {
        X_1=norm_distr(generator);  // X(1)
        if(X_1>0)
            break;
      }

      vector_DeltaTime[vector_DeltaTime.size()-1] = X_1;  // X(1)
      fprintf(OUT_F,"%f\n",vector_DeltaTime[vector_DeltaTime.size()-1]);    // запись в файл

       for(int i=1;i<i_Steps;i++)   // запускаем цикл по шагам
       {
           int colvo_val = pow(2,i); // кол-во значений при каждом шаге

           for(int s=1;s<colvo_val;s++) // цикл для подсчета всех значений при каждом шаге
           {

             if(vector_DeltaTime[s*N/pow(2,i)]==0)  // s*N/pow(2,i) - это интересующее нас в массиве место, которое должно быть ровно 0, т.е. в этот элемент массива ничего не должно быть записано
               {
                 float X_more,X_less;   // инициализируются переменные
                 X_less = 0;            // эти переменные являются обязательными для подсчета взятого значения
                 X_more = 0;            // одно из них- ближайшее расчитанное в большую сторону, другое - ближайшее расчитанное в меньшую сторону

                 // в большую сторону
                 for(int p=s*N/pow(2,i);p<=vector_DeltaTime.size()-1;p++)   // цикл проверяет массив от текущего значения в большую сторону до конца массиве
                 {
                     if(vector_DeltaTime[p]!=0)     // на поиск ближайшего ненулевого значения
                     {
                         X_more=vector_DeltaTime[p];    // и записывает его в переменную
                     }
                 }

                 // в меньшую сторону
                 for(int q=s*N/pow(2,i);q>=0;q--)       // цикл проверяет массив от текущего значение в меньшую сторону до конца массива
                 {
                     if(vector_DeltaTime[q]!=0)         // если ближайшее значение не нулевое
                     {
                       X_less =vector_DeltaTime[q]; // записывай его в перменную
                     }
                 }
                 double disp =sqrt(pow(1.0/pow(2.0,i),2.0*d_HurstParametr)+(1.0+pow(2,2*d_HurstParametr-2))*ui->lineEdit_DispersionRMD->text().toDouble());
                 std::normal_distribution <float> norm_distr_N(0,disp);

                 // vector_DeltaTime[s*N/pow(2,i)]=(X_more+X_less)/2+(1/pow(2,(s+1)/2))*norm_distr_N(generator);
                 float d;
                 for(;;)
                 {
                   d= norm_distr_N(generator);
                   if(d>0)
                       break;
                 }

                 vector_DeltaTime[s*N/pow(2,i)]=(X_more+X_less)/2+(1.0/sqrt(pow(2.0,i)))*d;
                 fprintf(OUT_F,"%f\n",vector_DeltaTime[s*N/pow(2,i)]);    // запись в файл

             }
           }
       }

 }
 else if(i_WhatGen==6)
 {
     /*
        SRA-метод
    */
     double d_HurstParametrSRA = ui->lineEdit_HurstSRA->text().toDouble();
     int i_StepsSRA = ui->spinBox_CountOfStepsSRA->value(); // кол-во шагов
     N = ui->lineEdit_NumbersSRA->text().toInt(); // 2^n - кол-во сделанных отсчетов
     vector_DeltaTime.resize(N);  // общий массив теперь становится длиной N
     vector_DeltaTime.fill(0);    // заполняем его нулями

     // расчитывем X(0) X(1)
    vector_DeltaTime[0] = 0;  // X(0)
    fprintf(OUT_F,"%f\n",0);

    std::normal_distribution <float> norm_distr(0,1);
    float X_1=0;
    X_1=norm_distr(generator);  // X(1)

    vector_DeltaTime[vector_DeltaTime.size()-1] = X_1;  // X(1)
    for(int i=1;i<i_StepsSRA;i++)   // запускаем цикл по шагам
    {
        int colvo_val = pow(2,i); // кол-во значений при каждом шаге
        // qDebug()<<"-----------------"<<i;
        for(int s=0;s<=colvo_val;s++) // цикл для подсчета всех значений при каждом шаге
        {
           //  qDebug()<<"s = "<<s<<"      "<<s*N/pow(2,i);
            float X_more,X_less;   // инициализируются переменные
            X_less = 0;            // эти переменные являются обязательными для подсчета взятого значения
            X_more = 0;            // одно из них- ближайшее расчитанное в большую сторону, другое - ближайшее расчитанное в меньшую сторону

            // в большую сторону
            for(int p=s*N/pow(2,i);p<=vector_DeltaTime.size()-1;p++)   // цикл проверяет массив от текущего значения в большую сторону до конца массиве
            {
                if(vector_DeltaTime[p]!=0)     // на поиск ближайшего ненулевого значения
                {
                    X_more=vector_DeltaTime[p];    // и записывает его в переменную
                }
            }

            // в меньшую сторону
            for(int q=s*N/pow(2,i);q>=0;q--)       // цикл проверяет массив от текущего значение в меньшую сторону до конца массива
            {
                if(vector_DeltaTime[q]!=0)         // если ближайшее значение не нулевое
                {
                  X_less =vector_DeltaTime[q]; // записывай его в перменную
                }
            }


            double disp =sqrt(1-pow(2,2*d_HurstParametrSRA-2)*ui->lineEdit_DispersionSRA->text().toDouble()/pow(2,i*2*d_HurstParametrSRA));
            std::normal_distribution <float> norm_distrSRA_N(0,disp);
            vector_DeltaTime[s*N/pow(2,i)]=(X_more+X_less)/2+norm_distrSRA_N(generator);
            fprintf(OUT_F,"%f\n",vector_DeltaTime[s*N/pow(2,i)]);    // запись в файл

        }
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

// Кнопка Загрузить из TXT
//void MainWindow::on_pushButton_DownloadFromTXT_clicked()
//{
//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/home",tr("Текстовые файлы (*txt)"));
//    vector_DeltaTime.clear();
//    ui-> tableWidget ->setRowCount(0);
//    QFile inputFile(fileName);
//    i_line = -1;
//     if (inputFile.open(QIODevice::ReadOnly))
//      {
//         QTextStream stream(&inputFile);
//         QString line;
//         do
//         {
//             i_line++;
//             line = stream.readLine();

//         }while (!line.isNull());
//         vector_DeltaTime.resize(i_line);
//         ui->spinBox_parametr_N->setValue(i_line);
//     }
//     inputFile.close();
//  if (inputFile.open(QIODevice::ReadOnly))
//  {
//     i_line = 0;
//     QTextStream stream(&inputFile);
//     QString line;
//      do
//      {
//        line = stream.readLine();
//        if(line.toFloat()!=0)
//        {
//           vector_DeltaTime[i_line] = line.toFloat();
//           i_line++;
//        }
//      }while (!line.isNull());
//  }
//     ui-> tableWidget ->setRowCount(i_line);
//     QTableWidgetItem *newItem = new QTableWidgetItem();
//     newItem->setText(QString::number(0));
//     newItem->setTextAlignment(Qt::AlignHCenter);
//     ui->tableWidget->setItem(0,0,newItem);

//     int i_currentRow = 0;
//     for(int i=0;i<vector_DeltaTime.size();i++)
//       {
//         i_currentRow++;
//         QTableWidgetItem *newItem = new QTableWidgetItem();
//         newItem->setText(QString::number(vector_DeltaTime[i]));
//         newItem->setTextAlignment(Qt::AlignHCenter);
//         ui->tableWidget->setItem(i_currentRow,0,newItem);
//         ui->tableWidget->setCurrentCell(i,0);
//       }
//}



// Building histogram
void MainWindow::drawGraphic()
{
    ui->widget_customplot->clearGraphs();
    ui->tableWidget_basket->setRowCount(20);
    vectordouble_HistData.clear();
    vectordouble_TicksForHist.clear();
    float sd[20];
    float sdd = 2.0;

    for(int i=0;i<20;i++)
    {
       sdd+=0.5;
       sd[i]=sdd;
       vectordouble_TicksForHist<<sdd;

       QTableWidgetItem *newItem = new QTableWidgetItem();
       newItem->setText(QString::number(sdd));
       newItem->setTextAlignment(Qt::AlignHCenter);
       ui->tableWidget_basket->setItem(i,0,newItem);
    }

    for(int j=0;j<20-1;j++)
    {
        float min =sd[j];
        float max =sd[j+1];
        double as=0;

        for(int i=0;i<vector_DeltaTime.size();i++)
        {
            if(min<=vector_DeltaTime[i]&&max>vector_DeltaTime[i])
            {
               as+=1.0;
            }
        }
        vectordouble_HistData<<as;
    }

//  QCPBars *bars_distribution = new QCPBars(ui->widget_customplot->xAxis, ui->widget_customplot->yAxis);
//  bars_distribution->setAntialiased(false); // gives more crisp, pixel aligned bar borders
//  bars_distribution->setPen(QPen(QColor(111, 9, 176).lighter(170)));



    bars_packets->setWidth(0.5);
    bars_packets->setAntialiased(true);
    // set names and colors:
    bars_packets->setName("bars_packets fuels");
    bars_packets->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    bars_packets->setBrush(QColor(111, 9, 176));

    // prepare x axis with country labels:
    ui->widget_customplot->xAxis->setTickLabelRotation(150);
    ui->widget_customplot->xAxis->setLabel(tr("Время, c"));
    // ui->widget_customplot->xAxis->setSubvectordouble_TicksForHist(false);
    ui->widget_customplot->xAxis->setTickLength(0, 5);
    ui->widget_customplot->xAxis->setRange(0, 8);
    ui->widget_customplot->xAxis->setBasePen(QPen(Qt::white));
    ui->widget_customplot->xAxis->setTickPen(QPen(Qt::white));
    ui->widget_customplot->xAxis->grid()->setVisible(true);
    ui->widget_customplot->xAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));
    ui->widget_customplot->xAxis->setTickLabelColor(Qt::white);
    ui->widget_customplot->xAxis->setLabelColor(Qt::white);

    // prepare y axis:
    ui->widget_customplot->yAxis->setRange(0, 200);
    ui->widget_customplot->yAxis->setPadding(5); // a bit more space to the left border
    ui->widget_customplot->yAxis->setLabel(tr("Кол-во пакетов"));
    ui->widget_customplot->yAxis->setBasePen(QPen(Qt::white));
    ui->widget_customplot->yAxis->setTickPen(QPen(Qt::white));
    ui->widget_customplot->yAxis->setSubTickPen(QPen(Qt::white));
    ui->widget_customplot->yAxis->grid()->setSubGridVisible(true);
    ui->widget_customplot->yAxis->setTickLabelColor(Qt::white);
    ui->widget_customplot->yAxis->setLabelColor(Qt::white);
    ui->widget_customplot->yAxis->grid()->setPen(QPen(QColor(130, 130, 130), 0, Qt::SolidLine));
    ui->widget_customplot->yAxis->grid()->setSubGridPen(QPen(QColor(130, 130, 130), 0, Qt::DotLine));


    // например
    // отсортировали от меньшего к старшему
    // qSort(vector_DeltaTime.begin(), vector_DeltaTime.end());

     bars_packets->setData(vectordouble_TicksForHist,vectordouble_HistData);
     //setup legend:
//    ui->widget_customplot->legend->setVisible(true);
//    ui->widget_customplot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignHCenter);
//    ui->widget_customplot->legend->setBrush(QColor(255, 255, 255, 100));
//    ui->widget_customplot->legend->setBorderPen(Qt::NoPen);
//    QFont legendFont = font();
//    legendFont.setPointSize(10);
//    ui->widget_customplot->legend->setFont(legendFont);

    ui->widget_customplot->replot();
}

void MainWindow::drawGraphicNoise()
{
    QVector<double> d_x(vector_DeltaTime.size()),d_y(vector_DeltaTime.size());
    for(int i=0;i<vector_DeltaTime.size();i++)
    {
        d_x[i] =(double)i;
        d_y[i] =(double)vector_DeltaTime[i];
    }

    double minY = d_y[0],maxY = d_y[0];
    for(int i = 0;i<vector_DeltaTime.size();i++)
    {
        if (d_y[i]<minY) minY = d_y[i];
        if (d_y[i]>maxY) maxY = d_y[i];
    }

       ui->widget_customplotNoise->clearGraphs();
       ui->widget_customplotNoise->addGraph();
       ui->widget_customplotNoise->xAxis->setRange(d_x[0],d_x[vector_DeltaTime.size()-1]);
       ui->widget_customplotNoise->yAxis->setRange(minY,maxY);
       ui->widget_customplotNoise->graph(0)->setData(d_x,d_y);
       ui->widget_customplotNoise->graph(0)->setPen(QPen(Qt::red));

//        ui->widget_customplotNoise->xAxis->setLabel("x");
//        ui->widget_customplotNoise->yAxis->setLabel("y");
       ui->widget_customplotNoise->replot();
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    qDebug()<<"index.row = "<<index.row();
    qDebug()<<"count widget = "<<ui->stackedWidget_ParametersOfDistribution->count();
    ui->stackedWidget_ParametersOfDistribution->setCurrentIndex(index.row());

//    if(index.row()==4)
//    {
//       ui->spinBox_parametr_N->setEnabled(false);
//    }
//    else if(!ui->spinBox_parametr_N->isEnabled())
//    {
//        ui->spinBox_parametr_N->setEnabled(true);
//    }

}

void MainWindow::on_pushButton_GenerateTime_clicked()
{
    GenerateTime(ui->stackedWidget_ParametersOfDistribution->currentIndex()+1);
}


void MainWindow::on_toolButton_SaveNoise_clicked()
{
    bool b_IsItSaveWithoutAnyTroubles = SaveGraphicsAsImage(ui->widget_customplotNoise);
    if(b_IsItSaveWithoutAnyTroubles)
    {
        qDebug(logDebug)<<"Save noise graphic succesfully";
    }

}

void MainWindow::on_toolButton_SaveHist_clicked()
{
    bool b_IsItSaveWithoutAnyTroubles = SaveGraphicsAsImage(ui->widget_customplot);
    if(b_IsItSaveWithoutAnyTroubles)
    {
          qDebug(logDebug)<<"Save histogramm graphic succesfully";
    }
}

bool MainWindow::SaveGraphicsAsImage(QCustomPlot *widget_Graphic)
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить график как картинку"),
                               "/home/save.png",
                               tr("*.png;; *. bpm;; *.jpg"));
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly|QFile::WriteOnly))
        QMessageBox::warning(this, "Ошибка", tr("Не получается сохранить файл."));

    if(fileName.endsWith("png"))
      return widget_Graphic->savePng(fileName,0,0,1.0,-1);
    else if(fileName.endsWith("bmp"))
      return widget_Graphic->saveBmp(fileName,0,0,1.0);
    else if(fileName.endsWith("jpg"))
      return widget_Graphic->saveJpg(fileName,0,0,1.0,-1);
    else
        return false;
}

void MainWindow::on_toolButton_SettingsOfHist_clicked()
{
    colorpanel_histogram->show();
    colorpanel_histogram->activateWindow();
}

void MainWindow::on_toolButton_SettingsOfNoise_clicked()
{
    colorpanel_noise->show();
    colorpanel_noise->activateWindow();
}


void MainWindow::on_toolButton_grab_handNoise_toggled(bool checked)
{
    ui->toolButton_CrossCursorXY->setChecked(!checked);
    if(checked)
    {
       ui->widget_customplotNoise->setCursor(Qt::OpenHandCursor);
       ui->widget_customplotNoise->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
       connect(ui->widget_customplotNoise,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(slot_clickedHandClose(QMouseEvent*)));
       connect(ui->widget_customplotNoise,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(slot_clickedHandOpen(QMouseEvent*)));
    }
    else
    {
        ui->widget_customplotNoise->setInteractions(0);
        disconnect(ui->widget_customplotNoise,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(slot_clickedHandOpen(QMouseEvent*)));
        disconnect(ui->widget_customplotNoise,SIGNAL(mouseRelease(QMouseEvent*)),this,SLOT(slot_clickedHandOpen(QMouseEvent*)));
    }
}

void MainWindow::slot_clickedHandOpen(QMouseEvent *event)
{
    Q_UNUSED(event)
    ui->widget_customplotNoise->setCursor(Qt::OpenHandCursor);
}

void MainWindow::slot_clickedHandClose(QMouseEvent* event)
{
    Q_UNUSED(event)
    ui->widget_customplotNoise->setCursor(Qt::ClosedHandCursor);
}

void MainWindow::on_toolButton_CrossCursorXY_toggled(bool checked)
{
   ui->toolButton_grab_handNoise->setChecked(!checked);

   if(checked)
   {
      ui->widget_customplotNoise->setCursor(Qt::CrossCursor);
      connect(ui->widget_customplotNoise,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(slot_clickedPointNoise(QMouseEvent*)));
   }
   else
   {
       disconnect(ui->widget_customplotNoise,SIGNAL(mousePress(QMouseEvent*)),this,SLOT(slot_clickedPointNoise(QMouseEvent*)));
   }
}

void MainWindow::on_toolButton_resetHistogram_clicked()
{
    drawGraphic();
}


void MainWindow::on_spinBox_CountOfSteps_valueChanged(int arg1)
{
    if(arg1 !=3)
    {
      ui->lineEdit_NumbersRMD->setText(QString::number(pow(2,arg1-1)));
    }
    else
    {
      ui->lineEdit_NumbersRMD->setText(QString::number(arg1-1));
    }
}


void MainWindow::on_spinBox_CountOfStepsSRA_valueChanged(int arg1)
{
   // тут остановился
    if(arg1 !=3)
    {
      ui->lineEdit_NumbersSRA->setText(QString::number(pow(2,arg1-1)));
    }
    else
    {
      ui->lineEdit_NumbersSRA->setText(QString::number(arg1-1));
    }
}

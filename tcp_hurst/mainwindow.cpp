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

    const char* outname;
    outname = "HurstBurst_valuesOfdistribution.txt";

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
 else if(i_WhatGen==5)
 {
    // инициализируем переменные, которые ввел пользователь
    double d_StandartDeviation = sqrt(ui->lineEdit_DispersionRMD->text().toDouble());
    double d_HurstParametr = ui->lineEdit_HurstRMD->text().toDouble();
    int i_Steps = ui->spinBox_CountOfSteps->value();
    N = ui->lineEdit_NumbersRMD->text().toInt();

    vector_DeltaTime.resize(ui->lineEdit_NumbersRMD->text().toInt());
    vector_DeltaTime.fill(0);

    // шаг 1
   std::normal_distribution <float> norm_distr(0,d_StandartDeviation);
   float X_1;

   for(;;)
   {
        X_1 = norm_distr(generator);
        if((X_1>0)&&(X_1<1))    // я так понимаю - это нормировка
            break;
   }
   vector_DeltaTime[vector_DeltaTime.size()-1]=X_1;

   qDebug()<<"[vector_DeltaTime.size()-1] = " <<vector_DeltaTime[vector_DeltaTime.size()-1];

//     шаг 2
    float X_12;
    double disp =sqrt(pow(0.5,2*d_HurstParametr)*(1-pow(2.0,2*d_HurstParametr-2))*pow(d_StandartDeviation,2));
    std::normal_distribution <float> norm_distr_2(0,disp);

    for(;;)
    {
        X_12 = 0.5*X_1+0.5*norm_distr_2(generator);
        if((X_12 >0)&&(X_12 <1))
                break;
    }
    vector_DeltaTime[vector_DeltaTime.size()/2]=X_12;


    // тут должны быть шаги с 3 -по n
    int i_val = pow(2,i_Steps);
    for(int d =1;d<i_Steps;d++)// цикл для всех итераций
    {
       float f_values[(int)pow(2,d)];        // создание массива переменных для вычисления значений на данном шаге
       double disp3 =sqrt(pow(1/pow(2,d),2*d_HurstParametr)*(1-pow(2.0,2*d_HurstParametr-2))*pow(d_StandartDeviation,2));
       std::normal_distribution <float> norm_distrStep(0,disp3);
       for(int i=0;i<pow(2,d);i++)  //  в нем i - это кол-во переменных генерируемых на этом шаге
       {
            int i_FindFirstThatIsNotNull;   // создаем переменную, которая укажет номер - в массиве где не ноль
        // далее устанавливаем для переменной - ближней к нулю
           if(i==0) // если итерация первая
           {
                for(int g=0;g<=vector_DeltaTime.size()/2;g++)
                {
                    if(vector_DeltaTime[g]!=0)
                    {
                        i_FindFirstThatIsNotNull=g; // нашли номер ненулевого массива - выходим
                        for(;;) // бахаем в элемент массива стоящей на половину от того ненулеовго число
                        {
                              vector_DeltaTime[i_FindFirstThatIsNotNull/2] = 0.5*vector_DeltaTime[i_FindFirstThatIsNotNull]+0.3535*norm_distrStep(generator);
                              if((vector_DeltaTime[i_FindFirstThatIsNotNull/2] >0)&&(vector_DeltaTime[i_FindFirstThatIsNotNull/2] <1))
                              {
                                   i_val--;
                                   break;
                              }
                        }
                    }
                }
            }
          int countFinding = 0;
          int one,two;

          for(int i=i_FindFirstThatIsNotNull;i<vector_DeltaTime.size();i++)
           {
               if(vector_DeltaTime[i]!=0)
               {
                   countFinding++;
                   if(countFinding==1)
                   {
                        one= i;
                   }

                   if(countFinding==2)
                   {
                        two =i;
                        vector_DeltaTime[(two+one+1)/2] = 0.5*(vector_DeltaTime[one]+vector_DeltaTime[two])+0.3535*norm_distrStep(generator);
                        qDebug()<<"Result = "<<(two+one+1)/2<<"  " <<vector_DeltaTime[(two+one+1)/2];
                        break;
                   }

               }
           }
       }
    }

    // шаг 3
    float X_14,X_34;
    double disp3 =sqrt(pow(0.3535,2*d_HurstParametr)*(1-pow(2.0,2*d_HurstParametr-2))*pow(d_StandartDeviation,2));
    std::normal_distribution <float> norm_distr_3(0,disp3);
    for(;;)
    {
        X_14 = 0.5*X_1+0.3535*norm_distr_3(generator);
        if((X_14 >0)&&(X_14 <1))
           break;
    }

    for(;;)
    {
        X_34 = 0.5*(X_12+X_1)+0.3535*norm_distr_3(generator);
        if((X_34 >0)&&(X_34 <1))
                break;
    }



//     // шаг 3
//     float X_1div4,X_3div4;
//     X_1div4 = 0.5*X_1div2+


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
    ui->stackedWidget_ParametersOfDistribution->setCurrentIndex(index.row());
    if(index.row()==4)
    {
       ui->spinBox_parametr_N->setEnabled(false);
    }
    else if(!ui->spinBox_parametr_N->isEnabled())
    {
        ui->spinBox_parametr_N->setEnabled(true);
    }

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

void MainWindow::on_pushButton_CalculateHurst_clicked()
{
    QVector<float> vector_CalculateDeltaTimeNorm;
    vector_CalculateDeltaTimeNorm.resize(vector_DeltaTime.size());
    QVectorIterator<float> fvect(vector_DeltaTime);
    float sum;
    while(fvect.hasNext())
    {
        sum+=fvect.next();
    }
    float f_averagevalue= sum/vector_DeltaTime.size();

    for(int i=0;i<vector_DeltaTime.count();i++)
    {
      vector_CalculateDeltaTimeNorm[i]=vector_DeltaTime[i]-f_averagevalue;
    }

     double mlarge =floor(vector_CalculateDeltaTimeNorm.size()/5);
//   qDebug()<<" logspace(0,log10(mlarge),50) = " <<logspace(0,log10(mlarge),50) ;
     //QVector <float> s = logspace(1,mlarge,50);

//     std::sort( s.begin(), s.end() );
//     s.erase( std::unique(s.begin(), s.end() ),s.end() );//remove duplicates

   //  n = length(M);  % вычисляем длину получившегося вектора
//     int cut_min = ceil(s.size()/10);   // делим полчившуюся длину вектора на 10 - округляем в большую сторону
//     int cut_max = floor(6*s.size()/10);

//     qDebug()<<cut_min<<"   "<<cut_max;
}

// проообраз функции из матлаба
// спизжено отсюда: https://www.codeproject.com/Questions/188926/Generating-a-logarithmically-spaced-numbers
//QVector<float> MainWindow::logspace(double i_min,double i_max,double i_count)
//{
//      double logMin = log10(i_min);
//      double logMax = log10(i_max);
//      double delta = (logMax - logMin)/i_count;
//      double accDelta = 0;
//      QVector <float> vector_result;
//      vector_result.resize(i_count);
//      for (int i = 0;i<i_count; ++i)
//      {
//        vector_result[i] = (float)pow(2.721828,logMin + accDelta);
//        //v[i] = (float) Math.pow(logarithmicBase, logMin + accDelta);
//        qDebug()<<vector_result[i];
//        accDelta += delta;// accDelta = delta * i
//      }
//      return vector_result;
//}

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


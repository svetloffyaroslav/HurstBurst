// ======================================================================
// pult.cpp
// ======================================================================
//           This file is a part of the panel simulator for
//                           "Ornament-POO"
// ======================================================================
//  Copyright (c) 2017 by Svetlov Yaroslav
//
//  Email  : svetlov.yaroslav@gmail.com
//  vk.com : https://vk.com/id52304190
//
//  Description
//  ---------------
//  This is the main modul of programm. There has been described all algorithms
//  of interface, connection with servers socket.
//
//  ---------------
//  Codec:  UTF-8
//
//  Last changes: 18.09.2017
// ======================================================================


#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("HURST BURST | TCP client");
    TcpSocket_One= new QTcpSocket(this);                                                                        // создается объект сокета
    Timer_BetweenPacket = new QTimer(this);                                                                     // создается объект таймера
    ui->lineEdit_IPAdres->setInputMask("000.000.000.000;");                                                     // в строку ввода IP адреса устанавливается маска
    connect(TcpSocket_One,  SIGNAL (connected()),    SLOT(slot_TcpSocket_OneConnected()));                      // connected() -  вызывается при создании соединения c сервером;
    connect(TcpSocket_One,  SIGNAL (readyRead()),    SLOT(slot_TcpSocket_OneReadyRead()));                      // readyRead() - при готовности предоставить данные для чтения;
    connect(TcpSocket_One, SIGNAL(disconnected()),  SLOT(slot_TcpSocket_OneDisconnected()));                    // disconnected  - при отключении сокета от сервера;
    connect(TcpSocket_One, SIGNAL(error(QAbstractSocket::SocketError)),
             this,                                  SLOT(slot_TcpSocket_OneError(QAbstractSocket::SocketError))  // error(QAbstractSocket::SocketError) - при возникновении ошибки
            );
    connect(Timer_BetweenPacket,SIGNAL(timeout()),SLOT(slot_timeoutTimer_BetweenPacket()));                       // подключаем таймер к слоту
    i_numberPacket = 0;                                                                                           // устанавливаем кол-во переданных пакетов
    ui->listWidget->setCurrentRow(0);

}

void MainWindow::slot_TcpSocket_OneConnected()
{
    /* slot */
    ui->statusBar->showMessage("Подключился",3000);                                                             // вывод информации в строку статуса на 3 с
    ui->groupBox_TimeRow->setEnabled(true);                                                                     // группа элементов "Временной ряд" - становится доступной
    ui->groupBox_Transmission->setEnabled(true);                                                                // группа элементов передача, становится доступным
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Слот, срабатывающий при поступлении данных от сервера~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::slot_TcpSocket_OneReadyRead()
{
    qDebug()<<"Готовность к чтению";                                                                            // Если от сервера- что-то придет
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Слот, срабатывающий при Отключении от сервера~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::slot_TcpSocket_OneDisconnected()
{
    ui->statusBar->showMessage("Отключился",3000);                                         // При отключении от сервера на 3 сек. показывается сообщение в строке статуса
    ui->groupBox_TimeRow->setEnabled(false);                                               // группы элементов становятся недоступными
    ui->groupBox_Transmission->setEnabled(false);
    Timer_BetweenPacket->stop();                                                           // таймер для пересылки пакетов останавливается
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Слот, срабатывающий при Возникновении ошибки~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
    ui->statusBar->showMessage(strSystemError,3000);                            // выводится сообщение в строку статуса на 3 сек
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_ConnectOrDisconnect_toggled(bool checked)
{
     if(checked)
    {
         /*Если кнопка  находится в состоянии "зажатия"*/

        QString string_IPAdress = ui->lineEdit_IPAdres->text();                          // строчная переменная заполняется введенным пользователем IP-адресом в строку "IP-адрес:"
        quint16 string_Port = ui->lineEdit_Port->text().toUShort();                      // информация в строке "Порт" заполняет
        QHostAddress address(string_IPAdress);                                           // класс обеспечивающий IP адрес
        TcpSocket_One -> connectToHost (address,string_Port);                            // подключай сокеты по этому адрессу в соответствующие порты
        ui->pushButton_ConnectOrDisconnect->setText("Отключить");                        // переименовывается кнопка
    }
    else
    {
        /*Если кнопка  находится в состоянии "отжатия"*/

        TcpSocket_One  -> disconnectFromHost();                                          // закрытие сокета
        ui->pushButton_ConnectOrDisconnect->setText("Подключить");                       // кнопке устанавливается имя
    }
}



void MainWindow::on_pushButton_Transmit_clicked()
{
    ui->progressBar->setValue(0);                                                   // строке прогресса устанавливается начальное значение
    ByteArray_Send.clear();                                                         // очищается байтовый массив для передачи

    ByteArray_Send.resize(ui->spinBox_SizeOfPacket->value());                       // байтовому массиву для передачи задается размер, указанный в spinBox "Размер пакетов"
    ByteArray_Send.fill('x');                                                       // заполняется заполняется буквой

    /* Посылка первого пакета */
    sendToSocket();

    i_numberPacket = 0;
    StartTimerDelta(i_numberPacket);                                                // запуск таймера
    ui->progressBar->setMaximum(ui->spinBox_parametr_N->value());
}

void MainWindow::StartTimerDelta(int i)
{
    qDebug()<<" i =" <<i;
    if(i>ui->spinBox_parametr_N->value())                               // если пришедший аргумент превышает допустимый максимальное число пакетов
    {
        Timer_BetweenPacket->stop();                                    // таймер останавливается
    }
    else
    {
        Timer_BetweenPacket->start((int)(vector_DeltaTime[i]));         // вызывается таймер со значением i-го элемента массива в качестве аргумента
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Слот, срабатывающий при окончании таймера~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::slot_timeoutTimer_BetweenPacket()
{
    i_numberPacket++;                                                       // увеличивается число пакетов
    ui->progressBar->setValue(i_numberPacket);                              // сдвигается заполнение строки прогресса
    if(i_numberPacket<ui->spinBox_parametr_N->value())                      // если кол-во пакетов не превышает указнных в spinbox
    {
        sendToSocket();                                                       // посылай порту
        StartTimerDelta(i_numberPacket);                                    // Вызывается функция запуска таймера
    }
    else                                                                    // если кол-во пакетов превышает указнные
    {
       Timer_BetweenPacket->stop();                                         // таймер останавливается
    }
}



void MainWindow::sendToSocket()
{
    qDebug()<<"write";
    TcpSocket_One->write(ByteArray_Send);                                           // запись массива в сокет
    TcpSocket_One->flush();                                                         // "выбрасывание" данных
}


void MainWindow::on_action_SaveTime_triggered()
{



}

void MainWindow::GenerateTime(int i_WhatGen)
{

    ui-> tableWidget ->setRowCount(0);                                              // в таблице - стираются предыдущие значения
    vector_DeltaTime.clear();                                                       // очищается массив переменных для времени

    /* Параметры распределения Парето */
    float f_a = ui->lineEdit_parametr_a->text().toFloat();                          //  значение введеное в строку "Параметр формы, a:", "Граничный параметр, k:" записывается во float переменные
    float f_k = ui->lineEdit_parametr_k->text().toFloat();

    /* Параметры экспоненциального распределения */
    float f_L = ui->lineEdit_parametr_L->text().toFloat();                          // значение в строке "Параметр" записывается в float

    /* Кол-во пакетов*/
    int N = ui->spinBox_parametr_N->value();

    vector_DeltaTime.resize(N);                                                     // задается размер массива в соответствии с кол-вом байт

    char* outname;                                                                  // указатель на char задания имени файла куда будут записываться сгенерированные значения


    switch (i_WhatGen)
    {
        case 1: outname = "HurstBurst_pareto.txt"; break;
        case 2: outname = "HurstBurst_exp.txt"; break;
        case 3: outname = "HurstBurst_weibull.txt"; break;
        case 4: outname = "HurstBurst_lognorm.txt"; break;
        default:                                 break;
    }

    FILE* OUT;                                                                  // объект файла
    OUT = fopen(outname, "wt");                                                 //  открытия файла для записи

    /*Псевдослучайная величина с равномерным распределением*/
    const int range_from = 0;// N(0,1)
    const int range_to = 1;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

    //std::
    // std::random_device                      rand_dev;
    std::mt19937                            generator(seed);              // алгоритм генерирования
    std::uniform_real_distribution<float>   distr(range_from, range_to);        // тип распределения - равномерный float от 0 до 1


    float f_tens = 0;                                                           // десятикратное увеличение
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
        fprintf(OUT, "%f\n",y);
    }
 }
 else if(i_WhatGen==2)
 {
     for(int s= 0;s<N;s++)
     {
         float r = distr(generator);
         float y =  - log(r)/f_L;
         vector_DeltaTime[s] = y*f_tens;
         fprintf(OUT, "%f\n",y);
     }
 }
 else if(i_WhatGen==3)
 {
     std::weibull_distribution<float> weibull_distr(ui->lineEdit_weibull_a->text().toDouble(),ui->lineEdit_weibull_b->text().toDouble());
     for(int s= 0;s<N;s++)
     {
         float r = weibull_distr(generator);
         vector_DeltaTime[s]=r*f_tens;
         fprintf(OUT,"%f\n",r);
     }
 }
 else if(i_WhatGen==4)
 {
     std::lognormal_distribution<float> lognorm_distr(ui->lineEdit_lognorm_m->text().toDouble(),ui->lineEdit_lognorm_s->text().toDouble());
     for(int s= 0;s<N;s++)
     {
          float r = lognorm_distr(generator);
          vector_DeltaTime[s]=r*f_tens;
          fprintf(OUT,"%f\n",r);
     }
 }

 fclose(OUT);
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
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Кнопка "ЗАГРУЗИТЬ ИЗ TXT"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    ui->stackedWidget_ParametersOfDistribution->setCurrentIndex(index.row());
}

void MainWindow::on_pushButton_GenerateTime_clicked()
{
    GenerateTime(ui->stackedWidget_ParametersOfDistribution->currentIndex()+1);                                               // вызывается функция осущетсвляющая генерацию  - 1 значит распределение Парето
}


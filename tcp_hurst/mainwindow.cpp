#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Слот, срабатывающий при подключении к серверу~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::slot_TcpSocket_OneConnected()
{
    ui->statusBar->showMessage("Подключился",3000);                                     // вывод информации в строку статуса на 3 с
    ui->groupBox_TimeRow->setEnabled(true);                                             // группа элементов "Временной ряд" - становится доступной
    ui->groupBox_Transmission->setEnabled(true);                                        // группа элементов передача, становится доступным
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Слот, срабатывающий при поступлении данных от сервера~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::slot_TcpSocket_OneReadyRead()
{
    qDebug()<<"Готовность к чтению";                                                      // Если от сервера- что-то придет
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Удаление интерфеса при закрытии приложения~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MainWindow::~MainWindow()
{
    delete ui;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Кнопка "Подключить"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_pushButton_ConnectOrDisconnect_toggled(bool checked)
{
     ui->lineEdit_Port->setDisabled(checked);                                           // при подключении группы элементов становятся доступными
     ui->lineEdit_IPAdres->setDisabled(checked);
     if(checked)
    {
        QString string_IPAdress = ui->lineEdit_IPAdres->text();                          // строчная переменная заполняется введенным пользователем IP-адресом в строку "IP-адрес:"
        quint16 string_Port = ui->lineEdit_Port->text().toUShort();                      // информация в строке "Порт" заполняет
        QHostAddress address(string_IPAdress);
        TcpSocket_One -> connectToHost (address,string_Port);
        ui->pushButton_ConnectOrDisconnect->setText("Отключить");
    }
    else
    {
        TcpSocket_One  -> disconnectFromHost();
        ui->pushButton_ConnectOrDisconnect->setText("Подключить");
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Кнопка "Передать"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_pushButton_Transmit_toggled(bool checked)
{
    ui->progressBar->setValue(0);

    ByteArray_Send.clear();
    ByteArray_Send.resize(ui->spinBox_SizeOfPacket->value());
    ByteArray_Send.fill('x');

    TcpSocket_One->write(ByteArray_Send);
    TcpSocket_One->flush();

    StartTimerDelta(i_numberPacket);

    ui->progressBar->setMaximum(ui->spinBox_parametr_N->value());
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
        TcpSocket_One->write(ByteArray_Send);
        TcpSocket_One->flush();
        StartTimerDelta(i_numberPacket);
    }
    else
    {
       Timer_BetweenPacket->stop();
    }
}

void MainWindow::on_action_SaveTime_triggered()
{



}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Кнопка "ГЕНЕРАЦИЯ"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::on_pushButton_GenPareto_clicked()
{
    ui-> tableWidget ->setRowCount(0);
    vector_DeltaTime.clear();
    float f_a = ui->lineEdit_parametr_a->text().toFloat();
    float f_k = ui->lineEdit_parametr_k->text().toFloat();
    int N = ui->spinBox_parametr_N->value();
    vector_DeltaTime.resize(N);
    char* outname = "HurstBurst_pareto.txt";  // имя тектового файла, куда попадут значения

    FILE* OUT;
    OUT = fopen(outname, "wt");
    const int range_from = 0;
    const int range_to = 1;

    std::random_device                      rand_dev;
    std::mt19937                            generator(rand_dev());
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

    for(int s= 0;s<N;s++)
    {
        // float r = (float)rand()/RAND_MAX;
        float r = distr(generator);
        float y = f_k/pow(1-r,1.0/f_a);
        vector_DeltaTime[s] = y*f_tens;
        fprintf(OUT, "%f\n",y);
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
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------



void MainWindow::on_pushButton_GenExp_clicked()
{
     ui-> tableWidget ->setRowCount(0);
     vector_DeltaTime.clear();
     float f_L = ui->lineEdit_parametr_L->text().toFloat();
    int N = ui->spinBox_parametr_N->value();

     vector_DeltaTime.resize(N);
     char* outname = "HurstBurst_exp.txt";  // имя тектового файла, куда попадут значения

     FILE* OUT;
     OUT = fopen(outname, "wt");
     const int range_from = 0;
     const int range_to = 1;

     std::random_device                      rand_dev;
     std::mt19937                            generator(rand_dev());
     std::uniform_real_distribution<float>   distr(range_from, range_to);

     float f_tens = 0;
     if(ui->radioButton_1X_exp->isChecked())
     {
         f_tens = 1.0;
     }
     else if(ui->radioButton_X10_exp->isChecked())
     {
         f_tens = 10.0;
     }
     else if(ui->radioButton_X100_exp->isChecked())
     {
         f_tens = 100.0;
     }

     for(int s= 0;s<N;s++)
     {
         // float r = (float)rand()/RAND_MAX;
         float r = distr(generator);
         float y =  - log(r)/f_L;
         vector_DeltaTime[s] = y*f_tens;
         fprintf(OUT, "%f\n",y);
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

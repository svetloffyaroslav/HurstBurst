#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TcpSocket_One= new QTcpSocket(this);         // создается сокет
    Timer_BetweenPacket = new QTimer(this);



    ui->lineEdit_IPAdres->setInputMask("000.000.000.000;");



    connect(TcpSocket_One,  SIGNAL (connected()),    SLOT(slot_TcpSocket_OneConnected()));                                           // соединение сигнала connected() - который вызывается при создании соединения со слотом выводящим об этом информацию
    connect(TcpSocket_One,  SIGNAL (readyRead()),    SLOT(slot_TcpSocket_OneReadyRead()));                                          //  сокет отправляет сигнал readyRead() при готовности предоставить данные для чтения
    connect(TcpSocket_One, SIGNAL(disconnected()),  SLOT(slot_TcpSocket_OneDisconnected()));                            //
    connect(TcpSocket_One, SIGNAL(error(QAbstractSocket::SocketError)),
             this,                                  SLOT(slot_TcpSocket_OneError(QAbstractSocket::SocketError))       //
            );

    connect(Timer_BetweenPacket,SIGNAL(timeout()),SLOT(slot_timeoutTimer_BetweenPacket()));


    i_numberPacket = -1;


}


void MainWindow::slot_TcpSocket_OneConnected()
{
    ui->statusBar->showMessage("Подключился",3000);

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
     ui->lineEdit_Port->setDisabled(checked);
     ui->lineEdit_IPAdres->setDisabled(checked);

     if(checked)
    {

        QString string_IPAdress = ui->lineEdit_IPAdres->text();
        quint16 string_Port = ui->lineEdit_Port->text().toUShort();

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



void MainWindow::on_pushButton_Transmit_toggled(bool checked)
{

    if(checked)
    {
        Timer_BetweenPacket->start(ui->spinBox_DelayTime->value());
        ui->progressBar->setValue(0);
        i_numberPacket =0;

        ui->pushButton_Transmit->setText("Остановить");
          b_firstSend = true;
    }
    else
    {
        Timer_BetweenPacket->stop();
        ui->pushButton_Transmit->setText("Передать");
        b_firstSend = false;
        TcpSocket_One->flush();
    }
    ui->progressBar->setMaximum(ui->spinBox_CountOfSize->value());

}

void MainWindow::slot_timeoutTimer_BetweenPacket()
{
    if(b_firstSend)
    {

        int i_SizeOfPacket = ui->spinBox_SizeOfPacket->value();
        int i_CountOfSize = ui->spinBox_CountOfSize->value();


        QByteArray FirstFrame;
        QDataStream stream_ToFirstFrame(&FirstFrame,QIODevice::WriteOnly);
        stream_ToFirstFrame.setByteOrder(QDataStream::LittleEndian);
        stream_ToFirstFrame.setFloatingPointPrecision(QDataStream::SinglePrecision);
        stream_ToFirstFrame<<(quint8)0x42<<i_SizeOfPacket<<i_CountOfSize;


        qDebug()<<"soo"<<FirstFrame.toHex();

        TcpSocket_One->write(FirstFrame);
        TcpSocket_One->flush();

        b_firstSend =false;


    }
    else
    {
        i_numberPacket++;
        ui-> tableWidget ->setRowCount(i_numberPacket);

        ByteArray_Send.clear();

        ui->progressBar->setValue(i_numberPacket);
        ByteArray_Send.resize(ui->spinBox_SizeOfPacket->value());
        ByteArray_Send.fill('x');

        if(i_numberPacket==1)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setText(QString::number(0));
            newItem->setTextAlignment(Qt::AlignHCenter);
            ui->tableWidget->setItem(i_numberPacket-1,0,newItem);
            i_previousTime = QTime::currentTime().msecsSinceStartOfDay();


            TcpSocket_One->write(ByteArray_Send);
            TcpSocket_One->flush();
        }
        else
        {


                float f_time = (float)(QTime::currentTime().msecsSinceStartOfDay()-i_previousTime)/1000;
                QTableWidgetItem *newItem = new QTableWidgetItem();
                newItem->setText(QString::number(f_time));
                newItem->setTextAlignment(Qt::AlignHCenter);
                ui->tableWidget->setItem(i_numberPacket-1,0,newItem);
                ui->tableWidget->setCurrentCell(i_numberPacket-1,0);

                i_previousTime = QTime::currentTime().msecsSinceStartOfDay();

                TcpSocket_One->write(ByteArray_Send);
                TcpSocket_One->flush();

        if(i_numberPacket>ui->spinBox_CountOfSize->value()-1)
        {
            Timer_BetweenPacket       -> stop();
            ui -> pushButton_Transmit -> setChecked(false);
            b_firstSend = true;
            i_numberPacket = 0;
        }
      }
    }
}

void MainWindow::on_action_SaveTime_triggered()
{

    QString fileName  = QFileDialog::getSaveFileName(this,tr("Сохранить отправленные сообщения КС"),"/home/Time_Client",tr("Текстовый файл(*.txt)"));              // получай имя файла в строковую переменную
    QFile  file(fileName);
    if(!file.open(QIODevice::WriteOnly|QFile::WriteOnly))                                                           // если файл не получается открыть для записи
    {
          QMessageBox::warning(this,"Файл не сохранен",tr("Ошибка в сохранении файла: %1.").arg(file.errorString()));                                       // показывай сообщение об ошибке
    }
    else
    {
        QTextStream data( &file );
        QStringList strList;                // лист из стринговых переменных



        for( int r = 0; r < ui->tableWidget->rowCount(); ++r )
        {
                    strList.clear();
                    strList<<"\" "+ui->tableWidget->item( r, 0 )->text()+"\" " ;                                                                           // цикл от нуля до максимального количества строк
                     data << strList.join( ";" ) << "\n";
        }


    }

}

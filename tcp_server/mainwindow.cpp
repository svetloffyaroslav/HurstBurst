#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include <QHostAddress>
#include<QNetworkInterface>
#include<QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TcpServer_One = new QTcpServer (this);
    i_numberOfPackets =0;
    i_numberOfTruePackets = 0;
    f_AvirTime = 0.0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_ServerStart_clicked()
{
    if (!TcpServer_One->listen(QHostAddress::Any, ui->lineEdit_Port->text().toInt()))
    {
            QMessageBox::critical(0,
                                  "System Server Error",
                                  "Unable to start the server:"
                                  + TcpServer_One->errorString());
                                  TcpServer_One->close();
            return;
    }

    foreach(const QHostAddress &address,QNetworkInterface::allAddresses())
    {
        if(address.protocol()==QAbstractSocket::IPv4Protocol&&address!= QHostAddress::LocalHost)
        {
            ui->label_IP_Adress->setText(address.toString());
        }
    }
    connect(TcpServer_One, SIGNAL(newConnection()),
             this,         SLOT(slotTcpServer_OneConnection()));
}


/*virtual*/ void MainWindow::slotTcpServer_OneConnection()
{

     TcpServerSocket_One = TcpServer_One->nextPendingConnection();

    connect(TcpServerSocket_One, SIGNAL(disconnected()),
             this              , SLOT(slot_TcpServer_OneClientIsDisconnect())
            );

    connect(TcpServerSocket_One, SIGNAL(readyRead()),
            this,          SLOT(slot_TcpServer_OneClientReadyRead())
           );

    ui->statusBar->showMessage("Связь установлена",3000);
}

void MainWindow::slot_TcpServer_OneClientIsDisconnect()
{
    ui->statusBar->showMessage("Произошло отключение",3000);
}





void MainWindow::slot_TcpServer_OneClientReadyRead()
{
    i_numberOfPackets++;

    qDebug()<<"Пришло пакетов"<<i_numberOfPackets;

//    QByteArray ByteArray_OneFrame;
//    ByteArray_OneFrame.clear();
//    ByteArray_OneFrame= TcpServerSocket_One->readAll();

//    if(ByteArray_OneFrame.size()==9)
//    {
//           quint8 q8_check;
//           int i_sizeOfPacket;
//           int i_countOfPacket;

//           QDataStream stream_OneFrame(&ByteArray_OneFrame,QIODevice::ReadOnly);
//           stream_OneFrame.setByteOrder(QDataStream::LittleEndian);
//           stream_OneFrame.setFloatingPointPrecision(QDataStream::SinglePrecision);
//           stream_OneFrame>>q8_check>>i_sizeOfPacket>>i_countOfPacket;



//           if(q8_check==0x42)
//           {
//                ui->progressBar->setValue(0);
//                ui->tableWidget->setRowCount(0);
//                ui->label_CountOfPacket->setText(QString::number(i_countOfPacket));
//                ui->label_sizeOfPacket->setText(QString::number(i_sizeOfPacket));
//                ui->progressBar->setMaximum(i_countOfPacket);
//                i_numberOfTruePackets = 0;
//                f_AvirTime = 0.0;
//           }
//           else
//           {
//              return;
//           }

//     }
//    else
//    {
//                i_numberOfTruePackets++;
//                ui->progressBar->setValue(i_numberOfTruePackets);
//                ui->lcdNumber->display(i_numberOfTruePackets);
//                if(i_numberOfTruePackets==1)
//                {

//                            qDebug()<<"i_numberOfTruePackets"<<i_numberOfTruePackets;
//                             ui->tableWidget->setRowCount(i_numberOfTruePackets);
//                            QTableWidgetItem *newItem = new QTableWidgetItem();
//                            newItem->setText(QString::number(0));
//                            newItem->setTextAlignment(Qt::AlignHCenter);
//                            ui->tableWidget->setItem(i_numberOfTruePackets-1,0,newItem);
//                            i_previousTime = QTime::currentTime().msecsSinceStartOfDay();
//               }
//               else
//               {
//                            float f_time = (float)(QTime::currentTime().msecsSinceStartOfDay()-i_previousTime)/1000;

//                            f_AvirTime=f_AvirTime+f_time;


//                            float Av = f_AvirTime/((float)i_numberOfTruePackets-1.0);

//                            ui->label_AvirageTime->setText(QString::number(Av));

//                            ui->tableWidget->setRowCount(i_numberOfTruePackets);
//                            QTableWidgetItem *newItem = new QTableWidgetItem();
//                            newItem->setText(QString::number(f_time));
//                            newItem->setTextAlignment(Qt::AlignHCenter);
//                            ui->tableWidget->setItem(i_numberOfTruePackets-1,0,newItem);
//                            ui->tableWidget->setCurrentCell(i_numberOfTruePackets-1,0);
//                            i_previousTime = QTime::currentTime().msecsSinceStartOfDay();

//                            if(i_numberOfTruePackets == ui->progressBar->maximum())
//                            {

//                                i_numberOfTruePackets=0;
//                                i_numberOfPackets = 0;
//                            }
//               }
//        }
}

void MainWindow::on_action_TimeServer_triggered()
{
    QString fileName  = QFileDialog::getSaveFileName(this,tr("Сохранить отправленные сообщения КС"),"/home/Time_Server",tr("Текстовый файл(*.txt)"));              // получай имя файла в строковую переменную
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

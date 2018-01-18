#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QMessageBox>
#include <QHostAddress>
#include<QNetworkInterface>
#include<QFileDialog>
#include<ctime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    TcpServer_One = new QTcpServer (this);
    i_numberOfPackets =0;
    i_numberOfTruePackets = 0;
    f_AvirTime = 0.0;

    file = new QFile("server_par.txt");

    elapsedTimer = new QElapsedTimer();

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
            ui->plainTextEdit_IPAdrr->insertPlainText(address.toString()+"\n");
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



   if(i_numberOfPackets ==1)
    {
       vector_DeltaTime.clear();
       i_previousTime = QTime::currentTime().msecsSinceStartOfDay();
       vector_DeltaTime<<0;
       elapsedTimer->start();

    }
    else
    {
         quint64 i_CurrentTime = elapsedTimer->nsecsElapsed();
         vector_DeltaTime<<i_CurrentTime;
         elapsedTimer->start();
         qDebug()<<vector_DeltaTime.size();

    }
}



void MainWindow::on_action_TimeServer_triggered()
{
//    QString fileName  = QFileDialog::getSaveFileName(this,tr("Сохранить отправленные сообщения КС"),"/home/Time_Server",tr("Текстовый файл(*.txt)"));              // получай имя файла в строковую переменную
//    QFile  file(fileName);
//    if(!file.open(QIODevice::WriteOnly|QFile::WriteOnly))                                                           // если файл не получается открыть для записи
//    {
//          QMessageBox::warning(this,"Файл не сохранен",tr("Ошибка в сохранении файла: %1.").arg(file.errorString()));                                       // показывай сообщение об ошибке
//    }
//    else
//    {
//        QTextStream data( &file );
//        QStringList strList;                // лист из стринговых переменных

//        for( int r = 0; r < ui->tableWidget->rowCount(); ++r )
//        {
//                    strList.clear();
//                    strList<<"\" "+ui->tableWidget->item( r, 0 )->text()+"\" " ;                                                                           // цикл от нуля до максимального количества строк
//                     data << strList.join( ";" ) << "\n";
//        }
//    }
}

void MainWindow::on_pushButton_Clear_clicked()
{
    ui->textEdit->clear();
    ui->lcdNumber->display(0);
    i_numberOfPackets = 0;

}

void MainWindow::on_pushButton_Output_clicked()
{
    for(int i = 0;i<vector_DeltaTime.size();i++)
    {
        ui->textEdit->insertPlainText(QString::number((qlonglong)vector_DeltaTime[i])+"\n");
    }
}

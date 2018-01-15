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


   if(i_numberOfPackets ==1)
    {
         i_previousTime = QTime::currentTime().msecsSinceStartOfDay();

       ui->textEdit->insertPlainText(QString::number(0)+"\n");

    }
    else
    {
         int i_CurrentTime = QTime::currentTime().msecsSinceStartOfDay();
         int i_deltaTime = i_CurrentTime-i_previousTime;
         i_previousTime = i_CurrentTime;

         ui->textEdit->insertPlainText(QString::number(i_deltaTime)+"\n");


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

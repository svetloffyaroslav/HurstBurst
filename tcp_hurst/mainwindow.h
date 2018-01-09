#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include <QDateTime>
#include <QTimer>
#include <QTableWidgetItem>
#include <QIODevice>
#include <QDataStream>
#include <QMessageBox>
#include <QString>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkAddressEntry>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int i_numberPacket;
    int i_previousTime;


    QByteArray ByteArray_Send;
    bool b_firstSend;



private slots:
    void slot_TcpSocket_OneConnected();
    void slot_TcpSocket_OneReadyRead();
    void slot_TcpSocket_OneDisconnected();
    void slot_TcpSocket_OneError(QAbstractSocket::SocketError error_One);

    void on_pushButton_ConnectOrDisconnect_toggled(bool checked);  

    void on_pushButton_Transmit_toggled(bool checked);
    void slot_timeoutTimer_BetweenPacket();



    void on_action_SaveTime_triggered();

private:
    Ui::MainWindow*     ui;
    QTcpSocket*         TcpSocket_One;
    QTimer *            Timer_BetweenPacket;
};

#endif // MAINWINDOW_H

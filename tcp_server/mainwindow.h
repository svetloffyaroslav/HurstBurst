#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QHostAddress>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int i_numberOfPackets;
    int i_numberOfTruePackets;
    int i_previousTime;
    float f_AvirTime;
public slots:
     virtual void slotTcpServer_OneConnection();
private slots:
    void on_pushButton_ServerStart_clicked();
    void slot_TcpServer_OneClientReadyRead();
    void slot_TcpServer_OneClientIsDisconnect();


    void on_action_TimeServer_triggered();

private:
    Ui::MainWindow *ui;
    QTcpServer* TcpServer_One;
    QTcpSocket* TcpServerSocket_One;

};

#endif // MAINWINDOW_H

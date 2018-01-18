#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QHostAddress>
#include<QElapsedTimer>

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
    qint64 i_previousTime;
    float f_AvirTime;


      QVector<quint64> vector_DeltaTime;
public slots:
     virtual void slotTcpServer_OneConnection();
private slots:
    void on_pushButton_ServerStart_clicked();
    void slot_TcpServer_OneClientReadyRead();
    void slot_TcpServer_OneClientIsDisconnect();


    void on_action_TimeServer_triggered();

    void on_pushButton_Clear_clicked();

    void on_pushButton_Output_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer* TcpServer_One;
    QTcpSocket* TcpServerSocket_One;
    QFile *file;
    QElapsedTimer*      elapsedTimer;

};

#endif // MAINWINDOW_H

// ======================================================================
// mainwidow.cpp
// ======================================================================
//           This file is a part of programm Hurst Burst | TCP client v0.1
// ======================================================================
//  Author: Svetlov Yaroslav
//  vk.com : https://vk.com/id52304190
//
//  Contact
//  --------------
//  Email  : hurst_burst@gmail.com
//
//
//  Description:
//  This is the main part of programm.There has been described all algorithms
//  of interface, distributions, and tcp-socket. This programm works like TCP
//  client.
//
//  ---------------
//  Codec:  UTF-8
//  Data: 07.02.2018
// ======================================================================

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
#include <QVector>
#include <QDebug>
#include <QFileDialog>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include "qcustomplot.h"
#include <QLoggingCategory>
#include "setcolorandsettingsgraph.h"
#include <fftw3.h>
#include <fstream>
#include <complex>
#include <vector>
#include <math.h>
#include <QtMath>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_poly.h>
#include <iostream>
#include <random>
#include <thread>
#include <utility>



Q_DECLARE_LOGGING_CATEGORY(logDebug)
Q_DECLARE_LOGGING_CATEGORY(logWarning)
Q_DECLARE_LOGGING_CATEGORY(logCritical)
Q_DECLARE_LOGGING_CATEGORY(logPutInfo)

class setcolorandsettingsgraph;

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
    int i_line;

    QVector<float> vector_DeltaTime;
    QVector<double> vectordouble_HistData;
    QVector<double> vectordouble_TicksForHist;
    QVector<float> vector_FloatAfterDFT;



    // QVector<float> logspace(double i_min, double i_max, double i_count);

private slots:
    void slot_TcpSocket_OneConnected();
    void slot_TcpSocket_OneReadyRead();
    void slot_TcpSocket_OneDisconnected();
    void slot_TcpSocket_OneError(QAbstractSocket::SocketError error_One);
    void on_pushButton_ConnectOrDisconnect_toggled(bool checked);  

    void slot_timeoutTimer_BetweenPacket();
    void on_action_SaveTime_triggered();

    void StartTimerDelta(int i);

    void GenerateTime(int i_WhatGen);

    // void on_pushButton_DownloadFromTXT_clicked();

    void on_pushButton_Transmit_clicked();

    void sendToSocket();

    void on_listWidget_clicked(const QModelIndex &index);

    void on_pushButton_GenerateTime_clicked();

    void drawGraphic();

    void drawGraphicNoise();

    bool SaveGraphicsAsImage(QCustomPlot *widget_Graphic);

    void slot_clickedPointNoise(QMouseEvent* event);

    void slot_clickedHandClose(QMouseEvent*event);

    void on_toolButton_SaveNoise_clicked();

    void on_toolButton_SaveHist_clicked();

    void on_toolButton_SettingsOfHist_clicked();

    void on_toolButton_SettingsOfNoise_clicked();

    void slot_clickedHandOpen(QMouseEvent*event);

    void slot_setColorGraph(QString qstr_BackgroundColor,QString qstr_LineColor, QString qstr_PourColor);

    void slot_setColorHist(QString qstr_BackgroundColor,QString qstr_LineColor,QString qstr_PourColor);

    void on_toolButton_grab_handNoise_toggled(bool checked);

    void on_toolButton_CrossCursorXY_toggled(bool checked);

    void on_toolButton_resetHistogram_clicked();

    void on_spinBox_CountOfSteps_valueChanged(int arg1);



    void on_spinBox_CountOfStepsSRA_valueChanged(int arg1);

    void on_pushButton_PeriodogramMethod_clicked();

   // void autocorrelation(QVector<float> massive);

    // void VelvetAnalys();

    //void on_pushButton_Velvet_clicked();

private:

    Ui::MainWindow*     ui;
    QTcpSocket*         TcpSocket_One;
    QTimer *            Timer_BetweenPacket;
    setcolorandsettingsgraph * colorpanel_histogram;
    setcolorandsettingsgraph * colorpanel_noise;
    QCPBars *bars_packets;
};

#endif // MAINWINDOW_H

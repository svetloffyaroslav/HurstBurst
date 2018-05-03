// ======================================================================
// main.cpp
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
//  Data: 12.02.2018
// ======================================================================

#include "mainwindow.h"
#include <QApplication>
#include "qthread.h"
#include <QSplashScreen>
#include <QFile>
#include <QDir>
#include <QScopedPointer>
#include <QTextStream>
#include <QDateTime>
#include <QLoggingCategory>
#include <QString>
#include <QSound>
#include <QTest>
#include"test_gui.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>


QScopedPointer<QFile>   m_logFile;
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

/*//crete thread class
class I: public QThread
{
  public:
    // Forces the current thread to sleep
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};*/


int main(int argc, char *argv[])
{
    char* c_name_log= "testing_hurst_burst.log";

    FILE* LOG_F;
    LOG_F = fopen(c_name_log, "wt");

    freopen(c_name_log, "w", stdout);
    QApplication a(argc, argv);

    QTest::qExec(new test_gui, argc, argv);

    QString filePath = "logFile_HurstBurst.txt";
    QFile del(filePath);
    del.remove();
    m_logFile.reset(new QFile(filePath));
    m_logFile.data()->open(QFile::Append | QFile::Text);
    // qInstallMessageHandler(messageHandler);

    MainWindow w;
    //QSplashScreen *splash= new QSplashScreen;
    //splash->setPixmap(QPixmap(":/Images/Images/desiresplash.png"));
    // QSound::play((":/Audio/Audio/intro.wav"));
    //splash->show();
    //I::sleep(5);
    w.show();
    //splash->finish(&w);
    //delete splash;
    return a.exec();
    fclose(LOG_F);

}

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QTextStream out(m_logFile.data());
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");
    switch (type)
    {
        case QtDebugMsg:    out << "DBG "; break;
        case QtWarningMsg:  out << "WRN "; break;
        case QtCriticalMsg: out << "CRT "; break;
        case QtFatalMsg:    out << "FTL "; break;
    }
    out<<context.category<<":"<<msg.toUtf8()<<"\n";
    out.flush();
}

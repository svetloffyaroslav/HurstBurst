#ifndef TEST_GUI_H
#define TEST_GUI_H

#include <QObject>
#include"mainwindow.h"
#include "ui_mainwindow.h"



class test_gui : public QObject
{
    Q_OBJECT
public:
    explicit test_gui(QObject *parent = 0);

private slots: // должны быть приватными
    void test_lineEdit();
    void test_pushButton();
    void test_checkbox();

   private:
    Ui_MainWindow *panel;



};

#endif // TEST_H

#include "test_gui.h"
#include <QTest>
#include<QDebug>

test_gui::test_gui(QObject *parent):
    QObject(parent)
{

}

void test_gui::test_lineEdit()
{
    QLineEdit a;
    QTest::keyClicks(&a, "abCDEf123-");
    QCOMPARE(a.text(), QString("abCDEf123-"));
    QVERIFY(a.isModified());
}

void test_gui::test_pushButton()
{
    QPushButton s;
    s.setText(tr("OUT"));
    QCOMPARE(s.text(),QString("OUT"));
    s.setDown(true);
    QVERIFY(s.isDown());
}

void test_gui::test_checkbox()
{
//    QCheckBox v;
//    v.show();
//    QTest::keyClick(&v,Qt::LeftButton,Qt::NoModifier,-1);
//    qDebug()<<v.isChecked();
}



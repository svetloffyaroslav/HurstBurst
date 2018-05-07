#ifndef SETCOLORANDSETTINGSGRAPH_H
#define SETCOLORANDSETTINGSGRAPH_H

#include <QDialog>
#include<QColor>
#include<QFrame>
#include<QLineEdit>
#include<QColorDialog>



namespace Ui {
class setcolorandsettingsgraph;
}

class setcolorandsettingsgraph : public QDialog
{
    Q_OBJECT

public:
    explicit setcolorandsettingsgraph(QWidget *parent = 0);
    ~setcolorandsettingsgraph();

signals:
    void signal_setColorsGraph(QString ColorOfBackground,QString ColorOfLine,QString  SetColorOfPouring);

private slots:
    void on_toolButton_SetColorOfBackground_clicked();

    void on_pushButton_OK_clicked();

    void on_pushButton_Set_clicked();

    void on_pushButton_Close_clicked();


    void on_toolButton_SetColorOfLine_clicked();

    void setColor(QFrame *frame_color, QLineEdit *lineedit_RGBcolor, QString string_title);

    void on_toolButton_SetColorOfPouring_clicked();

private:
    Ui::setcolorandsettingsgraph *ui;
};

#endif // SETCOLORANDSETTINGSGRAPH_H

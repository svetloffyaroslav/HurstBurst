#include "setcolorandsettingsgraph.h"
#include "ui_setcolorandsettingsgraph.h"



setcolorandsettingsgraph::setcolorandsettingsgraph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::setcolorandsettingsgraph)
{
    ui->setupUi(this);
}

setcolorandsettingsgraph::~setcolorandsettingsgraph()
{
    delete ui;
}

void setcolorandsettingsgraph::on_toolButton_SetColorOfBackground_clicked()
{
  setColor(ui->frame_Background, ui->lineEdit_Phone,tr("Выбирите цвет для фона"));
}

void setcolorandsettingsgraph::on_toolButton_SetColorOfLine_clicked()
{
  setColor(ui->frame_line, ui->lineEdit_line,tr("Выбирите цвет линии"));
}

void setcolorandsettingsgraph::on_toolButton_SetColorOfPouring_clicked()
{
  setColor(ui->frame_pouring,ui->lineEdit_pouring,tr("Выбирите цвет заливки"));
}

void setcolorandsettingsgraph::setColor(QFrame *frame_color,QLineEdit *lineedit_RGBcolor, QString string_title)
{
    QColor background = QColorDialog::getColor(Qt::red,this,string_title,0);
    QString nameofcolor;
    nameofcolor.clear();
    nameofcolor.append("background-color: "+background.name()+";");
    frame_color->setStyleSheet(nameofcolor);
    lineedit_RGBcolor->setText(background.name());
}

void setcolorandsettingsgraph::on_pushButton_OK_clicked()
{ 
    close();
}

void setcolorandsettingsgraph::on_pushButton_Set_clicked()
{
    signal_setColorsGraph(ui->lineEdit_Phone->text(),ui->lineEdit_line->text(),ui->lineEdit_pouring->text());
}

void setcolorandsettingsgraph::on_pushButton_Close_clicked()
{
    close();
}





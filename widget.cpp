#include "widget.h"
#include "ui_widget.h"
#include "algorithms.h"
#include "draw.h"
#include <QFileDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

}


Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_clicked()
{
    ui->Canvas->switchSource();
}


void Widget::on_pushButton_2_clicked()
{
    //Get polygons and Boolean operation
    //TPolygon A = ui->Canvas->getA();
    //TPolygon B = ui->Canvas->getB();
    std::vector <TPolygon> pols = ui->Canvas->getPolygons();
    TBooleanOperation op = (TBooleanOperation)ui->comboBox->currentIndex();

    //Create overlay of polygons
    Algorithms a;
    int n = pols.size();
    TEdges res, res_;

    for (int i = 0; i < n-1; i++)
    {
        for (int j = 1; j < n; j++)
        {
            TPolygon A = pols[i];
            TPolygon B = pols[j];
            TEdges res_ = a.createOverlay(A, B, op);
            for (int k = 0; k < res_.size(); k++)
            {
                res.push_back(res_[k]);
            }
        }
    }

    //Set result
    ui->Canvas->setEdges(res);

    repaint();
}


void Widget::on_pushButton_3_clicked()
{
    //Clear results
    ui->Canvas->clear();
    repaint();
}


void Widget::on_pushButton_4_clicked()
{
    //Clear all
    ui->Canvas->clearAll();
    repaint();
}

void Widget::on_pushButton_5_clicked()
{
    //Open text file with dialog
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Text file"), "", tr("Text Files (*.txt)"));
    ui->Canvas->loadData(file_name);
    repaint();
}


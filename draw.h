#ifndef DRAW_H
#define DRAW_H

#include <QWidget>
#include "types.h"
#include "edge.h"
#include "sortbyx.h"
#include "sortbyy.h"

class Draw : public QWidget
{
    Q_OBJECT

private:
    TPolygon A, B;
    TEdges res;
    bool addA;
    double y_max = 0, x_min = 999999999; //pro transformaci
    double y_min = 999999999, x_max = 0; //pro meritko

public:
    explicit Draw(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void switchSource(){addA = !addA;}
    void drawPolygon(TPolygon &pol, QPainter &qp);
    TPolygon getA(){return A;}
    TPolygon getB(){return B;}
    void setEdges(TEdges &edg){res = edg;}
    void clear(){res.clear();}
    void clearAll(){A.clear(); B.clear(); res.clear();}
    void loadData(QString &file_name);

signals:

public slots:
};

#endif // DRAW_H

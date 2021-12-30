#include "draw.h"

Draw::Draw(QWidget *parent) : QWidget(parent)
{
    addA = true;
}

void Draw::paintEvent(QPaintEvent *event)
{
    QPainter qp(this);
    qp.begin(this);

    //Draw polygons
    drawPolygon(A, qp);
    drawPolygon(B, qp);

    //Draw edges
    qp.setPen(Qt::red);
    for(Edge e:res)
        qp.drawLine(e.getStart(), e.getEnd());

    qp.end();
}

void Draw::mousePressEvent(QMouseEvent *event)
{
    //Get coordinates
    int x = event->pos().x();
    int y = event->pos().y();

    //Create point
    QPointFBO p(x,y);

    //Add point to A, B
    if (addA)
        A.push_back(p);
    else
        B.push_back(p);

    //Update screen
    repaint();
}

void Draw::drawPolygon(TPolygon &polygon, QPainter &qp)
{
    //Draw polygon on canvas
    int r=4;
    QPolygon pol;

    //Convert polygon to QPolygon
    for (int i=0; i<polygon.size(); i++)
    {
       qp.drawEllipse(polygon[i].x()-r,polygon[i].y()-r,2*r,2*r);
       pol.append(QPoint(polygon[i].x(),polygon[i].y()));
    }

    //Draw polygon
    qp.drawPolygon(pol);
}

void Draw::loadData(QString &file_name)
{
    //Load data from the *.txt file
    int ab = 0;
    QPolygonF polygon;
    QPointFBO verticeA;
    QPointFBO verticeB;

    QFile inputFile(file_name);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            int id = line.split(" ")[0].toInt();
            double y = line.split(" ")[1].toDouble();
            double x = line.split(" ")[2].toDouble();

            if (id == 1)
                ab++;
            {
                if (ab == 1)
                {
                    //Add vertice to the end of the polygon A
                    verticeA.setX(x);
                    verticeA.setY(y);
                    A.push_back(verticeA);
                    polygon.push_back(QPointF(verticeA.x(),verticeA.y()));
                }
                else if (ab == 2)
                {
                    //Add vertice to the end of the polygon B
                    verticeB.setX(x);
                    verticeB.setY(y);
                    B.push_back(verticeB);
                    polygon.push_back(QPointF(verticeB.x(),verticeB.y()));
                }
            }

            //Find max Y and min X to determine origin of local system
            QPointF pymax = *std::max_element(polygon.begin(), polygon.end(), sortByY());
            double yy = pymax.y();
            QPointF pxmin = *std::min_element(polygon.begin(), polygon.end(), sortByX());
            double xx = pxmin.x();

            if (yy > y_max)
                y_max = yy;
            if (xx < x_min)
                x_min = xx;

            //Additionaly find min Y and max X to determine the scale in both directions
            QPointF pymin = *std::min_element(polygon.begin(), polygon.end(), sortByY());
            double yy_ = pymin.y();
            QPointF pxmax = *std::max_element(polygon.begin(), polygon.end(), sortByX());
            double xx_ = pxmax.x();

            if (yy_ < y_min)
                y_min = yy_;
            if (xx_ > x_max)
                x_max = xx_;
        }
    }

    //Compute scales to zoom in in canvas
    double canvas_weight = 952.0;
    double canvas_height = 748.0;

    double dy = fabs(y_max-y_min);
    double dx = fabs(x_max-x_min);

    double k;
    if (dy > dx)
        k = canvas_weight/dy;
    else
        k = canvas_height/dx;

    //Transform coordinates from JTSK to canvas
    for (int unsigned i = 0; i < A.size(); i++)
    {
        double temp = A[i].x();
        A[i].setX(-k*(A[i].y()-y_max)+11);
        A[i].setY(k*(temp-x_min)+11);
    }

    for (int unsigned i = 0; i < B.size(); i++)
    {
        double temp = B[i].x();
        B[i].setX(-k*(B[i].y()-y_max)+11);
        B[i].setY(k*(temp-x_min)+11);
    }
    inputFile.close();
}


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
    drawPolygon(polygons, qp);

    //Draw edges
    QPen fill_pen(Qt::red, 3);
    qp.setPen(fill_pen);

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

void Draw::drawPolygon(std::vector<TPolygon> &polygons, QPainter &qp)
{
    //Draw polygon on canvas
    int r=4;
    QPolygon pol;

    for (int j = 0; j < polygons.size(); j++)
    {
        TPolygon polygon = polygons[j];
        pol.clear();

        //Convert polygon to QPolygon
        for (int i=0; i<polygon.size(); i++)
        {
           qp.drawEllipse(polygon[i].x()-r,polygon[i].y()-r,2*r,2*r);
           pol.append(QPoint(polygon[i].x(),polygon[i].y()));
        }

        //Draw polygon
        qp.drawPolygon(pol);
    }

}

void Draw::loadData(QString &file_name)
{
    //Load data from the *.txt file
    TPolygon polygon;
    std::vector<TPolygon> polygons_;

    QPointFBO vertice;

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
            {
                if (polygon.empty() == false)
                    polygons_.push_back(polygon);
                polygon.clear();

                //Add vertice to the end of the QPointF vector
                vertice.setX(x);
                vertice.setY(y);
                polygon.push_back(vertice);
            }
            else
            {
                //Add vertice to the end of the QPointF vector
                vertice.setX(x);
                vertice.setY(y);
                polygon.push_back(vertice);
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

        //Save polygon to the vector of QPolygonFs
        polygons_.push_back(polygon);
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
    for (int unsigned i = 0; i < polygons_.size(); i++)
    {
        TPolygon pol = polygons_[i];
        for (int j = 0; j < pol.size(); j++)
        {
            double temp = pol[j].x();
            pol[j].setX(-k*(pol[j].y()-y_max));
            pol[j].setY(k*(temp-x_min));
        }

        polygons.push_back(pol);
    }
    inputFile.close();
}

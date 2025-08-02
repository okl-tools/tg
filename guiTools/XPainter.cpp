#include "Tools.h"


namespace guiTools
{

    XPainter::XPainter(QPaintDevice * p) : QPainter(p)
    {

    }

    void XPainter::drawXRect(const QRectF & r, qreal w, QColor col, Qt::PenStyle style)
    {
        save();
        setPen(QPen(col, w, style));

        QPainter::drawRect(r);

        restore();
    }
    void XPainter::drawXLine(QPointF pt1, QPointF pt2, qreal w, QColor col, Qt::PenStyle style)
    {
        save();
        setPen(QPen(col, w, style));

        QPainter::drawLine(pt1, pt2);

        restore();
    }

    void XPainter::drawXLine(qreal x1, qreal y1, qreal x2, qreal y2, qreal w, QColor col, Qt::PenStyle style)
    {
        save();
        setPen(QPen(col, w, style));

        QPainter::drawLine(QPointF(x1, y1), QPointF(x2, y2));

        restore();
    }


    void XPainter::drawCircle(QPointF pt, qreal radius, QColor col)
    {
        save();
        setPen(col);

        drawEllipse(pt, radius, radius);

        restore();
    }

    void XPainter::fillCircle(QPointF pt, qreal radius, QColor colBorder, qreal lineWidth, QColor colFill)
    {
        save();
        setPen(QPen(colBorder, lineWidth));

        QBrush brush(colFill);

        setBrush(brush);

        drawEllipse(pt, radius, radius);

        restore();
    }

    void XPainter::setAntiAlias(bool flagOn)
    {
        setRenderHint(XPainter::Antialiasing, flagOn);
    }




}
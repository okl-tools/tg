#include "Tools.h"

namespace guiTools
{
    XMatrix & XMatrix::translate(const QVector2D & v)
    {
        QTransform::translate(v.x(), v.y());
        return *this;
    }

    XMatrix & XMatrix::translate(const QPointF & p)
    {
        QTransform::translate(p.x(), p.y());
        return *this;
    }


    XMatrix & XMatrix::translate(qreal dx, qreal dy)
    {
        QTransform::translate(dx, dy);
        return *this;
    }

    XMatrix & XMatrix::scale(qreal factorX, qreal factorY)
    {
        QTransform::scale(factorX, factorY);
        return *this;
    }

    XMatrix & XMatrix::scale(qreal factor)
    {
        QTransform::scale(factor, factor);
        return *this;
    }

    XMatrix & XMatrix::rotate(qreal degrees)
    {
        QTransform::rotate(degrees);
        return *this;
    }

    double XMatrix::get_scale_x() const
    {
        return m11();
    }

    double XMatrix::get_scale_y() const
    {
        return m22();
    }

    QVector2D XMatrix::get_translation() const
    {
        return QVector2D(dx(), dy());
    }


}

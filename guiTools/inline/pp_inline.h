#pragma once

#include <QImage>
#include <QVector2D>

namespace okl
{
    template <>
    inline void write_object(std::stringstream & ss, const QString & o)
    {
        ss << o.toStdString();
    }
    template <>
    inline void write_object(std::stringstream & ss, const QPoint & pt)
    {
        ss << pt.x() << ", " << pt.y();
    }

    template <>
    inline void write_object(std::stringstream & ss, const QPointF & pt)
    {
        ss << pt.x() << ", " << pt.y();
    }

    template <>
    inline void write_object(std::stringstream & ss, const QRectF & r)
    {
        ss << r.x() << ", " << r.y() << ", " << r.width() << ", " << r.height();
    }
    template <>
    inline void write_object(std::stringstream & ss, const QRect & r)
    {
        ss << r.x() << ", " << r.y() << ", " << r.width() << ", " << r.height();
    }
    template <>
    inline void write_object(std::stringstream & ss, const QVector2D & v)
    {
        ss << v.x() << ", " << v.y();
    }

    template <>
    inline void write_object(std::stringstream & ss, const QImage & img)
    {
        if (img.isNull())
        {
            ss << "img-null";
        }
        else
        {
            ss << img.width() << ", " << img.height();

        }

    }
}

